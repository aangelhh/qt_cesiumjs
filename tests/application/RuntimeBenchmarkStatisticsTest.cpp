#include <gtest/gtest.h>

#include "application/RuntimeBenchmarkStatistics.h"

#include <limits>
#include <vector>

TEST(RuntimeBenchmarkStatistics, EmptyInputProducesZeroSummary) {
  const auto result = application::summarizeRuntimeDurations({});
  EXPECT_EQ(result.sampleCount, 0U);
  EXPECT_DOUBLE_EQ(result.meanMilliseconds, 0.0);
  EXPECT_DOUBLE_EQ(result.p95Milliseconds, 0.0);
}

TEST(RuntimeBenchmarkStatistics, ComputesNearestRankPercentilesAndMean) {
  std::vector<double> samples;
  for (int value = 1; value <= 100; ++value) {
    samples.push_back(static_cast<double>(value));
  }

  const auto result = application::summarizeRuntimeDurations(samples);
  EXPECT_EQ(result.sampleCount, 100U);
  EXPECT_DOUBLE_EQ(result.minimumMilliseconds, 1.0);
  EXPECT_DOUBLE_EQ(result.meanMilliseconds, 50.5);
  EXPECT_DOUBLE_EQ(result.p50Milliseconds, 50.0);
  EXPECT_DOUBLE_EQ(result.p95Milliseconds, 95.0);
  EXPECT_DOUBLE_EQ(result.p99Milliseconds, 99.0);
  EXPECT_DOUBLE_EQ(result.maximumMilliseconds, 100.0);
}

TEST(RuntimeBenchmarkStatistics, IgnoresInvalidAndNegativeDurations) {
  const std::vector<double> samples = {
      4.0,
      -1.0,
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::infinity(),
      2.0,
  };

  const auto result = application::summarizeRuntimeDurations(samples);
  EXPECT_EQ(result.sampleCount, 2U);
  EXPECT_DOUBLE_EQ(result.minimumMilliseconds, 2.0);
  EXPECT_DOUBLE_EQ(result.meanMilliseconds, 3.0);
  EXPECT_DOUBLE_EQ(result.maximumMilliseconds, 4.0);
}
