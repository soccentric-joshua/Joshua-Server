#pragma once
// services/BaselineService.h — Compare a run against the current baseline run.
//
// A run is marked as the baseline via RunService::set_baseline() (baseline_flag=TRUE
// on the runs table; one baseline per mode).  BaselineService compares any later
// run against that baseline at the (module_id, test_id) granularity, producing
// per-module status classifications and per-metric numeric drift.
//
// Classification rules (applied after the appeared/disappeared cases):
//   appeared    — test present in current run but not in the baseline.
//   new_pass    — appeared, and the current status is PASS.
//   new_failure — appeared, and the current status is a failure.
//   disappeared — present in baseline but not in the current run.
//   regressed   — status worsened (non-failure → aborted → failure) vs baseline.
//   recovered   — status improved vs baseline.
//   unchanged   — same status level and no metric drifted by > drift_pct%.
//   drifted     — same status but ≥1 numeric metric changed by > drift_pct%.
//
// Typed metrics are compared type-aware: numeric values produce delta / percent;
// string/bool values are reported as-is without a numeric delta.

#include "repositories/RunRepository.h"
#include "repositories/ResultRepository.h"

#include <optional>
#include <string>
#include <vector>

namespace joshua {

// Per-metric drift between the baseline and the current run.
struct MetricDrift {
    std::string key;
    std::string baseline_value;   // human-readable source value
    std::string current_value;    // human-readable current value
    double      delta  = 0.0;     // current - baseline (numeric only)
    double      pct    = 0.0;     // percent change (numeric only)
    bool        numeric = false;  // true when both sides are int/double
};

// Per-(module_id, test_id) baseline comparison result.
struct BaselineEntry {
    std::string module_id;
    std::string test_id;
    int16_t     baseline_status = 0;  // TestStatus enum integer (0 = absent)
    int16_t     current_status  = 0;  // TestStatus enum integer (0 = absent)
    std::string classification;       // see class doc
    std::vector<MetricDrift> drifts;
};

class BaselineService {
public:
    BaselineService() = default;

    // Returns the baseline run for a mode, if any.
    std::optional<Run> find_baseline_for_mode(const std::string& mode);

    // True if the run exists and its mode has an active baseline run.
    bool has_baseline(const std::string& run_id);

    // Compares the given run against the mode's baseline run.
    // Returns an empty vector if the run or a baseline cannot be found.
    std::vector<BaselineEntry> compare_to_baseline(const std::string& run_id,
                                                    double drift_pct = 5.0);

    // Static helpers (unit-testable without a DB).

    // Classify a status pair where both statuses are present (not appeared/
    // disappeared).  Returns regressed / recovered / unchanged.
    static std::string classify(int16_t baseline_status, int16_t current_status);

    // Compute per-key drift between two serialised typed-map JSON metrics blobs.
    static std::vector<MetricDrift> compute_drifts(
        const std::string& baseline_metrics_json,
        const std::string& current_metrics_json,
        double drift_pct);

private:
    RunRepository    run_repo_;
    ResultRepository result_repo_;
};

}  // namespace joshua
