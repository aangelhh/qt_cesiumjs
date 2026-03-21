#include "MainWindow.h"

#include "CesiumViewport.h"

#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), _statusLabel(nullptr), _timer(nullptr) {
  this->setWindowTitle("Qt + Cesium Native");
  this->resize(1280, 800);

  auto* central = new QWidget(this);
  auto* layout = new QVBoxLayout(central);
  layout->setContentsMargins(16, 16, 16, 16);
  layout->setSpacing(12);

  auto* title = new QLabel("Base de aplicacion Qt/C++ preparada para Cesium Native", central);
  title->setWordWrap(true);

  auto* instructions = new QLabel(
      "Variables de entorno esperadas: CESIUM_ION_ACCESS_TOKEN y "
      "CESIUM_ION_ASSET_ID.",
      central);
  instructions->setWordWrap(true);

  this->_statusLabel =
      new QLabel(QString::fromStdString(this->_runtime.statusText()), central);
  this->_statusLabel->setWordWrap(true);

  auto* viewport = new CesiumViewport(central);

  layout->addWidget(title);
  layout->addWidget(instructions);
  layout->addWidget(this->_statusLabel);
  layout->addWidget(viewport, 1);

  this->setCentralWidget(central);

  this->_runtime.startFromEnvironment();
  this->refreshStatus();

  this->_timer = new QTimer(this);
  this->_timer->setInterval(16);
  QObject::connect(this->_timer, &QTimer::timeout, this, [this]() {
    this->_runtime.tick();
    this->refreshStatus();
  });
  this->_timer->start();
}

void MainWindow::refreshStatus() {
  this->_statusLabel->setText(QString::fromStdString(this->_runtime.statusText()));
}
