#pragma once

#include <QOpenGLWidget>

class CesiumViewport : public QOpenGLWidget {
  Q_OBJECT

public:
  explicit CesiumViewport(QWidget* parent = nullptr);

protected:
  void initializeGL() override;
  void paintGL() override;
};
