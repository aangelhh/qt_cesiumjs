#include "application/ScenarioState.h"

#include "application/FlightDynamicsEngine.h"
#include "application/SensorEngine.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace {

QString projectRoot() {
#ifdef QTTEST_SOURCE_DIR
  return QString::fromUtf8(QTTEST_SOURCE_DIR);
#else
  return QDir::currentPath();
#endif
}

QJsonObject toJson(const SensorDefinition& sensor) {
  return {
      {QStringLiteral("id"), sensor.id},
      {QStringLiteral("name"), sensor.name},
      {QStringLiteral("sensorType"), sensor.sensorType},
      {QStringLiteral("enabled"), sensor.enabled},
      {QStringLiteral("emitting"), sensor.emitting},
      {QStringLiteral("maxRangeMeters"), sensor.maxRangeMeters},
      {QStringLiteral("minRangeMeters"), sensor.minRangeMeters},
      {QStringLiteral("azimuthCenterDegrees"), sensor.azimuthCenterDegrees},
      {QStringLiteral("azimuthWidthDegrees"), sensor.azimuthWidthDegrees},
      {QStringLiteral("elevationCenterDegrees"), sensor.elevationCenterDegrees},
      {QStringLiteral("elevationWidthDegrees"), sensor.elevationWidthDegrees},
      {QStringLiteral("updatePeriodSeconds"), sensor.updatePeriodSeconds},
      {QStringLiteral("maxTracks"), sensor.maxTracks},
      {QStringLiteral("iffCapable"), sensor.iffCapable},
      {QStringLiteral("canDetectAir"), sensor.canDetectAir},
      {QStringLiteral("canDetectGround"), sensor.canDetectGround},
      {QStringLiteral("canDetectSurface"), sensor.canDetectSurface},
      {QStringLiteral("terrainMaskingEnabled"), sensor.terrainMaskingEnabled},
      {QStringLiteral("probabilityOfDetection"), sensor.probabilityOfDetection},
  };
}

SensorDefinition sensorFromJson(const QJsonObject& object) {
  SensorDefinition sensor;
  sensor.id = object.value(QStringLiteral("id")).toString();
  sensor.name = object.value(QStringLiteral("name")).toString();
  sensor.sensorType = object.value(QStringLiteral("sensorType")).toString(QStringLiteral("radar"));
  sensor.enabled = object.value(QStringLiteral("enabled")).toBool(true);
  sensor.emitting = object.value(QStringLiteral("emitting")).toBool(true);
  sensor.maxRangeMeters = object.value(QStringLiteral("maxRangeMeters")).toDouble(250000.0);
  sensor.minRangeMeters = object.value(QStringLiteral("minRangeMeters")).toDouble(0.0);
  sensor.azimuthCenterDegrees = object.value(QStringLiteral("azimuthCenterDegrees")).toDouble(0.0);
  sensor.azimuthWidthDegrees = object.value(QStringLiteral("azimuthWidthDegrees")).toDouble(360.0);
  sensor.elevationCenterDegrees = object.value(QStringLiteral("elevationCenterDegrees")).toDouble(0.0);
  sensor.elevationWidthDegrees = object.value(QStringLiteral("elevationWidthDegrees")).toDouble(90.0);
  sensor.updatePeriodSeconds = object.value(QStringLiteral("updatePeriodSeconds")).toDouble(5.0);
  sensor.maxTracks = object.value(QStringLiteral("maxTracks")).toInt(32);
  sensor.iffCapable = object.value(QStringLiteral("iffCapable")).toBool(true);
  sensor.canDetectAir = object.value(QStringLiteral("canDetectAir")).toBool(true);
  sensor.canDetectGround = object.value(QStringLiteral("canDetectGround")).toBool(false);
  sensor.canDetectSurface = object.value(QStringLiteral("canDetectSurface")).toBool(false);
  sensor.terrainMaskingEnabled = object.value(QStringLiteral("terrainMaskingEnabled")).toBool(false);
  sensor.probabilityOfDetection = object.value(QStringLiteral("probabilityOfDetection")).toDouble(1.0);
  return sensor;
}

QJsonObject toJson(const EntityTask& task) {
  return {
      {QStringLiteral("taskType"), task.taskType},
      {QStringLiteral("enabled"), task.enabled},
      {QStringLiteral("status"), task.status},
      {QStringLiteral("targetHeadingDegrees"), task.targetHeadingDegrees},
      {QStringLiteral("targetAltitudeMeters"), task.targetAltitudeMeters},
      {QStringLiteral("targetSpeedKnots"), task.targetSpeedKnots},
      {QStringLiteral("targetLatitude"), task.targetLatitude},
      {QStringLiteral("targetLongitude"), task.targetLongitude},
      {QStringLiteral("targetEntityName"), task.targetEntityName},
      {QStringLiteral("targetWaypointName"), task.targetWaypointName},
      {QStringLiteral("targetRouteName"), task.targetRouteName},
      {QStringLiteral("targetAreaName"), task.targetAreaName},
      {QStringLiteral("targetAreaRadiusMeters"), task.targetAreaRadiusMeters},
  };
}

EntityTask taskFromJson(const QJsonObject& object) {
  EntityTask task;
  task.taskType = object.value(QStringLiteral("taskType")).toString();
  task.enabled = object.value(QStringLiteral("enabled")).toBool(false);
  task.status = object.value(QStringLiteral("status")).toString(QStringLiteral("Idle"));
  task.targetHeadingDegrees = object.value(QStringLiteral("targetHeadingDegrees")).toDouble(0.0);
  task.targetAltitudeMeters = object.value(QStringLiteral("targetAltitudeMeters")).toInt(0);
  task.targetSpeedKnots = object.value(QStringLiteral("targetSpeedKnots")).toDouble(0.0);
  task.targetLatitude = object.value(QStringLiteral("targetLatitude")).toDouble(0.0);
  task.targetLongitude = object.value(QStringLiteral("targetLongitude")).toDouble(0.0);
  task.targetEntityName = object.value(QStringLiteral("targetEntityName")).toString();
  task.targetWaypointName = object.value(QStringLiteral("targetWaypointName")).toString();
  task.targetRouteName = object.value(QStringLiteral("targetRouteName")).toString();
  task.targetAreaName = object.value(QStringLiteral("targetAreaName")).toString();
  task.targetAreaRadiusMeters = object.value(QStringLiteral("targetAreaRadiusMeters")).toDouble(0.0);
  return task;
}

QJsonObject toJson(const Waypoint& waypoint) {
  return {
      {QStringLiteral("name"), waypoint.name},
      {QStringLiteral("latitude"), waypoint.latitude},
      {QStringLiteral("longitude"), waypoint.longitude},
      {QStringLiteral("altitudeMeters"), waypoint.altitudeMeters},
  };
}

Waypoint waypointFromJson(const QJsonObject& object) {
  Waypoint waypoint;
  waypoint.name = object.value(QStringLiteral("name")).toString();
  waypoint.latitude = object.value(QStringLiteral("latitude")).toDouble(0.0);
  waypoint.longitude = object.value(QStringLiteral("longitude")).toDouble(0.0);
  waypoint.altitudeMeters = object.value(QStringLiteral("altitudeMeters")).toDouble(0.0);
  return waypoint;
}

QJsonObject toJson(const RouteGraphic& route) {
  QJsonArray points;
  for (const RoutePoint& point : route.points) {
    points.append(QJsonObject{
        {QStringLiteral("latitude"), point.latitude},
        {QStringLiteral("longitude"), point.longitude},
        {QStringLiteral("altitudeMeters"), point.altitudeMeters},
    });
  }
  return {
      {QStringLiteral("name"), route.name},
      {QStringLiteral("points"), points},
  };
}

RouteGraphic routeFromJson(const QJsonObject& object) {
  RouteGraphic route;
  route.name = object.value(QStringLiteral("name")).toString();
  const QJsonArray points = object.value(QStringLiteral("points")).toArray();
  for (const QJsonValue& value : points) {
    const QJsonObject pointObject = value.toObject();
    RoutePoint point;
    point.latitude = pointObject.value(QStringLiteral("latitude")).toDouble(0.0);
    point.longitude = pointObject.value(QStringLiteral("longitude")).toDouble(0.0);
    point.altitudeMeters = pointObject.value(QStringLiteral("altitudeMeters")).toDouble(0.0);
    route.points.push_back(point);
  }
  return route;
}

QJsonObject toJson(const AreaDefinition& area) {
  QJsonArray points;
  for (const RoutePoint& point : area.points) {
    points.append(QJsonObject{
        {QStringLiteral("latitude"), point.latitude},
        {QStringLiteral("longitude"), point.longitude},
        {QStringLiteral("altitudeMeters"), point.altitudeMeters},
    });
  }
  return {
      {QStringLiteral("id"), area.id},
      {QStringLiteral("name"), area.name},
      {QStringLiteral("areaType"), area.areaType},
      {QStringLiteral("forceIdentifier"), area.forceIdentifier},
      {QStringLiteral("centerLatitude"), area.centerLatitude},
      {QStringLiteral("centerLongitude"), area.centerLongitude},
      {QStringLiteral("centerAltitudeMeters"), area.centerAltitudeMeters},
      {QStringLiteral("radiusMeters"), area.radiusMeters},
      {QStringLiteral("semiMajorAxisMeters"), area.semiMajorAxisMeters},
      {QStringLiteral("semiMinorAxisMeters"), area.semiMinorAxisMeters},
      {QStringLiteral("rotationDegrees"), area.rotationDegrees},
      {QStringLiteral("points"), points},
      {QStringLiteral("minAltitudeMeters"), area.minAltitudeMeters},
      {QStringLiteral("maxAltitudeMeters"), area.maxAltitudeMeters},
      {QStringLiteral("notes"), area.notes},
  };
}

AreaDefinition areaFromJson(const QJsonObject& object) {
  AreaDefinition area;
  area.id = object.value(QStringLiteral("id")).toString();
  area.name = object.value(QStringLiteral("name")).toString();
  area.areaType = object.value(QStringLiteral("areaType")).toString(QStringLiteral("Circle"));
  area.forceIdentifier = object.value(QStringLiteral("forceIdentifier")).toInt(0);
  area.centerLatitude = object.value(QStringLiteral("centerLatitude")).toDouble(0.0);
  area.centerLongitude = object.value(QStringLiteral("centerLongitude")).toDouble(0.0);
  area.centerAltitudeMeters = object.value(QStringLiteral("centerAltitudeMeters")).toDouble(0.0);
  area.radiusMeters = object.value(QStringLiteral("radiusMeters")).toDouble(1000.0);
  area.semiMajorAxisMeters = object.value(QStringLiteral("semiMajorAxisMeters")).toDouble(1000.0);
  area.semiMinorAxisMeters = object.value(QStringLiteral("semiMinorAxisMeters")).toDouble(600.0);
  area.rotationDegrees = object.value(QStringLiteral("rotationDegrees")).toDouble(0.0);
  const QJsonArray points = object.value(QStringLiteral("points")).toArray();
  for (const QJsonValue& value : points) {
    const QJsonObject pointObject = value.toObject();
    RoutePoint point;
    point.latitude = pointObject.value(QStringLiteral("latitude")).toDouble(0.0);
    point.longitude = pointObject.value(QStringLiteral("longitude")).toDouble(0.0);
    point.altitudeMeters = pointObject.value(QStringLiteral("altitudeMeters")).toDouble(0.0);
    area.points.push_back(point);
  }
  area.minAltitudeMeters = object.value(QStringLiteral("minAltitudeMeters")).toDouble(0.0);
  area.maxAltitudeMeters = object.value(QStringLiteral("maxAltitudeMeters")).toDouble(0.0);
  area.notes = object.value(QStringLiteral("notes")).toString();
  return area;
}

QJsonObject toJson(const SensorContact& contact) {
  return {
      {QStringLiteral("sensorId"), contact.sensorId},
      {QStringLiteral("targetEntityName"), contact.targetEntityName},
      {QStringLiteral("rangeMeters"), contact.rangeMeters},
      {QStringLiteral("bearingDegrees"), contact.bearingDegrees},
      {QStringLiteral("lineOfSight"), contact.lineOfSight},
      {QStringLiteral("detected"), contact.detected},
  };
}

SensorContact sensorContactFromJson(const QJsonObject& object) {
  SensorContact contact;
  contact.sensorId = object.value(QStringLiteral("sensorId")).toString();
  contact.targetEntityName = object.value(QStringLiteral("targetEntityName")).toString();
  contact.rangeMeters = object.value(QStringLiteral("rangeMeters")).toDouble(0.0);
  contact.bearingDegrees = object.value(QStringLiteral("bearingDegrees")).toDouble(0.0);
  contact.lineOfSight = object.value(QStringLiteral("lineOfSight")).toBool(true);
  contact.detected = object.value(QStringLiteral("detected")).toBool(false);
  return contact;
}

QJsonObject toJson(const Entity& entity) {
  QJsonArray sensors;
  for (const SensorDefinition& sensor : entity.sensors) {
    sensors.append(toJson(sensor));
  }

  QJsonArray contacts;
  for (const SensorContact& contact : entity.sensorContacts) {
    contacts.append(toJson(contact));
  }

  return {
      {QStringLiteral("name"), entity.name},
      {QStringLiteral("type"), entity.type},
      {QStringLiteral("domain"), entity.domain},
      {QStringLiteral("category"), entity.category},
      {QStringLiteral("entityTypeCode"), entity.entityTypeCode},
      {QStringLiteral("callsign"), entity.callsign},
      {QStringLiteral("forceIdentifier"), entity.forceIdentifier},
      {QStringLiteral("latitude"), entity.latitude},
      {QStringLiteral("longitude"), entity.longitude},
      {QStringLiteral("groundHeight"), entity.groundHeight},
      {QStringLiteral("altitude"), entity.altitude},
      {QStringLiteral("modelName"), entity.modelName},
      {QStringLiteral("modelUri"), entity.modelUri},
      {QStringLiteral("entityKind"), entity.entityKind},
      {QStringLiteral("entityDomain"), entity.entityDomain},
      {QStringLiteral("entityCountry"), entity.entityCountry},
      {QStringLiteral("entityCategory"), entity.entityCategory},
      {QStringLiteral("entitySubcategory"), entity.entitySubcategory},
      {QStringLiteral("entitySpecific"), entity.entitySpecific},
      {QStringLiteral("entityExtra"), entity.entityExtra},
      {QStringLiteral("headingDegrees"), entity.headingDegrees},
      {QStringLiteral("flightDynamicsEnabled"), entity.flightDynamicsEnabled},
      {QStringLiteral("flightDynamicsMode"), entity.flightDynamicsMode},
      {QStringLiteral("jsbsimAircraftModel"), entity.jsbsimAircraftModel},
      {QStringLiteral("speedKnots"), entity.speedKnots},
      {QStringLiteral("verticalSpeedMetersPerSecond"), entity.verticalSpeedMetersPerSecond},
      {QStringLiteral("currentTask"), toJson(entity.currentTask)},
      {QStringLiteral("sensors"), sensors},
      {QStringLiteral("sensorContacts"), contacts},
  };
}

Entity entityFromJson(const QJsonObject& object) {
  Entity entity;
  entity.name = object.value(QStringLiteral("name")).toString();
  entity.type = object.value(QStringLiteral("type")).toString(QStringLiteral("Entity"));
  entity.domain = object.value(QStringLiteral("domain")).toString(QStringLiteral("Air"));
  entity.category = object.value(QStringLiteral("category")).toString(QStringLiteral("Fighter"));
  entity.entityTypeCode = object.value(QStringLiteral("entityTypeCode")).toString();
  entity.callsign = object.value(QStringLiteral("callsign")).toString();
  entity.forceIdentifier = object.value(QStringLiteral("forceIdentifier")).toInt(1);
  entity.latitude = object.value(QStringLiteral("latitude")).toDouble(0.0);
  entity.longitude = object.value(QStringLiteral("longitude")).toDouble(0.0);
  entity.groundHeight = object.value(QStringLiteral("groundHeight")).toDouble(0.0);
  entity.altitude = object.value(QStringLiteral("altitude")).toInt(0);
  entity.modelName = object.value(QStringLiteral("modelName")).toString();
  entity.modelUri = object.value(QStringLiteral("modelUri")).toString();
  entity.entityKind = object.value(QStringLiteral("entityKind")).toInt(0);
  entity.entityDomain = object.value(QStringLiteral("entityDomain")).toInt(0);
  entity.entityCountry = object.value(QStringLiteral("entityCountry")).toInt(0);
  entity.entityCategory = object.value(QStringLiteral("entityCategory")).toInt(0);
  entity.entitySubcategory = object.value(QStringLiteral("entitySubcategory")).toInt(0);
  entity.entitySpecific = object.value(QStringLiteral("entitySpecific")).toInt(0);
  entity.entityExtra = object.value(QStringLiteral("entityExtra")).toInt(0);
  entity.headingDegrees = object.value(QStringLiteral("headingDegrees")).toDouble(0.0);
  entity.flightDynamicsEnabled = object.value(QStringLiteral("flightDynamicsEnabled")).toBool(false);
  entity.flightDynamicsMode = object.value(QStringLiteral("flightDynamicsMode")).toString(QStringLiteral("kinematic"));
  entity.jsbsimAircraftModel = object.value(QStringLiteral("jsbsimAircraftModel")).toString();
  entity.speedKnots = object.value(QStringLiteral("speedKnots")).toDouble(0.0);
  entity.verticalSpeedMetersPerSecond = object.value(QStringLiteral("verticalSpeedMetersPerSecond")).toDouble(0.0);
  entity.currentTask = taskFromJson(object.value(QStringLiteral("currentTask")).toObject());

  const QJsonArray sensors = object.value(QStringLiteral("sensors")).toArray();
  for (const QJsonValue& value : sensors) {
    entity.sensors.push_back(sensorFromJson(value.toObject()));
  }

  const QJsonArray contacts = object.value(QStringLiteral("sensorContacts")).toArray();
  for (const QJsonValue& value : contacts) {
    entity.sensorContacts.push_back(sensorContactFromJson(value.toObject()));
  }

  if (entity.entityTypeCode.isEmpty()) {
    entity.refreshEntityTypeCode();
  }
  return entity;
}

} // namespace

ScenarioState::ScenarioState() {
  this->reset();
}

void ScenarioState::addEntity(const Entity& entity) {
  _entities.push_back(entity);
  this->refreshSensors();
  this->save();
}

const QVector<Entity>& ScenarioState::entities() const {
  return _entities;
}

bool ScenarioState::removeEntity(const QString& entityName) {
  for (qsizetype index = 0; index < _entities.size(); ++index) {
    if (_entities.at(index).name == entityName) {
      _entities.removeAt(index);
      this->refreshSensors();
      this->save();
      return true;
    }
  }
  return false;
}

void ScenarioState::addWaypoint(const Waypoint& waypoint) {
  for (Waypoint& existing : _waypoints) {
    if (existing.name == waypoint.name) {
      existing = waypoint;
      this->save();
      return;
    }
  }
  _waypoints.push_back(waypoint);
  this->save();
}

const QVector<Waypoint>& ScenarioState::waypoints() const {
  return _waypoints;
}

bool ScenarioState::removeWaypoint(const QString& waypointName) {
  for (qsizetype index = 0; index < _waypoints.size(); ++index) {
    if (_waypoints.at(index).name == waypointName) {
      _waypoints.removeAt(index);
      this->save();
      return true;
    }
  }
  return false;
}

void ScenarioState::addRoute(const RouteGraphic& route) {
  for (RouteGraphic& existing : _routes) {
    if (existing.name == route.name) {
      existing = route;
      this->save();
      return;
    }
  }
  _routes.push_back(route);
  this->save();
}

const QVector<RouteGraphic>& ScenarioState::routes() const {
  return _routes;
}

bool ScenarioState::removeRoute(const QString& routeName) {
  for (qsizetype index = 0; index < _routes.size(); ++index) {
    if (_routes.at(index).name == routeName) {
      _routes.removeAt(index);
      this->save();
      return true;
    }
  }
  return false;
}

void ScenarioState::addArea(const AreaDefinition& area) {
  for (AreaDefinition& existing : _areas) {
    if (existing.id == area.id || existing.name == area.name) {
      existing = area;
      this->save();
      return;
    }
  }
  _areas.push_back(area);
  this->save();
}

const QVector<AreaDefinition>& ScenarioState::areas() const {
  return _areas;
}

bool ScenarioState::removeArea(const QString& areaName) {
  for (qsizetype index = 0; index < _areas.size(); ++index) {
    if (_areas.at(index).name == areaName || _areas.at(index).id == areaName) {
      _areas.removeAt(index);
      for (Entity& entity : _entities) {
        if ((entity.currentTask.taskType == QStringLiteral("PatrolArea") ||
             entity.currentTask.taskType == QStringLiteral("OrbitArea")) &&
            entity.currentTask.targetAreaName == areaName) {
          entity.currentTask = EntityTask{};
          entity.currentTask.status = QStringLiteral("Idle");
        }
      }
      this->save();
      return true;
    }
  }
  return false;
}

bool ScenarioState::assignTask(const QString& entityName, const EntityTask& task) {
  for (Entity& entity : _entities) {
    if (entity.name == entityName) {
      entity.currentTask = task;
      if (entity.currentTask.taskType == QStringLiteral("MoveToWaypoint") &&
          !entity.currentTask.targetWaypointName.trimmed().isEmpty()) {
        for (const Waypoint& waypoint : _waypoints) {
          if (waypoint.name == entity.currentTask.targetWaypointName) {
            entity.currentTask.targetLatitude = waypoint.latitude;
            entity.currentTask.targetLongitude = waypoint.longitude;
            entity.currentTask.targetAltitudeMeters =
                static_cast<int>(qRound(waypoint.altitudeMeters));
            break;
          }
        }
      }
      if (entity.currentTask.taskType == QStringLiteral("MoveAlongRoute") &&
          !entity.currentTask.targetRouteName.trimmed().isEmpty()) {
        for (const RouteGraphic& route : _routes) {
          if (route.name != entity.currentTask.targetRouteName || route.points.isEmpty()) {
            continue;
          }
          const RoutePoint& point = route.points.last();
          entity.currentTask.targetLatitude = point.latitude;
          entity.currentTask.targetLongitude = point.longitude;
          entity.currentTask.targetAltitudeMeters =
              static_cast<int>(qRound(point.altitudeMeters));
          break;
        }
      }
      if ((entity.currentTask.taskType == QStringLiteral("PatrolArea") ||
           entity.currentTask.taskType == QStringLiteral("OrbitArea")) &&
          !entity.currentTask.targetAreaName.trimmed().isEmpty()) {
        for (const AreaDefinition& area : _areas) {
          if (area.name != entity.currentTask.targetAreaName &&
              area.id != entity.currentTask.targetAreaName) {
            continue;
          }
          entity.currentTask.targetLatitude = area.centerLatitude;
          entity.currentTask.targetLongitude = area.centerLongitude;
          entity.currentTask.targetAltitudeMeters =
              static_cast<int>(qRound(area.centerAltitudeMeters));
          double radiusMeters = area.radiusMeters;
          if (radiusMeters <= 0.0) {
            if (area.areaType == QStringLiteral("Ellipse")) {
              radiusMeters = qMax(area.semiMinorAxisMeters, 100.0);
            } else if (!area.points.isEmpty()) {
              radiusMeters = 250.0;
            } else {
              radiusMeters = 500.0;
            }
          }
          entity.currentTask.targetAreaRadiusMeters = radiusMeters;
          break;
        }
      }
      if (task.enabled) {
        entity.flightDynamicsEnabled = true;
        if (entity.flightDynamicsMode.trimmed().isEmpty()) {
          entity.flightDynamicsMode = QStringLiteral("kinematic");
        }
        if (((entity.currentTask.taskType == QStringLiteral("MoveToLocation")) ||
             (entity.currentTask.taskType == QStringLiteral("MoveToWaypoint")) ||
             (entity.currentTask.taskType == QStringLiteral("MoveAlongRoute")) ||
             (entity.currentTask.taskType == QStringLiteral("PatrolArea")) ||
             (entity.currentTask.taskType == QStringLiteral("OrbitArea"))) &&
            entity.currentTask.targetSpeedKnots <= 0.0) {
          entity.currentTask.targetSpeedKnots =
              entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0
              ? 220.0
              : 12.0;
        }
        if (entity.currentTask.taskType == QStringLiteral("FollowEntity") &&
            entity.currentTask.targetSpeedKnots <= 0.0) {
          entity.currentTask.targetSpeedKnots =
              entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0
              ? 220.0
              : 12.0;
        }
      }
      this->save();
      return true;
    }
  }
  return false;
}

bool ScenarioState::clearTask(const QString& entityName) {
  EntityTask clearedTask;
  clearedTask.status = QStringLiteral("Idle");
  return this->assignTask(entityName, clearedTask);
}

domain::TaskStack* ScenarioState::getTaskStack(const QString& entityName) {
  return &_taskStacks[entityName];
}

void ScenarioState::refreshSensors() {
  SensorEngine::updateEntityContacts(_entities);
}

void ScenarioState::advanceSimulation(double deltaSeconds) {
  FlightDynamicsEngine::advanceEntities(_entities, _taskStacks, deltaSeconds);
  this->refreshSensors();
}

void ScenarioState::stopMission() {
  for (Entity& entity : _entities) {
    entity.currentTask = EntityTask{};
    entity.currentTask.status = QStringLiteral("Stopped");
    entity.speedKnots = 0.0;
    entity.verticalSpeedMetersPerSecond = 0.0;
    entity.sensorContacts.clear();
  }
  _taskStacks.clear();
  this->refreshSensors();
  this->save();
}

bool ScenarioState::save() const {
  QJsonArray entities;
  for (const Entity& entity : _entities) {
    entities.append(toJson(entity));
  }

  QJsonArray waypoints;
  for (const Waypoint& waypoint : _waypoints) {
    waypoints.append(toJson(waypoint));
  }

  QJsonArray routes;
  for (const RouteGraphic& route : _routes) {
    routes.append(toJson(route));
  }

  QJsonArray areas;
  for (const AreaDefinition& area : _areas) {
    areas.append(toJson(area));
  }

  QFile file(this->storagePath());
  const QFileInfo info(file);
  QDir().mkpath(info.absolutePath());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    return false;
  }

  const QJsonDocument document(QJsonObject{
      {QStringLiteral("entities"), entities},
      {QStringLiteral("waypoints"), waypoints},
      {QStringLiteral("routes"), routes},
      {QStringLiteral("areas"), areas},
  });
  file.write(document.toJson(QJsonDocument::Indented));
  return true;
}

bool ScenarioState::load() {
  _entities.clear();
  _waypoints.clear();
  _routes.clear();
  _areas.clear();

  QFile file(this->storagePath());
  if (!file.exists()) {
    return true;
  }
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
  if (!document.isObject()) {
    return false;
  }

  const QJsonArray entities = document.object().value(QStringLiteral("entities")).toArray();
  for (const QJsonValue& value : entities) {
    _entities.push_back(entityFromJson(value.toObject()));
  }

  const QJsonArray waypoints = document.object().value(QStringLiteral("waypoints")).toArray();
  for (const QJsonValue& value : waypoints) {
    _waypoints.push_back(waypointFromJson(value.toObject()));
  }

  const QJsonArray routes = document.object().value(QStringLiteral("routes")).toArray();
  for (const QJsonValue& value : routes) {
    _routes.push_back(routeFromJson(value.toObject()));
  }

  const QJsonArray areas = document.object().value(QStringLiteral("areas")).toArray();
  for (const QJsonValue& value : areas) {
    _areas.push_back(areaFromJson(value.toObject()));
  }

  this->refreshSensors();
  return true;
}

void ScenarioState::reset() {
  _entities.clear();
  _waypoints.clear();
  _routes.clear();
  _areas.clear();
  this->save();
}

QString ScenarioState::storagePath() const {
  return QDir(projectRoot()).absoluteFilePath(QStringLiteral("Data/scenario_state.json"));
}
