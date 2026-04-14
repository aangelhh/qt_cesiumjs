#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include "application/EventBus.h"

class MapBridge : public QObject {
  Q_OBJECT

public:
  explicit MapBridge(QObject* parent = nullptr) : QObject(parent) {
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

public slots:
  void reportPickedCoordinate(double longitude, double latitude, double height) {
    emit pickedCoordinate(longitude, latitude, height);
  }

  void reportMapStatus(const QString& message) {
    emit mapStatus(message);
  }

  void reportSelectedTrack(const QString& trackName) {
    emit selectedTrack(trackName);
  }

  void requestEntityContextMenu(const QString& trackName, int viewX, int viewY) {
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
