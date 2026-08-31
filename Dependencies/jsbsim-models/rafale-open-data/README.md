# Dassault Rafale Open-Data Baseline

This is an experimental JSBSim package for the Rafale visual platform. It is
an independently assembled open-data baseline, not an official or validated
Dassault Aviation flight model.

The package derives its generic fighter control law, ground-contact geometry
and aerodynamic tables from the GPL JSBSim F-16A model. Public Rafale data is
applied to dimensions, empty mass, fuel capacity and twin M88 propulsion.
`manifest.json` records the sources, derivations and known limitations.

No file, coefficient table, model, texture or other content from the removed
historical FlightGear Rafale B addon is included. The qttest Rafale GLB assets
are separate visual artifacts and still require their own provenance and
redistribution audit.

The package is discovered automatically by qttest. Selecting a Rafale visual
model with JSBSim enabled chooses `rafale-open-data` and exposes the dynamics
match as `experimental`.
