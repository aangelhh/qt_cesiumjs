# Eurofighter Typhoon Open-Data Baseline

This is an experimental JSBSim package for the Eurofighter Typhoon visual
platform. It is deliberately an open-data baseline, not a claim of an exact
aircraft flight model.

The package derives its generic fighter control law and aerodynamic tables from
the GPL JSBSim F-16A model. Eurofighter-specific public data is applied only to
the dimensions, empty mass, fuel capacity and twin EJ200 propulsion values.
`manifest.json` records every source, derivation and known limitation.

The FlightGear FGAddon YASim Eurofighter is GPL and is not copied here. It may
be used as an external qualitative validation reference only. Any future
Eurofighter-specific aerodynamic tables must carry their own source and license
evidence before replacing the inherited baseline.

The package is discovered automatically by qttest. Select a Typhoon visual
model, enable JSBSim flight dynamics, and the `eurofighter-typhoon` model is
selected as an `experimental` dynamics match.
