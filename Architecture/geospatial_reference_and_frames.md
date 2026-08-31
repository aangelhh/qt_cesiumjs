# Geospatial Reference and Frame Policy

## Purpose

The simulation has one runtime geospatial authority: `geospatial::IGeospatialService`.
The default implementation, `GeographicLibGeospatialService`, uses WGS84 geodesics
through the pinned GeographicLib-C source in `Dependencies/geographiclib-c`.

`domain::GeoMath` remains the compatibility facade used by existing movement,
sensor, targeting and munition code. New runtime code should use that facade for
simple distance, bearing and destination queries, or `IGeospatialService` when a
coordinate-frame conversion is required.

## Canonical conventions

| Value | Convention |
| --- | --- |
| Geodetic latitude / longitude | WGS84 decimal degrees; latitude north-positive, longitude east-positive |
| Geodetic altitude | Ellipsoidal meters, positive away from the WGS84 ellipsoid |
| ECEF | WGS84 Earth-Centered, Earth-Fixed Cartesian meters (`x`, `y`, `z`) |
| ENU | Local tangent frame in meters: east, north, up |
| NED | Local tangent frame in meters: north, east, down |
| Heading / bearing | Degrees clockwise from true north, normalized to `[0, 360)` |
| Distance | Meters along the WGS84 ellipsoid unless explicitly named `slantDistanceMeters` |

The terrain surface is not yet an altitude datum. Ground entities retain their
configured safe altitude until terrain services are introduced; this policy does
not clamp their altitude to zero.

## Integration boundaries

- Tasks, sensor geometry, kinematic motion and munition propagation use the
  shared WGS84 geodesic functions.
- Cesium JavaScript may retain local visual helpers, but it is not an
  authoritative source for simulation coordinates or measurements.
- JSBSim, DIS, HLA, DDS and future external-model adapters must declare their
  input and output frame in their manifest and convert at the adapter boundary.
  No external frame convention may leak into `Entity` state.
- WGS84 geodetic state remains the common `Entity` representation. ECEF, ENU
  and NED are transient conversion products.

## Dependency policy

The project vendors GeographicLib-C `v2.2` as the two-file standalone geodesic
implementation (`geodesic.c` and `geodesic.h`). The exact source checksums and
MIT/X11 license are recorded in `Dependencies/geographiclib-c/README.md`.
This intentionally small pin avoids a platform package dependency while the
future Conan artifact-management task defines a wider third-party packaging
policy.

## Scope and follow-up

This foundation standardizes Earth-reference math only. It does not introduce
terrain following, collision detection, geoid/mean-sea-level conversion,
geodesic path planning, or a rewrite of inactive third-party HLA/DIS adapters.
Those capabilities must build on these conventions rather than add another
independent distance or frame implementation.
