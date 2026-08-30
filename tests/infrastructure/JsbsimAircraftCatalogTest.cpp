#include <gtest/gtest.h>

#include "infrastructure/JsbsimAircraftCatalog.h"
#include "infrastructure/JsbsimModelRepository.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

namespace {

void writeFile(const QString& path, const QByteArray& contents) {
  QFile file(path);
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
  ASSERT_EQ(file.write(contents), contents.size());
}

} // namespace

TEST(JsbsimAircraftCatalog, DiscoversOnlyCanonicalFdmConfigurations) {
  QTemporaryDir root;
  ASSERT_TRUE(root.isValid());
  ASSERT_TRUE(QDir(root.path()).mkpath(QStringLiteral("f16")));
  ASSERT_TRUE(QDir(root.path()).mkpath(QStringLiteral("auxiliary")));
  ASSERT_TRUE(QDir(root.path()).mkpath(QStringLiteral("broken")));
  writeFile(
      QDir(root.path()).absoluteFilePath(QStringLiteral("f16/f16.xml")),
      QByteArrayLiteral("<fdm_config name=\"F-16\" version=\"2.0\"/>"));
  writeFile(
      QDir(root.path()).absoluteFilePath(QStringLiteral("auxiliary/README")),
      QByteArrayLiteral("not a model"));
  writeFile(
      QDir(root.path()).absoluteFilePath(QStringLiteral("broken/broken.xml")),
      QByteArrayLiteral("<not_fdm/>"));

  const JsbsimAircraftCatalog catalog =
      JsbsimAircraftCatalog::discover(root.path());

  ASSERT_EQ(catalog.entries().size(), 1);
  EXPECT_EQ(catalog.entries().front().modelId, QStringLiteral("f16"));
  EXPECT_EQ(catalog.entries().front().displayName, QStringLiteral("F-16"));
  EXPECT_EQ(catalog.entries().front().formatVersion, QStringLiteral("2.0"));
  EXPECT_EQ(catalog.diagnostics().size(), 2);
}

TEST(JsbsimAircraftCatalog, OfficialCatalogContainsBroadModelSet) {
#ifdef QTTEST_SOURCE_DIR
  const QString aircraftRoot = QDir(QString::fromUtf8(QTTEST_SOURCE_DIR))
      .absoluteFilePath(QStringLiteral("Dependencies/jsbsim/aircraft"));
  const JsbsimAircraftCatalog catalog =
      JsbsimAircraftCatalog::discover(aircraftRoot);
  const QStringList modelIds = catalog.modelIds();

  EXPECT_GE(modelIds.size(), 50);
  EXPECT_TRUE(modelIds.contains(QStringLiteral("f16")));
  EXPECT_TRUE(modelIds.contains(QStringLiteral("f22")));
  EXPECT_TRUE(modelIds.contains(QStringLiteral("C130")));
  EXPECT_TRUE(modelIds.contains(QStringLiteral("ah1s")));
  EXPECT_TRUE(modelIds.contains(QStringLiteral("ballx")));
  EXPECT_FALSE(modelIds.contains(QStringLiteral("LM")));
#endif
}

TEST(JsbsimAircraftCatalog, MergesExternalPackagesAndReportsDuplicateIds) {
  QTemporaryDir project;
  ASSERT_TRUE(project.isValid());
  const QString externalRoot = QDir(project.path()).absoluteFilePath(
      QStringLiteral("Dependencies/jsbsim-models/experimental"));
  const QString officialRoot = QDir(project.path()).absoluteFilePath(
      QStringLiteral("Dependencies/jsbsim"));
  ASSERT_TRUE(QDir().mkpath(
      QDir(externalRoot).absoluteFilePath(QStringLiteral("aircraft/F35B"))));
  ASSERT_TRUE(QDir().mkpath(
      QDir(externalRoot).absoluteFilePath(QStringLiteral("engine"))));
  ASSERT_TRUE(QDir().mkpath(
      QDir(officialRoot).absoluteFilePath(QStringLiteral("aircraft/F35B"))));
  ASSERT_TRUE(QDir().mkpath(
      QDir(officialRoot).absoluteFilePath(QStringLiteral("engine"))));
  writeFile(
      QDir(externalRoot).absoluteFilePath(QStringLiteral("aircraft/F35B/F35B.xml")),
      QByteArrayLiteral("<fdm_config name=\"External F-35B\" version=\"2.0\"/>"));
  writeFile(
      QDir(officialRoot).absoluteFilePath(QStringLiteral("aircraft/F35B/F35B.xml")),
      QByteArrayLiteral("<fdm_config name=\"Duplicate\" version=\"2.0\"/>"));

  const QStringList roots =
      JsbsimModelRepository::modelRootsForProject(project.path());
  const JsbsimAircraftCatalog catalog =
      JsbsimAircraftCatalog::discoverModelRoots(roots);

  ASSERT_EQ(catalog.entries().size(), 1);
  EXPECT_EQ(catalog.entries().front().displayName, QStringLiteral("External F-35B"));
  EXPECT_EQ(
      catalog.entries().front().modelRootPath,
      QFileInfo(externalRoot).canonicalFilePath());
  ASSERT_EQ(catalog.diagnostics().size(), 1);
  EXPECT_TRUE(catalog.diagnostics().front().reason.contains(
      QStringLiteral("Duplicate model id")));
}

TEST(JsbsimModelRepository, ResolvesCanonicalModelsAndRejectsTraversal) {
  QTemporaryDir project;
  ASSERT_TRUE(project.isValid());
  const QString packageRoot = QDir(project.path()).absoluteFilePath(
      QStringLiteral("Dependencies/jsbsim-models/mirage"));
  ASSERT_TRUE(QDir().mkpath(
      QDir(packageRoot).absoluteFilePath(QStringLiteral("aircraft/Mirage2000"))));
  ASSERT_TRUE(QDir().mkpath(
      QDir(packageRoot).absoluteFilePath(QStringLiteral("engine"))));
  writeFile(
      QDir(packageRoot).absoluteFilePath(
          QStringLiteral("aircraft/Mirage2000/Mirage2000.xml")),
      QByteArrayLiteral("<fdm_config name=\"Mirage 2000\" version=\"2.0\"/>"));

  const QStringList roots =
      JsbsimModelRepository::modelRootsForProject(project.path());
  EXPECT_EQ(
      JsbsimModelRepository::resolveModelRoot(QStringLiteral("Mirage2000"), roots),
      QFileInfo(packageRoot).canonicalFilePath());
  EXPECT_TRUE(JsbsimModelRepository::resolveModelRoot(
                  QStringLiteral("../Mirage2000"), roots)
                  .isEmpty());
}
