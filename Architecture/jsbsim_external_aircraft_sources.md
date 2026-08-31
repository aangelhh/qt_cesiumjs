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

## Current Visual Platform Bindings

The 3D catalogue can declare dynamics defaults once per platform family. Those
defaults are inherited by every available visual variant in that family, and
are exposed on the entity as `Dynamics Model Match`:

- `family`: the JSBSim configuration represents the same aircraft family;
- `approximation`: a known, runnable configuration of the same broad class;
- `experimental`: an open-data package that runs but still has documented
  fidelity limitations;
- `custom`: the operator selected a different JSBSim configuration manually.

The Typhoon visual family, including the F-2000A catalogue entry, now selects
the local `eurofighter-typhoon` open-data package and presents `experimental`
in the entity details. Its twin
EJ200 propulsion, dimensions, mass and fuel data are public and traced in its
manifest; its control and aerodynamic baseline is inherited from the GPL F-16
model. The Rafale family similarly selects the independent
`rafale-open-data` package with public Dassault/Safran data and an inherited
GPL F-16 aerodynamic baseline. No content from the removed FlightGear Rafale B
addon is present. F-35, Hornet and MiG-29 visual platforms continue to use the
`f16` fighter approximation; Mirage F-1 and Mirage III use `f104`; A400M uses
`C130` as a transport approximation; B-2, B-52 and B-1B use `B747`; and Chinook/Tiger
use the `ah1s` rotorcraft approximation. Engine count and cockpit systems
profile remain those of the visual platform, not those of the approximate FDM.

This makes the aircraft immediately runnable with JSBSim while avoiding a
false claim of model fidelity. The external Mirage 2000 and F-35B candidates
remain separate experimental packages until their complete license,
provenance, adaptation patch and regression evidence are committed together.

## Candidate Matrix

| Platform | Candidate | Evidence | qttest JSBSim 1.3.2 | Decision |
| --- | --- | --- | --- | --- |
| Mirage 2000-5 | FGAddon `Mirage-2000/Mirage2000-vsp.xml` | JSBSim `ALPHA`, VSPAERO data, SNECMA M53-P2, GPL-2.0-or-later | Catalog and a 0.2 second run pass after the standalone adaptations listed below. | Preferred Mirage candidate; package as experimental and add control-response regression tests before binding it by default. |
| Mirage 2000C/RDI | FGAddon legacy `mirage2000/mirage2000.xml` | JSBSim `BETA`, derived from an F-16A Block 32 model and using F100-PW-229 | Loads and advances a short standalone run. One no-op product warning remains. License is not explicit in the FDM file. | Do not bind as an exact Mirage model. Keep only as historical fallback evidence. |
| F-35B | FGAddon `F-35B/F-35B-jsbsim.xml` | JSBSim `TRIAL`, Aeromatic/F-16-derived, GPL-3.0, F135 and lift-fan files included | Loads and advances after removing FlightGear-only pushback and declaring three auxiliary throttle inputs. | Importable as an explicitly experimental F-35B model after license packaging and regression tests. Do not reuse for F-35A/C without an approximation label. |
| Eurofighter Typhoon | qttest `eurofighter-typhoon` | Public Eurofighter/EUROJET data plus the GPL JSBSim F-16A baseline; FGAddon YASim is validation-only. | Loads through the external package root; runtime and deterministic tests required for every revision. | Bind as `experimental`, never as an exact Typhoon FDM, until aircraft-specific coefficients and control laws are validated. |
| Dassault Rafale | qttest `rafale-open-data` | Public Dassault/Safran data plus the GPL JSBSim F-16A baseline. The historical FlightGear Rafale B is explicitly excluded. | Loads through the external package root; twin-engine telemetry, fuel and deterministic runtime checks apply. | Bound as `experimental`; aerodynamic tables and control laws remain generic until independently validated. |

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
   variants, dependencies, and known adaptations: implemented for local
   Typhoon and Rafale packages.
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
- Rafale specifications: https://www.dassault-aviation.com/en/defense/rafale/specifications-and-performance-data/
- M88 engine: https://www.safran-group.com/products-services/m88-proven-performance-and-reliability
- Rafale removal request: https://sourceforge.net/p/flightgear/mailman/message/29947758/
