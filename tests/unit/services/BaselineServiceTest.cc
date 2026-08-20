// tests/unit/services/BaselineServiceTest.cc — unit tests for the static
// classification / drift helpers of BaselineService.  compare_to_baseline()
// requires a live DB and is exercised by the integration suite instead.
#include "services/BaselineService.h"
#include <gtest/gtest.h>
#include <json/json.h>

using joshua::BaselineService;
using joshua::MetricDrift;

// ── classify ──────────────────────────────────────────────────────────────────

TEST(BaselineClassifyTest, BothPass_Unchanged) {
    EXPECT_EQ(BaselineService::classify(1, 1), "unchanged");
}

TEST(BaselineClassifyTest, PassToFail_Regressed) {
    EXPECT_EQ(BaselineService::classify(1, 2), "regressed");
}

TEST(BaselineClassifyTest, FailToPass_Recovered) {
    EXPECT_EQ(BaselineService::classify(2, 1), "recovered");
}

TEST(BaselineClassifyTest, PassToTimeout_Regressed) {
    EXPECT_EQ(BaselineService::classify(1, 4), "regressed");
}

TEST(BaselineClassifyTest, FailToTimeout_Unchanged_SameLevel) {
    // Both are failures at the same "badness" level; drift is checked separately.
    EXPECT_EQ(BaselineService::classify(2, 4), "unchanged");
}

TEST(BaselineClassifyTest, SkippedDoesNotOutrankPass) {
    // SKIPPED=7 must not be treated as worse than a PASS.
    EXPECT_EQ(BaselineService::classify(1, 7), "unchanged");
}

TEST(BaselineClassifyTest, AbortedToPass_Recovered) {
    EXPECT_EQ(BaselineService::classify(5, 1), "recovered");
}

TEST(BaselineClassifyTest, PassToAborted_Regressed) {
    EXPECT_EQ(BaselineService::classify(1, 5), "regressed");
}

// ── compute_drifts ────────────────────────────────────────────────────────────

TEST(BaselineDriftTest, NumericDrift_Computed) {
    std::string base = R"({"throughput_mbps":{"type":"double","value":100.0}})";
    std::string cur  = R"({"throughput_mbps":{"type":"double","value":110.0}})";
    auto drifts = BaselineService::compute_drifts(base, cur, 5.0);
    ASSERT_EQ(drifts.size(), 1u);
    const MetricDrift& d = drifts[0];
    EXPECT_TRUE(d.numeric);
    EXPECT_DOUBLE_EQ(d.delta, 10.0);
    EXPECT_DOUBLE_EQ(d.pct, 10.0);
}

TEST(BaselineDriftTest, NegativeDrift) {
    std::string base = R"({"latency_ms":{"type":"int","value":50}})";
    std::string cur  = R"({"latency_ms":{"type":"int","value":40}})";
    auto drifts = BaselineService::compute_drifts(base, cur, 5.0);
    ASSERT_EQ(drifts.size(), 1u);
    EXPECT_DOUBLE_EQ(drifts[0].delta, -10.0);
    EXPECT_DOUBLE_EQ(drifts[0].pct, -20.0);
}

TEST(BaselineDriftTest, ZeroBaseline_NoPercent) {
    std::string base = R"({"cpu":{"type":"double","value":0.0}})";
    std::string cur  = R"({"cpu":{"type":"double","value":0.5}})";
    auto drifts = BaselineService::compute_drifts(base, cur, 5.0);
    ASSERT_EQ(drifts.size(), 1u);
    EXPECT_DOUBLE_EQ(drifts[0].delta, 0.5);
    EXPECT_DOUBLE_EQ(drifts[0].pct, 0.0);   // division by ~zero guarded
}

TEST(BaselineDriftTest, StringValue_NotNumeric) {
    std::string base = R"({"mode":{"type":"string","value":"auto"}})";
    std::string cur  = R"({"mode":{"type":"string","value":"manual"}})";
    auto drifts = BaselineService::compute_drifts(base, cur, 5.0);
    ASSERT_EQ(drifts.size(), 1u);
    EXPECT_FALSE(drifts[0].numeric);
    EXPECT_EQ(drifts[0].baseline_value, "\"auto\"");
    EXPECT_EQ(drifts[0].current_value, "\"manual\"");
}

TEST(BaselineDriftTest, TypeMismatch_NotNumeric) {
    std::string base = R"({"x":{"type":"int","value":5}})";
    std::string cur  = R"({"x":{"type":"string","value":"5"}})";
    auto drifts = BaselineService::compute_drifts(base, cur, 5.0);
    ASSERT_EQ(drifts.size(), 1u);
    EXPECT_FALSE(drifts[0].numeric);
}

TEST(BaselineDriftTest, AddedKey) {
    std::string base = R"({"a":{"type":"double","value":1.0}})";
    std::string cur  = R"({"a":{"type":"double","value":1.0},"b":{"type":"int","value":7}})";
    auto drifts = BaselineService::compute_drifts(base, cur, 5.0);
    ASSERT_EQ(drifts.size(), 2u);
    EXPECT_TRUE(drifts[0].key == "a" || drifts[1].key == "a");
    for (const auto& d : drifts) {
        if (d.key == "b") {
            EXPECT_TRUE(d.baseline_value.empty());
            EXPECT_EQ(d.current_value, "7");
        }
    }
}

TEST(BaselineDriftTest, RemovedKey) {
    std::string base = R"({"a":{"type":"int","value":1},"b":{"type":"int","value":2}})";
    std::string cur  = R"({"a":{"type":"int","value":1}})";
    auto drifts = BaselineService::compute_drifts(base, cur, 5.0);
    ASSERT_EQ(drifts.size(), 2u);
    for (const auto& d : drifts) {
        if (d.key == "b") {
            EXPECT_EQ(d.baseline_value, "2");
            EXPECT_TRUE(d.current_value.empty());
        }
    }
}
