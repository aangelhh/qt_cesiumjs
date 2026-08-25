# JSBSim Runtime Benchmark

## Purpose

Measure the steady-state cost and scaling behavior of the JSBSim integration
without Qt Widgets, Cesium, rendering, sensors, weapons, or AI. The same tool
also measures the current kinematic runtime as a control baseline.

The benchmark is evidence, not a hardware-independent pass/fail test. CTest
only verifies the deterministic statistics calculations and runtime behavior.

## Protocol

- Clock: `std::chrono::steady_clock`.
- JSBSim diagnostic verbosity: disabled inside the benchmark process.
- Runtime path: `FlightDynamicsEngine::advanceEntities`.
- Simulation step: configurable, 60 Hz by default.
- Initial model load plus first tick: measured separately.
- Warm-up: 60 ticks by default.
- Measurement: 300 ticks by default.
- Setpoints: deterministic heading, altitude, and speed phases.
- Timing fallback: observed but not enforced, so machine load cannot alter the
  simulated trajectory during the benchmark.
- Default scaling cases: 1, 5, 10, and 25 entities.
- Reported metrics: mean, p50, p95, p99 and maximum tick time, mean time per
  entity, entity-steps/second, real-time factor, 30/60 Hz sustainability, and
  unexpected fallback count.
- JSON metadata: UTC timestamp, build type, OS, kernel, CPU architecture, Qt
  version, and the complete benchmark configuration.

## Build And Run

```bash
cmake --build build-macos-debug -j2 --target qttest_jsbsim_runtime_benchmark
build-macos-debug/benchmarks/qttest_jsbsim_runtime_benchmark \
  --entities 1,5,10,25 \
  --backend both \
  --warmup 60 \
  --ticks 300 \
  --hz 60 \
  --output /tmp/jsbsim-runtime-benchmark.json
```

Use `--entities 1,5,10,25,50` for a wider stress run. A benchmark run returns
non-zero only for invalid arguments, report-write failure, or an unexpected
backend fallback. Missing a 30/60 Hz target is reported as data and does not
make the command fail.

## Interpretation

- `init_ms` includes model creation, aircraft load, initial conditions, and the
  first simulation tick. It is intentionally excluded from steady-state data.
- `p95_ms <= 16.67` means the complete group sustained a 60 Hz wall-clock
  budget for at least 95 percent of measured ticks on that machine.
- `rt_factor >= 1.0` means the requested simulated time advanced at least as
  fast as wall time.
- Results from Debug and Release builds must not be compared as equivalent.
- Baselines should record hardware, OS, compiler, build type, JSBSim revision,
  command line, and repository commit.

## Reference Baseline

Recorded on 2026-08-25 with:

- MacBook Air `Mac16,12`, Apple M4 (10 cores), 16 GB RAM.
- macOS 26.5.1, arm64.
- AppleClang 21.0.0, Qt 6.10.2.
- Release build, JSBSim F-16 model.
- 60 Hz simulation step, 120 warm-up ticks, 1000 measured ticks.
- Sequential `FlightDynamicsEngine` execution with deterministic timing policy.

| Backend | Entities | Init ms | Mean tick ms | P95 ms | P99 ms | Mean ms/entity | Entity steps/s | RT factor | Fallbacks |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Kinematic | 1 | 0.01 | 0.001 | 0.001 | 0.001 | 0.0008 | 1,269,635 | 21,160.58 | 0 |
| Kinematic | 5 | 0.01 | 0.004 | 0.004 | 0.004 | 0.0008 | 1,330,079 | 4,433.60 | 0 |
| Kinematic | 10 | 0.01 | 0.007 | 0.007 | 0.008 | 0.0007 | 1,402,598 | 2,337.66 | 0 |
| Kinematic | 25 | 0.02 | 0.016 | 0.017 | 0.017 | 0.0007 | 1,518,419 | 1,012.28 | 0 |
| Kinematic | 50 | 0.04 | 0.029 | 0.031 | 0.033 | 0.0006 | 1,715,465 | 571.82 | 0 |
| JSBSim F-16 | 1 | 2.67 | 0.014 | 0.014 | 0.016 | 0.0140 | 71,242 | 1,187.36 | 0 |
| JSBSim F-16 | 5 | 10.21 | 0.077 | 0.086 | 0.104 | 0.0154 | 64,951 | 216.50 | 0 |
| JSBSim F-16 | 10 | 20.49 | 0.152 | 0.166 | 0.192 | 0.0152 | 65,847 | 109.75 | 0 |
| JSBSim F-16 | 25 | 65.67 | 0.383 | 0.420 | 0.459 | 0.0153 | 65,242 | 43.49 | 0 |
| JSBSim F-16 | 50 | 104.71 | 0.775 | 0.862 | 0.932 | 0.0155 | 64,531 | 21.51 | 0 |

All reference cases remained below the 16.67 ms 60 Hz budget at p95. The
JSBSim steady-state cost was approximately 0.015 ms per entity and scaled
nearly linearly through 50 entities on this machine.

This baseline characterizes flight dynamics only. It must not be interpreted
as total scenario capacity because rendering, Cesium synchronization, sensors,
weapons, networking, recording, and AI are intentionally excluded.
