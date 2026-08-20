#include "services/BaselineService.h"
#include "util/ResultConvert.h"

#include <cmath>
#include <json/json.h>
#include <set>
#include <unordered_map>

namespace joshua {

namespace {
// Index most-recent metrics by the (module_id, test_id) composite key.
std::unordered_map<std::string, std::string> index_metrics(
        const std::vector<LastMetricsEntry>& rows) {
    std::unordered_map<std::string, std::string> out;
    for (const auto& e : rows)
        out[e.module_id + "\x1f" + e.test_id] = e.metrics_json;
    return out;
}

// jsoncpp toStyledString() appends a trailing newline — strip it for clean output.
std::string strip_trailing_newline(const std::string& s) {
    if (!s.empty() && s.back() == '\n') return s.substr(0, s.size() - 1);
    return s;
}
}  // namespace

// static
std::string BaselineService::classify(int16_t baseline_status, int16_t current_status) {
    // Map each TestStatus to a monotone "badness" level so statuses are compared
    // by severity rather than by raw enum integer (SKIPPED=7 must not read as the
    // worst outcome, ABORTED=5 must not outrank FAIL=2).
    //   PASS/SKIPPED → 0   (non-failure)
    //   ABORTED      → 1
    //   FAIL/ERROR/TIMEOUT → 2
    auto level = [](int16_t s) -> int {
        switch (s) {
            case 1: case 7: return 0;              // PASS / SKIPPED
            case 5: return 1;                      // ABORTED
            case 2: case 3: case 4: return 2;      // FAIL / ERROR / TIMEOUT
            default: return 0;                     // UNSPECIFIED / unknown → treat as non-failure
        }
    };

    int bl = level(baseline_status);
    int cl = level(current_status);
    if (cl > bl) return "regressed";
    if (cl < bl) return "recovered";
    return "unchanged";   // drift is assessed separately via metric comparison
}

// static
std::vector<MetricDrift> BaselineService::compute_drifts(
        const std::string& baseline_metrics_json,
        const std::string& current_metrics_json,
        double /*drift_pct*/) {
    std::vector<MetricDrift> out;

    Json::Value base = util::parse_typed_map_json(baseline_metrics_json);
    Json::Value cur  = util::parse_typed_map_json(current_metrics_json);

    auto base_names = base.getMemberNames();
    auto cur_names  = cur.getMemberNames();
    std::set<std::string> keys(base_names.begin(), base_names.end());
    for (const auto& k : cur_names) keys.insert(k);

    for (const auto& key : keys) {
        MetricDrift d;
        d.key = key;

        bool in_base = base.isMember(key);
        bool in_cur  = cur.isMember(key);

        if (!in_base) {                        // key newly introduced
            d.baseline_value = "";
            d.current_value  = strip_trailing_newline(cur[key]["value"].toStyledString());
            out.push_back(d);
            continue;
        }
        if (!in_cur) {                         // key removed
            d.baseline_value = strip_trailing_newline(base[key]["value"].toStyledString());
            d.current_value  = "";
            out.push_back(d);
            continue;
        }

        const Json::Value& va = base[key];
        const Json::Value& vb = cur[key];
        std::string ta = va.get("type", "").asString();
        std::string tb = vb.get("type", "").asString();

        d.baseline_value = strip_trailing_newline(va["value"].toStyledString());
        d.current_value  = strip_trailing_newline(vb["value"].toStyledString());

        if ((ta == "double" || ta == "int") && (tb == "double" || tb == "int")) {
            double da = va["value"].asDouble();
            double db = vb["value"].asDouble();
            d.numeric = true;
            d.delta   = db - da;
            d.pct     = (std::abs(da) > 1e-15) ? ((db - da) / da * 100.0) : 0.0;
        }
        out.push_back(d);
    }
    return out;
}

std::optional<Run> BaselineService::find_baseline_for_mode(const std::string& mode) {
    return run_repo_.find_baseline_run(mode);
}

bool BaselineService::has_baseline(const std::string& run_id) {
    auto run = run_repo_.find_by_id(run_id);
    if (!run) return false;
    return run_repo_.find_baseline_run(run->mode).has_value();
}

std::vector<BaselineEntry> BaselineService::compare_to_baseline(
        const std::string& run_id, double drift_pct) {
    auto run = run_repo_.find_by_id(run_id);
    std::vector<BaselineEntry> out;
    if (!run) return out;

    auto baseline = run_repo_.find_baseline_run(run->mode);
    if (!baseline) return out;

    // Status/severity pairs from run_summaries (run_a = current, run_b = baseline).
    auto raw = result_repo_.find_for_compare(run_id, baseline->id);

    // Most-recent typed metrics per (module,test) for both runs.
    auto run_metrics  = index_metrics(result_repo_.find_last_metrics(run_id));
    auto base_metrics = index_metrics(result_repo_.find_last_metrics(baseline->id));

    for (const auto& e : raw) {
        BaselineEntry b;
        b.module_id = e.module_id;
        b.test_id   = e.test_id;
        b.baseline_status = e.run_b_status.value_or(0);   // baseline
        b.current_status  = e.run_a_status.value_or(0);   // current run

        std::string key = e.module_id + "\x1f" + e.test_id;
        std::string baseline_metrics = base_metrics.count(key) ? base_metrics.at(key) : "";
        std::string current_metrics  = run_metrics.count(key)  ? run_metrics.at(key)  : "";
        b.drifts = compute_drifts(baseline_metrics, current_metrics, drift_pct);

        // Appeared / disappeared take priority over status comparison.
        if (!e.run_b_status.has_value()) {
            b.classification = (b.current_status == 1) ? "new_pass" : "new_failure";
        } else if (!e.run_a_status.has_value()) {
            b.classification = "disappeared";
        } else {
            b.classification = classify(b.baseline_status, b.current_status);
            if (b.classification == "unchanged") {
                bool any_drift = false;
                for (const auto& d : b.drifts)
                    if (d.numeric && std::abs(d.pct) > drift_pct) { any_drift = true; break; }
                if (any_drift) b.classification = "drifted";
            }
        }
        out.push_back(b);
    }
    return out;
}

}  // namespace joshua
