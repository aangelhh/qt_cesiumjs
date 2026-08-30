#include "infrastructure/JsbsimModelRepository.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QSet>

namespace {

bool isModelRoot(const QString& path) {
  const QDir root(path);
  return root.exists(QStringLiteral("aircraft")) &&
      root.exists(QStringLiteral("engine"));
}

void appendRoot(QStringList& roots, QSet<QString>& seen, const QString& path) {
  const QString canonical = QFileInfo(path).canonicalFilePath();
  if (canonical.isEmpty() || seen.contains(canonical) || !isModelRoot(canonical)) {
    return;
  }
  seen.insert(canonical);
  roots.push_back(canonical);
}

void appendRootOrPackages(
    QStringList& roots,
    QSet<QString>& seen,
    const QString& path) {
  if (isModelRoot(path)) {
    appendRoot(roots, seen, path);
    return;
  }

  const QDir container(path);
  const QFileInfoList packages = container.entryInfoList(
      QDir::Dirs | QDir::NoDotAndDotDot,
      QDir::Name | QDir::IgnoreCase);
  for (const QFileInfo& package : packages) {
    appendRoot(roots, seen, package.absoluteFilePath());
  }
}

void appendProjectRoots(
    QStringList& roots,
    QSet<QString>& seen,
    const QString& projectRoot) {
  const QDir project(projectRoot);
  appendRootOrPackages(
      roots,
      seen,
      project.absoluteFilePath(QStringLiteral("Dependencies/jsbsim-models")));
  appendRoot(
      roots,
      seen,
      project.absoluteFilePath(QStringLiteral("Dependencies/jsbsim")));
}

bool isSafeModelId(const QString& modelId) {
  const QString trimmed = modelId.trimmed();
  return !trimmed.isEmpty() && trimmed != QStringLiteral(".") &&
      trimmed != QStringLiteral("..") && !trimmed.contains('/') &&
      !trimmed.contains('\\');
}

} // namespace

QStringList JsbsimModelRepository::defaultModelRoots() {
  QStringList roots;
  QSet<QString> seen;

  const QString configuredPaths = QProcessEnvironment::systemEnvironment()
      .value(QStringLiteral("QTTEST_JSBSIM_MODEL_PATH"));
  for (const QString& configuredPath :
       configuredPaths.split(QDir::listSeparator(), Qt::SkipEmptyParts)) {
    appendRootOrPackages(roots, seen, configuredPath.trimmed());
  }

#ifdef QTTEST_SOURCE_DIR
  appendProjectRoots(roots, seen, QString::fromUtf8(QTTEST_SOURCE_DIR));
#endif

  QDir cursor(QCoreApplication::applicationDirPath());
  for (int depth = 0; depth < 8; ++depth) {
    appendProjectRoots(roots, seen, cursor.absolutePath());
    if (!cursor.cdUp()) {
      break;
    }
  }
  return roots;
}

QStringList JsbsimModelRepository::modelRootsForProject(
    const QString& projectRoot) {
  QStringList roots;
  QSet<QString> seen;
  appendProjectRoots(roots, seen, projectRoot);
  return roots;
}

QString JsbsimModelRepository::resolveModelRoot(
    const QString& modelId,
    const QStringList& modelRoots) {
  if (!isSafeModelId(modelId)) {
    return {};
  }

  const QString trimmedId = modelId.trimmed();
  for (const QString& rootPath : modelRoots) {
    const QDir root(rootPath);
    const QString configurationPath = root.absoluteFilePath(
        QStringLiteral("aircraft/%1/%1.xml").arg(trimmedId));
    if (QFileInfo::exists(configurationPath)) {
      return root.absolutePath();
    }
  }
  return {};
}

QString JsbsimModelRepository::resolveModelRoot(const QString& modelId) {
  return resolveModelRoot(modelId, defaultModelRoots());
}
