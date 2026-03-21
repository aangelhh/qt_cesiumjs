#include "CesiumViewport.h"

#include <QPainter>

CesiumViewport::CesiumViewport(QWidget* parent) : QOpenGLWidget(parent) {
  this->setMinimumSize(960, 540);
}

void CesiumViewport::initializeGL() {
  this->makeCurrent();
}

void CesiumViewport::paintGL() {
  QPainter painter(this);
  painter.fillRect(this->rect(), QColor(12, 19, 28));
  painter.setPen(QColor(216, 227, 240));
  painter.drawText(
      this->rect().adjusted(24, 24, -24, -24),
      Qt::AlignLeft | Qt::TextWordWrap,
      "Viewport Qt/OpenGL preparado.\n\n"
      "Siguiente paso: conectar Cesium Native con una camara, un Tileset y un "
      "renderer que suba la geometria a GPU.");
}
