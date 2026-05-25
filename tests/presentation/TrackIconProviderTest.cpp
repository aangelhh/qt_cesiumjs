#include <gtest/gtest.h>
#include "../../src/presentation/TrackIconProvider.h"
#include <QGuiApplication>

using namespace presentation;

// QGuiApplication fixture — required for QPixmap/QPainter tests
class GuiTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    if (!QGuiApplication::instance()) {
      static int argc = 0;
      static char* argv[] = {nullptr};
      s_app = new QGuiApplication(argc, argv);
    }
  }
  static void TearDownTestSuite() {
    // keep alive for entire test binary run
  }
  static QGuiApplication* s_app;
};
QGuiApplication* GuiTest::s_app = nullptr;

TEST(TrackIconProvider, ForceColorFromLabelFriendly) {
  const QColor c = forceColorFromLabel(QStringLiteral("Friendly"));
  EXPECT_EQ(c.name(), QStringLiteral("#55d3ff"));
}

TEST(TrackIconProvider, ForceColorFromLabelOpposing) {
  const QColor c = forceColorFromLabel(QStringLiteral("Opposing"));
  EXPECT_EQ(c.name(), QStringLiteral("#ff3b30"));
}

TEST(TrackIconProvider, ForceColorFromLabelNeutral) {
  const QColor c = forceColorFromLabel(QStringLiteral("Neutral"));
  EXPECT_EQ(c.name(), QStringLiteral("#35c759"));
}

TEST(TrackIconProvider, ForceColorFromLabelUnknown) {
  const QColor c = forceColorFromLabel(QStringLiteral("Unknown"));
  EXPECT_EQ(c.name(), QStringLiteral("#ffd60a"));
}

TEST(TrackIconProvider, CategoryGlyphKnownValues) {
  EXPECT_EQ(categoryGlyph(QStringLiteral("Fighter")),     QStringLiteral("F"));
  EXPECT_EQ(categoryGlyph(QStringLiteral("Bomber")),      QStringLiteral("B"));
  EXPECT_EQ(categoryGlyph(QStringLiteral("Helicopter")),  QStringLiteral("H"));
  EXPECT_EQ(categoryGlyph(QStringLiteral("Transport")),   QStringLiteral("T"));
  EXPECT_EQ(categoryGlyph(QStringLiteral("Tank")),        QStringLiteral("K"));
  EXPECT_EQ(categoryGlyph(QStringLiteral("Truck")),       QStringLiteral("R"));
  EXPECT_EQ(categoryGlyph(QStringLiteral("Radar")),       QStringLiteral("D"));
  EXPECT_EQ(categoryGlyph(QStringLiteral("Other")),       QStringLiteral("O"));
  EXPECT_EQ(categoryGlyph(QStringLiteral("Side")),        QStringLiteral("S"));
}

TEST(TrackIconProvider, CategoryGlyphArmoredVehicleVariants) {
  EXPECT_EQ(categoryGlyph(QStringLiteral("Armored Vehicle")),  QStringLiteral("V"));
  EXPECT_EQ(categoryGlyph(QStringLiteral("Armoured Vehicle")), QStringLiteral("V"));
  EXPECT_EQ(categoryGlyph(QStringLiteral("armored-vehicle")),  QStringLiteral("V"));
}

TEST(TrackIconProvider, CategoryGlyphFallback) {
  EXPECT_EQ(categoryGlyph(QStringLiteral("Submarine")), QStringLiteral("E"));
  EXPECT_EQ(categoryGlyph(QStringLiteral("")),          QStringLiteral("E"));
}

TEST_F(GuiTest, MakeTrackIconNonNull) {
  const QIcon icon = makeTrackIcon(
      QStringLiteral("Friendly"), QStringLiteral("Fighter"), false);
  EXPECT_FALSE(icon.isNull());
}

TEST_F(GuiTest, MakeTacticalGraphicIconNonNull) {
  EXPECT_FALSE(makeTacticalGraphicIcon(QStringLiteral("Route")).isNull());
  EXPECT_FALSE(makeTacticalGraphicIcon(QStringLiteral("Waypoint")).isNull());
  EXPECT_FALSE(makeTacticalGraphicIcon(QStringLiteral("Engagement Area")).isNull());
  EXPECT_FALSE(makeTacticalGraphicIcon(QStringLiteral("Other")).isNull());
}
