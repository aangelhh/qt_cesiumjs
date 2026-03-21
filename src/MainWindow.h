#pragma once

#include <QMainWindow>

class QLabel;
class QWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);

private:
  QLabel* _statusLabel;
  QWidget* _contentWidget;
};
