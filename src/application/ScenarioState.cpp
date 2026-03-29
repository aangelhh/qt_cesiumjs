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
  return task;
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
  this->load();
}

void ScenarioState::addEntity(const Entity& entity) {
  _entities.push_back(entity);
  this->refreshSensors();
  this->save();
}

const QVector<Entity>& ScenarioState::entities() const {
  return _entities;
}

bool ScenarioState::assignTask(const QString& entityName, const EntityTask& task) {
  for (Entity& entity : _entities) {
    if (entity.name == entityName) {
      entity.currentTask = task;
      if (task.enabled) {
        entity.flightDynamicsEnabled = true;
        if (entity.flightDynamicsMode.trimmed().isEmpty()) {
          entity.flightDynamicsMode = QStringLiteral("kinematic");
        }
        if (entity.currentTask.taskType == QStringLiteral("MoveToLocation") &&
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

void ScenarioState::refreshSensors() {
  SensorEngine::updateEntityContacts(_entities);
}

void ScenarioState::advanceSimulation(double deltaSeconds) {
  FlightDynamicsEngine::advanceEntities(_entities, deltaSeconds);
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
  this->refreshSensors();
  this->save();
}

bool ScenarioState::save() const {
  QJsonArray entities;
  for (const Entity& entity : _entities) {
    entities.append(toJson(entity));
  }

  QFile file(this->storagePath());
  const QFileInfo info(file);
  QDir().mkpath(info.absolutePath());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    return false;
  }

  const QJsonDocument document(QJsonObject{
      {QStringLiteral("entities"), entities},
  });
  file.write(document.toJson(QJsonDocument::Indented));
  return true;
}

bool ScenarioState::load() {
  _entities.clear();

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

  this->refreshSensors();
  return true;
}

QString ScenarioState::storagePath() const {
  return QDir(projectRoot()).absoluteFilePath(QStringLiteral("Data/scenario_state.json"));
}
