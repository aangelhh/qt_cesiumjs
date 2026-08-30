# External JSBSim Aircraft Sources

Status: Package loading implemented; candidate FDMs evaluated

## Purpose

This note records candidate flight dynamics models for visual aircraft that do
not have an exact counterpart in the upstream JSBSim aircraft catalog. A visual
model and an FDM are separate assets: matching their names is not enough to
claim representative dynamics.

The runtime remains responsible for simulation time. Any imported model must
load through the existing `JSBSimDynamicsModel` boundary, use the neutral
control contract, and retain the kinematic fallback.

## Source Policy

An external model is eligible only when all of the following are known:

- source URL and immutable revision;
- author and license;
- aircraft and engine configuration dependencies;
- JSBSim release and format compatibility;
- declared maturity (`ALPHA`, `BETA`, `TRIAL`, or equivalent);
- deterministic load and step results in the qttest JSBSim build.

External models must be kept outside the upstream JSBSim submodule. The runtime
now discovers isolated packages below `Dependencies/jsbsim-models`, plus roots
provided through `QTTEST_JSBSIM_MODEL_PATH`. A future Conan artifact can use the
same package contract: a root containing `aircraft`, `engine`, and optional
`systems` directories together with its license and provenance manifest.

## Candidate Matrix

| Platform | Candidate | Evidence | qttest JSBSim 1.3.2 | Decision |
| --- | --- | --- | --- | --- |
| Mirage 2000-5 | FGAddon `Mirage-2000/Mirage2000-vsp.xml` | JSBSim `ALPHA`, VSPAERO data, SNECMA M53-P2, GPL-2.0-or-later | Catalog and a 0.2 second run pass after the standalone adaptations listed below. | Preferred Mirage candidate; package as experimental and add control-response regression tests before binding it by default. |
| Mirage 2000C/RDI | FGAddon legacy `mirage2000/mirage2000.xml` | JSBSim `BETA`, derived from an F-16A Block 32 model and using F100-PW-229 | Loads and advances a short standalone run. One no-op product warning remains. License is not explicit in the FDM file. | Do not bind as an exact Mirage model. Keep only as historical fallback evidence. |
| F-35B | FGAddon `F-35B/F-35B-jsbsim.xml` | JSBSim `TRIAL`, Aeromatic/F-16-derived, GPL-3.0, F135 and lift-fan files included | Loads and advances after removing FlightGear-only pushback and declaring three auxiliary throttle inputs. | Importable as an explicitly experimental F-35B model after license packaging and regression tests. Do not reuse for F-35A/C without an approximation label. |
| Eurofighter Typhoon | FGAddon `eurofighter` | Available FDM files are YASim; FlightGear identifies the model as YASim GPLv2+. | Not loadable by JSBSim. | No exact JSBSim candidate. Create or obtain a separately validated JSBSim model. |
| Dassault Rafale | Historical FlightGear Rafale B | The author requested removal after Dassault prohibited redistribution. | Not evaluated and must not be imported. | Rejected. Build a clean-room/open-data model or use an explicitly labelled generic fighter approximation. |

## Smoke-Test Results

The checks used the JSBSim executable built from the repository submodule at
revision `506773ad3790a5a621445631613faf51a15284e7`.

### Legacy Mirage 2000

- canonical aircraft XML and existing `F100-PW-229`/`direct` dependencies load;
- `--catalog` exits successfully;
- a 0.2 second standalone run initializes and advances successfully;
- JSBSim reports a one-argument product expression that has no effect;
- its F-16 derivation makes it unsuitable as an exact production binding.

### Modern VSPAERO Mirage 2000

- the aircraft, SNECMA engine, mass model, and referenced systems were resolved;
- current JSBSim rejects `aero/c/CFYDRD` because it declares three lookup axes
  while providing only one table breakpoint;
- treating that data as the apparent two-dimensional alpha/rudder table allows
  model loading to continue;
- FlightGear-specific systems then reference properties not provided by the
  standalone qttest runtime, such as stores and pushback state;
- the FlightGear cursor channel is unrelated to flight dynamics and is removed;
- FlightGear control properties are mapped to standard JSBSim FCS command
  properties;
- external-store drag areas default to zero and hydraulic/electrical
  availability receives explicit standalone defaults;
- after those adaptations, both `--catalog` and a 0.2 second run exit
  successfully;
- the result remains an `ALPHA` model and requires control-response,
  deterministic-step, and fuel tests before production use.

### F-35B

- aircraft, F135 engine, direct/lift/side thrusters, and four propulsion units
  are discovered;
- `--catalog` loads with the original source files;
- standalone stepping initially fails on FlightGear pushback state and three
  undeclared throttle properties;
- removing the pushback system and declaring the auxiliary throttle inputs
  allows a 0.2 second standalone run to initialize and advance successfully;
- the source remains `TRIAL` and F-16/Aeromatic-derived, so it is experimental.

## Required Integration Work

1. External JSBSim package discovery outside `Dependencies/jsbsim`: implemented.
2. Extend model discovery and loading to select an aircraft package root while
   retaining the upstream catalog as fallback. This runtime support is now
   implemented through `JsbsimModelRepository`.
3. Add a provenance manifest with source revision, license, maturity, supported
   variants, dependencies, and known adaptations.
4. Create a standalone Mirage profile containing only required FDM systems and
   explicit neutral-runtime properties.
5. Add load, initialization, deterministic step, control response, fuel, and
   finite-state tests for every imported model.
6. Bind a visual platform only after the candidate passes those tests; expose
   `experimental` or `approximation` compatibility in the entity UI.

## Package Contract

An external package is discovered in either of these forms:

```text
Dependencies/jsbsim-models/<package>/
  aircraft/<modelId>/<modelId>.xml
  engine/*.xml
  systems/*.xml                 # optional
  manifest.json                 # planned provenance metadata
  LICENSE                       # required before distribution
```

Additional package roots or containers can be supplied with the platform path
separator in `QTTEST_JSBSIM_MODEL_PATH`. External packages take precedence over
the upstream root, and duplicate model IDs are diagnosed instead of silently
replacing a previously discovered model.

The evaluated source snapshots have these SHA-256 values:

- Mirage 2000 VSPAERO at FGAddon r16906:
  `69db1ffa4daacd7969ae3f0d59f8af3358f1539cfa1b4ef576b0d8e38ce01b52`;
- F-35B retrieved from FGAddon HEAD on 2026-08-30:
  `0f1cbe9f4d0232c3cbd36a64f1cfe046af947834a68e6053ddaa3eb7aa389e7d`.

The actual GPL model files are intentionally not committed in this change.
Their artifact must include the complete applicable license, original source,
adaptation patch, provenance manifest, and reproducible smoke-test evidence.

## Sources

- FlightGear FGAddon: https://sourceforge.net/p/flightgear/fgaddon/HEAD/tree/trunk/Aircraft/
- Modern Mirage 2000: https://sourceforge.net/p/flightgear/fgaddon/HEAD/tree/trunk/Aircraft/Mirage-2000/
- Legacy Mirage 2000: https://sourceforge.net/p/flightgear/fgaddon/HEAD/tree/trunk/Aircraft/mirage2000/
- F-35B: https://sourceforge.net/p/flightgear/fgaddon/HEAD/tree/trunk/Aircraft/F-35B/
- Eurofighter: https://sourceforge.net/p/flightgear/fgaddon/HEAD/tree/trunk/Aircraft/eurofighter/
- Rafale removal request: https://sourceforge.net/p/flightgear/mailman/message/29947758/
