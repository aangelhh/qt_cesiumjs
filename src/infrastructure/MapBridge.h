#pragma once

#include <QObject>
#include <QString>

class MapBridge : public QObject {
  Q_OBJECT

public:
  explicit MapBridge(QObject* parent = nullptr) : QObject(parent) {}

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

signals:
  void pickedCoordinate(double longitude, double latitude, double height);
  void mapStatus(const QString& message);
  void selectedTrack(const QString& trackName);
  
  // Bridge C++ to JS via QWebChannel
  void telemetryUpdated(const QVariantMap& track);
};
