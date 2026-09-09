# DIS Gateway MVP

## Validation

Release preparation on 2026-09-09: application, periodic tester and connection
panels built on macOS. `ctest --test-dir build-macos-debug --output-on-failure
-j 6` completed with 718 passed, 2 optional live Pitch integration tests skipped,
and no failures (720 registered tests). This includes real UDP exchange between
test sessions, a separate tester process sending all 11 PDU types twice, changing
Entity State positions, and both HLA/DIS panel smoke tests.

Use CTest to run the full suite: GUI fixtures require isolation and running the
entire GoogleTest binary in one process can abort on Qt application lifetime.
These results do not certify compatibility with an independent DIS simulator.

## Scope

The DIS gateway exchanges entity kinematics, weapon events, simulation controls,
and radar emissions using DIS 7 PDUs over IPv4 unicast or multicast UDP.

KDIS is pinned as a Git submodule and is used only for standards-compliant PDU
encoding and decoding. Qt Network owns the UDP transport. The existing
GeographicLib-backed geospatial service owns WGS84/ECEF conversions.

## Runtime Flow

1. The startup dialog selects DIS and configures address, port, exercise ID,
   site ID, application ID, and remote timeout.
2. `DisStartupSession` assigns a stable 16-bit DIS entity number to each local
   `Entity.entityId` for the lifetime of the session.
3. Local entities are converted to Entity State PDUs and published at 10 Hz.
4. A newly active munition emits one Fire PDU. Its impact effect emits one
   correlated Detonation PDU. There is no repeated Fire publication per tick.
5. `DisGateway` receives UDP datagrams and decodes the supported PDU
   types. Unsupported PDU types are ignored so they do not disrupt a mixed
   DIS exercise.
6. `DisRemoteEntityRegistry` rejects other exercises and this application's
   own PDUs, then tracks accepted remote entities by `(site, application,
   entity)`.
7. `MainWindow` applies received entity state through
   `ScenarioState::upsertExternalEntity`. These entities are marked
   `externallyControlled`, so local dynamics do not advance or republish them.
8. Received Fire and Detonation events are shown and logged, but deliberately
   do not trigger local weapon logic or damage.
9. A remote entity is removed after its configured update timeout.

## Runtime Panel And PDU Tester

`View > DIS Gateway` opens a dockable or floating runtime panel. The status-bar
indicator exposes the same connection state without requiring the panel to be
open.

The panel supports:

- opening and closing the configured UDP session;
- live transmitted/received PDU and remote-entity counters;
- sending a configurable DIS 7 Entity State PDU through the active gateway;
- inspecting the most recent entity state, expiry, Fire, and Detonation events;
- retaining at most 200 receive rows to keep long runs bounded.

The HLA and DIS transports are mutually exclusive. When one is active, the
other panel remains visible for diagnostics but its controls are disabled.
This rule is enforced both in the UI and by the runtime connection handlers.

To verify exchange with two qttest processes, give them different site or
application IDs and the same address, port, and exercise ID. Send a test PDU
from one process; the second process should increment RX, add a table row, and
create or update the corresponding externally controlled entity.

## Entity State Mapping

| qttest | DIS Entity State PDU |
| --- | --- |
| Stable runtime identity | Entity Identifier tuple |
| Display name/callsign | Entity Marking (11 ASCII characters) |
| Force | Force ID |
| DIS kind/domain/country/category tuple | Entity Type |
| WGS84 latitude/longitude/altitude | Geocentric world location (ECEF) |
| Heading/pitch/roll | Geocentric Euler orientation |
| Speed/heading/vertical speed | ECEF linear velocity |
| Damage/destroyed | Platform appearance damage bits |

The DIS Entity Identifier is authoritative for remote identity. Entity Marking
is display-only and is not assumed to be unique.

## Warfare Mapping

| qttest | DIS warfare PDU |
| --- | --- |
| First appearance of an active munition | One Fire PDU |
| Launcher and target stable references | Firing/Target Entity Identifier |
| Runtime munition identity | Munition Entity Identifier |
| Launch-to-impact correlation | Event Identifier shared by Fire and Detonation |
| Missile or bomb | Munition descriptor category |
| WGS84 position | Geocentric world location (ECEF) |
| Heading, pitch, and speed | ECEF velocity vector |
| Missile entity impact | Detonation result 1 |
| Bomb ground impact | Detonation result 3 |

Inbound warfare events are observational in this increment. Applying remote
damage requires an explicit ownership and authority policy and is intentionally
not inferred merely from receiving a Detonation PDU.

## Deferred Work

- Configurable dead-reckoning algorithms and smoothing.
- Capture/replay and interoperability tests with an external DIS application.

## Simulation Control And Radar

`DisStartupSession.ExchangesControlsAndRadarLifecycleOverUdp` exercises two
sessions on a temporary UDP multicast port. It verifies all three controls,
source echo filtering, stable radar IDs, emission deactivation, and the empty
snapshot after deleting the last sensor. The test requires permission to open
UDP sockets and a host with IPv4 multicast support. This is an in-process
transport integration test, not independent external-simulator certification.

Local Play publishes Start/Resume; Pause and Stop publish Stop/Freeze with
Recess and Termination reasons. Remote commands use the existing lifecycle
controller with publication suppressed. Commands execute on receipt; future
real-world scheduling and clock alignment are not implemented.
The destination is the application (entity zero) or broadcast
(65535). Other exercises, the local source, other destinations, and duplicate
source/request/type tuples in a bounded 1024-entry history are filtered.

Accepted controls receive an Acknowledge PDU (type 15) with the original request
ID, the source as destination, and AbleToComply. This confirms gateway acceptance,
not completion by the simulation controller. Duplicate controls are acknowledged
again without being applied again. Acknowledgements do not themselves receive
acknowledgements. Incoming responses must match a locally published request,
exercise, exact destination, and control flag. One response per remote entity
is retained for multicast requests, with bounded request and responder history.
Responses are exposed by the session and logged by the application. Automatic
retries and response deadlines are not implemented in this increment.

Radar emissions publish at 10 Hz alongside entity states. Sensor IDs retain
stable emitter numbers for the session; each local radar has one search beam.
Disabled or destroyed sensors publish deactivated beams. Full incoming
snapshots replace DIS sensors on the external host; changed-data updates
replace only the included systems. Emissions received before their host entity
are applied on a subsequent heartbeat. Remote radar sensors are not republished.

The mapping includes frequency, bandwidth, scan centers and half-angle sweeps,
and radiated power derived from transmitter peak power and antenna gain. See
the [DIS parameter definition](https://faculty.nps.edu/brutzman/vrtp/mil/navy/nps/disEnumerations/JdbeHtmlFiles/pdu/c3.htm).
Range is not conveyed and is represented as zero on reception. Pulse width,
PRF, scan phase, antenna location, emitter model, and track/jam targets are not
currently modeled in this mapping. Received radiated power is stored with zero
antenna gain because transmitter power cannot be recovered independently.

## Create/Remove, IFF, And Collision

Create Entity (11) and Remove Entity (12) are addressed to a specific local
site/application/entity, with IDs 1-32767 available for managed entities.
The application creates a neutral, stationary placeholder at 0/0 because the
request contains no model or initial kinematics. Its stable key is
`managed:dis:site:application:entity`; publication retains the requested ID.
Existing local entities are protected. Only the originating manager can remove
an entity it created. Requests receive AbleToComply or UnableToComply after
application, and duplicates reuse the cached response. Other peers expire their
last Entity State after removal. This is an exercise-level ownership policy,
not sender authentication.

IFF (28) currently exchanges the Mark X/XII transponder layer-1 on/off and
operational flags. The runtime panel displays incoming status. It does not infer
allegiance, authentication, or Mode 4/5 codes, nor derive a transponder from a
radar's `iffCapable` flag. Other IFF layers/types are outside this mapping.

Collision (4) exchanges source, target, event ID, collision type, mass, ECEF
velocity and entity-relative contact coordinates. Incoming collisions are shown
in the panel and logged once per source/event within a bounded history. No
automatic damage or collision detection has been added. The runtime PDU tester
can send each of these interactions explicitly.

## Standalone Periodic Tester

Open **View > DIS PDU Tester** in the main application. This starts a separate
process with the panel's current address, port and exercise, and a different
application ID. Transmission starts only after pressing **Start periodic send**
or **Send batch**. Stop closes the tester's UDP session.

Select any of the 11 supported types and set **Batch interval** in seconds.
Each interval sends one of every selected type. All types are initially selected.
Start/Resume and Stop/Freeze affect the receiving simulation; the latter uses
Pause/Recess. Create/Remove in one batch create and then delete the target
placeholder. Deselect these types when inspecting persistent entities.

The **Entity movement** tab configures starting latitude, longitude, altitude,
heading, speed and moving/static mode. Movement is enabled initially at 250 kt,
heading 90 degrees. Positions advance on WGS84 using actual elapsed time, then
travel in consecutive Entity State PDUs with a stable identifier. The receiver
updates on receipt; interpolation between packets is not implemented, so a
shorter interval produces smaller visible steps. Intervals longer than the
receiver's remote-entity timeout can cause expiry between packets.

The tester logs transmissions and correlated acknowledgements with counters.
Its explicit Acknowledge sample is unsolicited and may be ignored by receivers;
real control and management replies are correlated normally.

Launch directly from the build directory, for example:

```sh
./qttest_dis_pdu_tester --address 239.1.2.3 --port 3000 --exercise 1 --interval 1
```

For a bounded automatic run add `--auto-start --batches 2`. The integration test
launches this executable as a child process, receives two multicast batches on
an isolated port, checks all 11 wire types and verifies changing Entity State
positions. The full application does not need to run for this automated check.
