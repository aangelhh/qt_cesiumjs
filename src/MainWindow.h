#pragma once

#include <QMainWindow>
#include <QObject>
#include <QPointer>

class QDialog;
class QDoubleSpinBox;
class QWidget;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
class QWebEngineView;
#endif

namespace Ui {
class MainWindow;
}

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

signals:
  void pickedCoordinate(double longitude, double latitude, double height);
  void mapStatus(const QString& message);
};

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

private slots:
  void openAddFighterDialog();
  void beginFighterCoordinatePick();
  void reportPickedCoordinate(double longitude, double latitude, double height);
  void reportMapStatus(const QString& message);

private:
  Ui::MainWindow* _ui;
  QWidget* _contentWidget;
  MapBridge* _mapBridge;
  QPointer<QDialog> _fighterDialog;
  QPointer<QDoubleSpinBox> _fighterLatitudeSpin;
  QPointer<QDoubleSpinBox> _fighterLongitudeSpin;
  QPointer<QDoubleSpinBox> _fighterHeightSpin;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  QWebEngineView* _webView;
#endif
};
