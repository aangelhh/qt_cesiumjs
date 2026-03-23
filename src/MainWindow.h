#pragma once

#include <QMainWindow>
#include <QModelIndex>
#include <QPointer>
#include <QVariantMap>

class AddEntityDialog;
class MapBridge;
class ScenarioState;
class QStandardItem;
class QStandardItemModel;
class QWidget;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
class QWebEngineView;
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

private slots:
  void openAddEntityDialog();
  void beginEntityCoordinatePick();
  void reportPickedCoordinate(double longitude, double latitude, double height);
  void reportMapStatus(const QString& message);
  void updateSelectedTrackPanel(const QModelIndex& current, const QModelIndex& previous);
  void handleMapTrackSelection(const QString& trackName);
  void toggleTacticalOverlays(bool enabled);

private:
  void initializeModels();
  void appendEntityToUi(const class Entity& entity);
  QStandardItem* rootItemForForceIdentifier(int forceIdentifier) const;
  QStandardItem* ensureGroupItem(QStandardItem* parent, const QString& label, const QVariantMap& summary);
  void appendLogMessage(const QString& message);
  void setSelectedTrackDetails(const QVariantMap& summary);
  void sendTrackToMap(const QVariantMap& summary, bool focus = false);
  void syncTracksToMap();
  void selectObjectByName(const QString& trackName, bool notifyMap);
  QStandardItem* findTrackItemByName(QStandardItem* parent, const QString& trackName) const;

  Ui::MainWindow* _ui;
  QWidget* _contentWidget;
  MapBridge* _mapBridge;
  ScenarioState* _scenarioState;
  QStandardItemModel* _objectsModel;
  QStandardItem* _friendlyRootItem;
  QStandardItem* _opposingRootItem;
  QStandardItem* _neutralRootItem;
  QPointer<AddEntityDialog> _entityDialog;
  bool _applyingMapSelection;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  QWebEngineView* _webView;
#endif
};
