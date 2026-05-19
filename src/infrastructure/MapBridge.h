#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QtGlobal>
#include <cmath>
#include "application/EventBus.h"

class MapBridge : public QObject {
  Q_OBJECT

public:
  explicit MapBridge(QObject* parent = nullptr) : QObject(parent) {
      initializeEventSubscriptions();
  }

private:
  void initializeEventSubscriptions() {
      // Subscribe to domain events and translate them into Qt UI signals
      application::EventBus::instance().subscribe<application::EventKinematicsUpdated>(
          [this](const application::EventKinematicsUpdated& ev) {
              QVariantMap trackData;
              trackData["id"] = ev.entityId;
              trackData["name"] = ev.entityName;
              trackData["latitude"] = ev.latitude;
              trackData["longitude"] = ev.longitude;
              trackData["altitude"] = ev.altitudeMeters;
              trackData["headingDegrees"] = ev.headingDegrees;
              trackData["speedKnots"] = ev.speedKnots;
              trackData["team"] = ev.team;
              
              // We must use QMetaObject::invokeMethod to ensure the signal is emitted 
              // safely on the thread that owns this MapBridge (the main UI thread), 
              // since the EventBus callback executes on the SimulationEngine thread.
              QMetaObject::invokeMethod(this, "telemetryUpdated", Qt::QueuedConnection,
                                        Q_ARG(QVariantMap, trackData));
          }
      );
  }

public:
public slots:
  void reportPickedCoordinate(double longitude, double latitude, double height) {
    if (!std::isfinite(longitude) || !std::isfinite(latitude) || !std::isfinite(height)) {
      return;
    }
    if (latitude < -90.0 || latitude > 90.0) {
      return;
    }
    if (longitude < -180.0 || longitude > 180.0) {
      return;
    }
    emit pickedCoordinate(longitude, latitude, height);
  }

  void reportMapStatus(const QString& message) {
    emit mapStatus(message);
  }

  void reportSelectedTrack(const QString& trackName) {
    if (trackName.size() > 256) {
      return;
    }
    emit selectedTrack(trackName);
  }

  void requestEntityContextMenu(const QString& trackName, int viewX, int viewY) {
    if (trackName.isEmpty() || trackName.size() > 256) {
      return;
    }
    if (viewX < 0 || viewY < 0) {
      return;
    }
    emit entityContextMenuRequested(trackName, viewX, viewY);
  }

signals:
  void pickedCoordinate(double longitude, double latitude, double height);
  void mapStatus(const QString& message);
  void selectedTrack(const QString& trackName);
  void entityContextMenuRequested(const QString& trackName, int viewX, int viewY);
  
  // Bridge C++ to JS via QWebChannel
  void telemetryUpdated(const QVariantMap& track);
};
