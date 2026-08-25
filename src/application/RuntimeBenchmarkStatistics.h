#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <vector>

namespace application {

struct RuntimeBenchmarkStatistics {
  std::size_t sampleCount = 0;
  double minimumMilliseconds = 0.0;
  double meanMilliseconds = 0.0;
  double p50Milliseconds = 0.0;
  double p95Milliseconds = 0.0;
  double p99Milliseconds = 0.0;
  double maximumMilliseconds = 0.0;
};

inline double nearestRankPercentile(
    const std::vector<double>& sortedSamples,
    double percentile) {
  if (sortedSamples.empty()) {
    return 0.0;
  }
  const double boundedPercentile = std::clamp(percentile, 0.0, 1.0);
  const std::size_t rank = boundedPercentile <= 0.0
      ? 0
      : static_cast<std::size_t>(
            std::ceil(boundedPercentile * sortedSamples.size()) - 1.0);
  return sortedSamples.at(std::min(rank, sortedSamples.size() - 1));
}

inline RuntimeBenchmarkStatistics summarizeRuntimeDurations(
    const std::vector<double>& durationMilliseconds) {
  std::vector<double> samples;
  samples.reserve(durationMilliseconds.size());
  for (const double value : durationMilliseconds) {
    if (std::isfinite(value) && value >= 0.0) {
      samples.push_back(value);
    }
  }
  if (samples.empty()) {
    return {};
  }

  std::sort(samples.begin(), samples.end());
  RuntimeBenchmarkStatistics result;
  result.sampleCount = samples.size();
  result.minimumMilliseconds = samples.front();
  result.meanMilliseconds =
      std::accumulate(samples.begin(), samples.end(), 0.0) /
      static_cast<double>(samples.size());
  result.p50Milliseconds = nearestRankPercentile(samples, 0.50);
  result.p95Milliseconds = nearestRankPercentile(samples, 0.95);
  result.p99Milliseconds = nearestRankPercentile(samples, 0.99);
  result.maximumMilliseconds = samples.back();
  return result;
}

} // namespace application
