# Mirage 2000-5 Open-Data JSBSim Package

This package provides the qttest runtime with an experimental, aircraft-specific
Mirage 2000-5 JSBSim model. It replaces the former F-16 dynamics approximation
for Mirage 2000 catalog entries.

The aerodynamic tables were generated with VSPAERO by Richard Harrison and are
imported from the GPL-2.0-or-later FlightGear Mirage 2000 project. The package
also includes that project's M53-P2 engine, flight-control, mass and supporting
system definitions. Original copyright and license notices are preserved.

For standalone use, qttest maps the FlightGear cockpit controls to native
JSBSim properties, assumes serviceable hydraulic circuits and models a clean
aircraft with the eight internal tanks feeding the engine directly. FlightGear
display controls, pushback, external tanks and the cockpit-driven fuel transfer
system are intentionally excluded. The aerodynamic coefficient data is not
altered.

This is not an official Dassault Aviation or Safran flight model. Public
manufacturer and French Ministry of Armed Forces data are used as validation
references, not as evidence that the complete model has certified or operational
fidelity. See `manifest.json` for exact provenance, hashes and limitations.

The package is discovered automatically by qttest. Selecting a Mirage 2000
visual model with JSBSim enabled chooses `mirage2000-open-data` and reports the
dynamics compatibility as `experimental`.
