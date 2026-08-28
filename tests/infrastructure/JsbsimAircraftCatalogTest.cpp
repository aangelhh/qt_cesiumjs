#include <gtest/gtest.h>

#include "infrastructure/JsbsimAircraftCatalog.h"

#include <QDir>
#include <QFile>
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
