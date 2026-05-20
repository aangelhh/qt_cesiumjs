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
  sensor.sensorType = QStringLiteral("Radar");
  sensor.maxRangeMeters = 100000.0;
  e.sensors.push_back(sensor);
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_EQ(summary.value(QStringLiteral("sensorCount")).toInt(), 1);
  const QVariantList sensors = summary.value(QStringLiteral("sensors")).toList();
  ASSERT_EQ(sensors.size(), 1);
  EXPECT_EQ(sensors.first().toMap().value(QStringLiteral("name")).toString(), QStringLiteral("Radar1"));
}

TEST(EntityTrackSummaryTest, ContactCountMatchesContactsSize) {
  Entity e = makeBasicEntity();
  SensorContact contact;
  contact.targetEntityName = QStringLiteral("Enemy1");
  contact.detected = true;
  contact.rangeMeters = 25000.0;
  e.sensorContacts.push_back(contact);
  const QVariantMap summary = makeEntityTrackSummary(e, defaultVisualState());
  EXPECT_EQ(summary.value(QStringLiteral("contactCount")).toInt(), 1);
  const QVariantList contacts = summary.value(QStringLiteral("sensorContacts")).toList();
  ASSERT_EQ(contacts.size(), 1);
  EXPECT_EQ(contacts.first().toMap().value(QStringLiteral("targetEntityName")).toString(), QStringLiteral("Enemy1"));
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
