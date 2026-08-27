# MIXR Sensor Adapter

qttest builds a pinned MIXR revision on macOS and loads it through a small
native adapter library rather than exposing MIXR classes to the domain model.
The upstream Linux/POSIX implementation receives the minimal, idempotent
compatibility changes in `cmake/ApplyMixrPatch.cmake`; upstream source is not
vendored. The script uses exact source anchors and fails if the pinned source
no longer matches them.

Build the provider with:

```bash
cmake --build build-macos-debug --target qttest_setup_mixr
```

This produces
`build-macos-debug/sensor-plugins/libqttest_mixr_sensor.dylib`. Both VS Code
launch profiles build it and set `QTTEST_MIXR_SENSOR_PLUGIN` automatically.

The adapter exports these C symbols:

- `qttest_sensor_model_abi_version`
- `qttest_sensor_model_id`
- `qttest_sensor_model_evaluate`

The exact ABI is declared in
`src/infrastructure/sensors/SensorModelPluginApi.h`. The adapter uses MIXR
`RfEmission::setRange()` and the same two-way spherical range-loss composition
used by MIXR `IRadar` to derive a detection probability. qttest still owns the
geometric gates, global clock, and deterministic sampling.

To use a separately built adapter, set `libraryPath` for the `mixr` provider in
`Data/sensor_model_providers.json` or set `QTTEST_MIXR_SENSOR_PLUGIN` to the
resulting `.dylib`, `.so`, or `.dll`.

MIXR remains optional. If the adapter is missing, incompatible, or returns an
evaluation error, qttest keeps the requested provider in scenario data but
uses the native deterministic model for that evaluation. Only `base`,
`simulation`, and `models` are compiled; graphics, HLA, CIGI, and MIXR's
JSBSim integration are intentionally excluded.
