#pragma once

#include <QString>
#include <QStringList>

class JsbsimModelRepository {
public:
  static QStringList defaultModelRoots();
  static QStringList modelRootsForProject(const QString& projectRoot);
  static QString resolveModelRoot(
      const QString& modelId,
      const QStringList& modelRoots);
  static QString resolveModelRoot(const QString& modelId);
};
