#include <gtest/gtest.h>
#include "presentation/TrackSummaryBuilder.h"
#include "presentation/EntityVisualStateManager.h"
#include "domain/Entity.h"
#include "domain/TacticalGraphic.h"

using namespace presentation;

// ── Helpers ───────────────────────────────────────────────────────────────────

static Entity makeBasicEntity(const QString& name = QStringLiteral("Alpha")) {
  Entity e;
  e.name = name;
  e.type = QStringLiteral("F-16");
  e.category = QStringLiteral("Fighter");
  e.callsign = QStringLiteral("Viper 1");
  e.forceIdentifier = 1;
  e.domain = QStringLiteral("Air");
  e.latitude = 40.0;
  e.longitude = -3.0;
  e.altitude = 5000;
  e.headingDegrees = 90.0;
  e.speedKnots = 450.0;
  e.destroyed = false;
  e.damagePercent = 0.0;
  e.currentTask.enabled = false;
  e.currentTask.taskType = QStringLiteral("");
  return e;
}

static EntityVisualState defaultVisualState() {
  EntityVisualState vs;
  vs.hidden = false;
  vs.radarCoverageVisible = false;
  vs.trackHistoryVisible = false;
  return vs;
}

// ── Tests ─────────────────────────────────────────────────────────────────────

TEST(EntityTrackSummaryTest, ContainsEntityName) {
  const Entity e = makeBasicEntity(QStringLiteral("Bravo"));
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_EQ(summary.value(QStringLiteral("name")).toString(), QStringLiteral("Bravo"));
}

TEST(EntityTrackSummaryTest, ContainsStableEntityIdSeparateFromDisplayName) {
  const Entity e = makeBasicEntity(QStringLiteral("mirage2000"));
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());

  EXPECT_EQ(summary.value(QStringLiteral("entityId")).toString(), e.entityId);
  EXPECT_EQ(summary.value(QStringLiteral("name")).toString(), e.name);
  EXPECT_NE(
      summary.value(QStringLiteral("entityId")).toString(),
      summary.value(QStringLiteral("name")).toString());
}

TEST(EntityTrackSummaryTest, ContainsDomain) {
  const Entity e = makeBasicEntity();
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_EQ(summary.value(QStringLiteral("domain")).toString(), QStringLiteral("Air"));
}

TEST(EntityTrackSummaryTest, ContainsForceIdentifier) {
  Entity e = makeBasicEntity();
  e.forceIdentifier = 2;
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_EQ(summary.value(QStringLiteral("forceIdentifier")).toInt(), 2);
}

TEST(EntityTrackSummaryTest, ContainsPosition) {
  Entity e = makeBasicEntity();
  e.latitude = 41.5;
  e.longitude = 2.1;
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_DOUBLE_EQ(summary.value(QStringLiteral("latitude")).toDouble(), 41.5);
  EXPECT_DOUBLE_EQ(summary.value(QStringLiteral("longitude")).toDouble(), 2.1);
}

TEST(EntityTrackSummaryTest, ContainsCesiumModelAxes) {
  Entity e = makeBasicEntity();
  e.cesiumModelAxes = QStringLiteral("x-forward-y-up");

  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());

  EXPECT_EQ(
      summary.value(QStringLiteral("cesiumModelAxes")).toString(),
      QStringLiteral("x-forward-y-up"));
}

TEST(EntityTrackSummaryTest, ContainsFuelState) {
  Entity e = makeBasicEntity();
  e.fuelCapacityKilograms = 5875.0;
  e.fuelRemainingKilograms = 2500.0;

  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());

  EXPECT_DOUBLE_EQ(
      summary.value(QStringLiteral("fuelCapacityKilograms")).toDouble(),
      5875.0);
  EXPECT_DOUBLE_EQ(
      summary.value(QStringLiteral("fuelRemainingKilograms")).toDouble(),
      2500.0);
}

TEST(EntityTrackSummaryTest, ContainsRuntimeSubsystemAndResourceState) {
  Entity e = makeBasicEntity();
  e.activeDynamicsBackend = QStringLiteral("jsbsim");
  e.dynamicsFallbackReason = QStringLiteral("none");
  e.dynamicsStepDurationMilliseconds = 0.42;
  e.systemsDisplayProfileId = QStringLiteral("air-turbine-1-engine");
  e.engineCount = 1;
  e.weapons.push_back({QStringLiteral("Missile"), 2});
  e.weapons.push_back({QStringLiteral("Bomb"), 4});

  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());

  EXPECT_EQ(
      summary.value(QStringLiteral("activeDynamicsBackend")).toString(),
      QStringLiteral("jsbsim"));
  EXPECT_DOUBLE_EQ(
      summary.value(QStringLiteral("dynamicsStepDurationMilliseconds")).toDouble(),
      0.42);
  EXPECT_EQ(summary.value(QStringLiteral("engineCount")).toInt(), 1);
  const QVariantList weapons = summary.value(QStringLiteral("weapons")).toList();
  ASSERT_EQ(weapons.size(), 2);
  EXPECT_EQ(
      weapons.at(1).toMap().value(QStringLiteral("weaponType")).toString(),
      QStringLiteral("Bomb"));
  EXPECT_EQ(weapons.at(1).toMap().value(QStringLiteral("quantity")).toInt(), 4);
}

TEST(EntityTrackSummaryTest, BehaviorModeDefaultsToManualWhenEmpty) {
  Entity e = makeBasicEntity();
  e.behaviorMode = QStringLiteral("   ");
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_EQ(summary.value(QStringLiteral("behaviorMode")).toString(), QStringLiteral("Manual"));
}

TEST(EntityTrackSummaryTest, BehaviorModePreservesNonEmptyValue) {
  Entity e = makeBasicEntity();
  e.behaviorMode = QStringLiteral("Aggressive");
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_EQ(summary.value(QStringLiteral("behaviorMode")).toString(), QStringLiteral("Aggressive"));
}

TEST(EntityTrackSummaryTest, VisualStateHiddenReflected) {
  EntityVisualState vs = defaultVisualState();
  vs.hidden = true;
  const QVariantMap summary = makeEntityTrackSummary(makeBasicEntity(), vs);
  EXPECT_TRUE(summary.value(QStringLiteral("hidden")).toBool());
}

TEST(EntityTrackSummaryTest, VisualStateRadarVisible) {
  EntityVisualState vs = defaultVisualState();
  vs.radarCoverageVisible = true;
  const QVariantMap summary = makeEntityTrackSummary(makeBasicEntity(), vs);
  EXPECT_TRUE(summary.value(QStringLiteral("radarCoverageVisible")).toBool());
}

TEST(EntityTrackSummaryTest, TaskFieldsPopulated) {
  Entity e = makeBasicEntity();
  e.currentTask.enabled = true;
  e.currentTask.taskType = QStringLiteral("MoveToLocation");
  e.currentTask.status = QStringLiteral("Running");
  e.currentTask.targetLatitude = 39.0;
  e.currentTask.targetLongitude = -4.0;
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_TRUE(summary.value(QStringLiteral("taskEnabled")).toBool());
  EXPECT_EQ(summary.value(QStringLiteral("taskType")).toString(), QStringLiteral("MoveToLocation"));
  EXPECT_EQ(summary.value(QStringLiteral("taskStatus")).toString(), QStringLiteral("Running"));
  EXPECT_DOUBLE_EQ(summary.value(QStringLiteral("taskTargetLatitude")).toDouble(), 39.0);
}

TEST(EntityTrackSummaryTest, SensorCountMatchesSensorsSize) {
  Entity e = makeBasicEntity();
  SensorDefinition sensor;
  sensor.id = QStringLiteral("radar-1");
  sensor.name = QStringLiteral("Radar1");
  sensor.modelProviderId = QStringLiteral("stone-soup");
  sensor.sensorType = QStringLiteral("Radar");
  sensor.sensorSubType = QStringLiteral("AirborneRadar");
  sensor.maxRangeMeters = 100000.0;
  sensor.probabilityOfDetection = 0.75;
  sensor.trackHoldSeconds = 6.0;
  sensor.radarProfile.profileId = QStringLiteral("fighter-aesa");
  sensor.radarProfile.peakPowerWatts = 55000.0;
  e.sensors.push_back(sensor);
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_EQ(summary.value(QStringLiteral("sensorCount")).toInt(), 1);
  const QVariantList sensors = summary.value(QStringLiteral("sensors")).toList();
  ASSERT_EQ(sensors.size(), 1);
  EXPECT_EQ(sensors.first().toMap().value(QStringLiteral("name")).toString(), QStringLiteral("Radar1"));
  EXPECT_EQ(
      sensors.first().toMap().value(QStringLiteral("modelProviderId")).toString(),
      QStringLiteral("stone-soup"));
  EXPECT_EQ(
      sensors.first().toMap().value(QStringLiteral("sensorSubType")).toString(),
      QStringLiteral("AirborneRadar"));
  EXPECT_DOUBLE_EQ(
      sensors.first().toMap().value(QStringLiteral("probabilityOfDetection")).toDouble(),
      0.75);
  EXPECT_DOUBLE_EQ(
      sensors.first().toMap().value(QStringLiteral("trackHoldSeconds")).toDouble(),
      6.0);
  const QVariantMap profile = sensors.first().toMap()
      .value(QStringLiteral("radarProfile")).toMap();
  EXPECT_EQ(
      profile.value(QStringLiteral("profileId")).toString(),
      QStringLiteral("fighter-aesa"));
  EXPECT_DOUBLE_EQ(
      profile.value(QStringLiteral("peakPowerWatts")).toDouble(),
      55000.0);
}

TEST(EntityTrackSummaryTest, ContactCountMatchesContactsSize) {
  Entity e = makeBasicEntity();
  SensorContact contact;
  contact.sensorModelProviderId = QStringLiteral("stone-soup");
  contact.sensorType = QStringLiteral("radar");
  contact.sensorSubType = QStringLiteral("airborneRadar");
  contact.targetEntityName = QStringLiteral("Enemy1");
  contact.detected = true;
  contact.confidence = 0.67;
  contact.trackState = QStringLiteral("Coasting");
  contact.rangeMeters = 25000.0;
  contact.evaluation.requestedModelProviderId = QStringLiteral("stone-soup");
  contact.evaluation.effectiveModelProviderId = QStringLiteral("stone-soup");
  contact.evaluation.providerVersion = QStringLiteral("1.9");
  contact.evaluation.detectionProbability = 0.67;
  contact.evaluation.deterministicSample = 0.25;
  contact.evaluation.signalToNoiseRatioDecibels = 12.5;
  e.sensorContacts.push_back(contact);
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_EQ(summary.value(QStringLiteral("contactCount")).toInt(), 1);
  const QVariantList contacts = summary.value(QStringLiteral("sensorContacts")).toList();
  ASSERT_EQ(contacts.size(), 1);
  EXPECT_EQ(contacts.first().toMap().value(QStringLiteral("targetEntityName")).toString(), QStringLiteral("Enemy1"));
  EXPECT_EQ(
      contacts.first().toMap().value(QStringLiteral("sensorSubType")).toString(),
      QStringLiteral("airborneRadar"));
  EXPECT_DOUBLE_EQ(
      contacts.first().toMap().value(QStringLiteral("confidence")).toDouble(),
      0.67);
  EXPECT_EQ(
      contacts.first().toMap().value(QStringLiteral("sensorModelProviderId")).toString(),
      QStringLiteral("stone-soup"));
  EXPECT_EQ(
      contacts.first().toMap().value(QStringLiteral("trackState")).toString(),
      QStringLiteral("Coasting"));
  const QVariantMap evaluation = contacts.first().toMap()
      .value(QStringLiteral("evaluation")).toMap();
  EXPECT_EQ(
      evaluation.value(QStringLiteral("providerVersion")).toString(),
      QStringLiteral("1.9"));
  EXPECT_DOUBLE_EQ(
      evaluation.value(QStringLiteral("signalToNoiseRatioDecibels")).toDouble(),
      12.5);
}

TEST(EntityTrackSummaryTest, ContainsSensorRuntimeDiagnosticsWithoutContact) {
  Entity e = makeBasicEntity();
  SensorRuntimeStatus runtime;
  runtime.sensorId = QStringLiteral("radar-1");
  runtime.lastTargetEntityName = QStringLiteral("Bandit");
  runtime.evaluationCount = 8;
  runtime.detectionCount = 3;
  runtime.lastEvaluationIndex = 17;
  runtime.evaluation.requestedModelProviderId = QStringLiteral("mixr");
  runtime.evaluation.effectiveModelProviderId = QStringLiteral("native");
  runtime.evaluation.fallbackUsed = true;
  runtime.evaluation.fallbackReason = QStringLiteral("plugin unavailable");
  runtime.evaluation.rangeLossDecibels = 42.0;
  e.sensorRuntimeStatuses.push_back(runtime);

  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  const QVariantList statuses =
      summary.value(QStringLiteral("sensorRuntimeStatuses")).toList();

  ASSERT_EQ(statuses.size(), 1);
  const QVariantMap status = statuses.front().toMap();
  EXPECT_EQ(status.value(QStringLiteral("evaluationCount")).toULongLong(), 8U);
  EXPECT_EQ(status.value(QStringLiteral("detectionCount")).toULongLong(), 3U);
  const QVariantMap evaluation =
      status.value(QStringLiteral("evaluation")).toMap();
  EXPECT_TRUE(evaluation.value(QStringLiteral("fallbackUsed")).toBool());
  EXPECT_DOUBLE_EQ(
      evaluation.value(QStringLiteral("rangeLossDecibels")).toDouble(),
      42.0);
}

TEST(EntityTrackSummaryTest, DestroyedFlagReflected) {
  Entity e = makeBasicEntity();
  e.destroyed = true;
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_TRUE(summary.value(QStringLiteral("destroyed")).toBool());
}

// ── normalizeEntityCategory ───────────────────────────────────────────────────

TEST(TrackSummaryBuilderTest, NormalizeKnownCategory) {
  EXPECT_EQ(normalizeEntityCategory(QStringLiteral("Fighter")),   QStringLiteral("Fighter"));
  EXPECT_EQ(normalizeEntityCategory(QStringLiteral("Bomber")),    QStringLiteral("Bomber"));
  EXPECT_EQ(normalizeEntityCategory(QStringLiteral("Tank")),      QStringLiteral("Tank"));
  EXPECT_EQ(normalizeEntityCategory(QStringLiteral("SAMLauncher")), QStringLiteral("SAMLauncher"));
}

TEST(TrackSummaryBuilderTest, NormalizeUnknownCategoryReturnsOther) {
  EXPECT_EQ(normalizeEntityCategory(QStringLiteral("Submarine")), QStringLiteral("Other"));
  EXPECT_EQ(normalizeEntityCategory(QStringLiteral("")),          QStringLiteral("Other"));
  EXPECT_EQ(normalizeEntityCategory(QStringLiteral("   ")),       QStringLiteral("Other"));
}

// ── makeWaypointTrackSummary ──────────────────────────────────────────────────

TEST(TrackSummaryBuilderTest, WaypointSummaryHasCorrectFields) {
  Waypoint wp;
  wp.name = QStringLiteral("Alpha");
  wp.latitude = 40.0;
  wp.longitude = -3.0;
  wp.altitudeMeters = 500.0;
  const QVariantMap summary = makeWaypointTrackSummary(wp);
  EXPECT_EQ(summary.value(QStringLiteral("name")).toString(), QStringLiteral("Alpha"));
  EXPECT_EQ(summary.value(QStringLiteral("type")).toString(), QStringLiteral("Waypoint"));
  EXPECT_DOUBLE_EQ(summary.value(QStringLiteral("latitude")).toDouble(), 40.0);
}

// ── makeRouteTrackSummary ─────────────────────────────────────────────────────

TEST(TrackSummaryBuilderTest, RouteSummaryHasRoutePoints) {
  RouteGraphic route;
  route.name = QStringLiteral("Route1");
  RoutePoint pt; pt.latitude = 41.0; pt.longitude = -4.0;
  route.points.append(pt);
  const QVariantMap summary = makeRouteTrackSummary(route);
  EXPECT_EQ(summary.value(QStringLiteral("type")).toString(), QStringLiteral("Route"));
  EXPECT_EQ(summary.value(QStringLiteral("routePoints")).toList().size(), 1);
}

TEST(TrackSummaryBuilderTest, RouteSummaryEmptyRouteUsesDefaults) {
  RouteGraphic route;
  route.name = QStringLiteral("Empty");
  const QVariantMap summary = makeRouteTrackSummary(route);
  EXPECT_EQ(summary.value(QStringLiteral("routePoints")).toList().size(), 0);
}

// ── makeAreaTrackSummary ──────────────────────────────────────────────────────

TEST(TrackSummaryBuilderTest, AreaSummaryHasAreaFields) {
  AreaDefinition area;
  area.name = QStringLiteral("Zone1");
  area.areaType = QStringLiteral("Circle");
  area.centerLatitude = 42.0;
  area.centerLongitude = -5.0;
  area.radiusMeters = 5000.0;
  const QVariantMap summary = makeAreaTrackSummary(area);
  EXPECT_EQ(summary.value(QStringLiteral("type")).toString(), QStringLiteral("Area"));
  EXPECT_EQ(summary.value(QStringLiteral("areaType")).toString(), QStringLiteral("Circle"));
  EXPECT_DOUBLE_EQ(summary.value(QStringLiteral("radiusMeters")).toDouble(), 5000.0);
}
