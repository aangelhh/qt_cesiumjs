#pragma once

#include "CesiumRuntime.h"

#include <QMainWindow>

class QLabel;
class QTimer;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);

private:
  void refreshStatus();

  CesiumRuntime _runtime;
  QLabel* _statusLabel;
  QTimer* _timer;
};
