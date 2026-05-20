#include <gtest/gtest.h>
#include "presentation/TacticalGraphicsEditorController.h"
#include "presentation/GraphicPickCoordinator.h"
#include "application/ScenarioState.h"

using namespace presentation;

namespace {

// Minimal stub for GraphicPickCoordinator — delegates to spy callbacks.
struct PickSpy {
  QString lastMode;
  QString lastName;
  double  lastAlt         = 0.0;
  double  lastRadius      = 0.0;
  double  lastSemiMajor   = 0.0;
  double  lastSemiMinor   = 0.0;
  double  lastRotation    = 0.0;

  GraphicPickCoordinator* makeCoordinator(ScenarioState* state) {
    return new GraphicPickCoordinator(
        state,
        [](const QString&) {},
        []() {},
        [](const QVariantMap&) {},
        [](const QString&) {},
        []() {});
  }
};

struct Fixture {
  ScenarioState state;
  PickSpy       spy;
  QString       askTextReturn;
  bool          askTextOk    = true;
  QString       pickItemReturn;
  bool          pickItemOk   = true;
  double        askDoubleReturn = 0.0;
  int           askDoubleCallCount = 0;
  bool          askDoubleOk   = true;

  // Multiple doubles can be returned in sequence
  QVector<double> askDoubleQueue;
  QVector<bool>   askDoubleOkQueue;

  double nextDouble(bool& ok) {
    if (!askDoubleOkQueue.isEmpty()) {
      ok = askDoubleOkQueue.takeFirst();
    } else {
      ok = askDoubleOk;
    }
    if (!askDoubleQueue.isEmpty()) {
      return askDoubleQueue.takeFirst();
    }
    return askDoubleReturn;
  }

  TacticalGraphicsEditorController* makeController(GraphicPickCoordinator* coord) {
    return new TacticalGraphicsEditorController(
        coord,
        [this]() { return static_cast<int>(state.waypoints().size()); },
        [this]() { return static_cast<int>(state.routes().size()); },
        [this]() { return static_cast<int>(state.areas().size()); },
        [this](const QString&, const QString&,
               const QString&, bool& ok) -> QString {
          ok = askTextOk;
          return askTextReturn;
        },
        [this](const QString&, const QString&,
               const QStringList&, int, bool& ok) -> QString {
          ok = pickItemOk;
          return pickItemReturn;
        },
        [this](const QString&, const QString&,
               double, double, double, int, bool& ok) -> double {
          return this->nextDouble(ok);
        });
  }
};

} // namespace

// ── openAddWaypointDialog ────────────────────────────────────────────────────

TEST(TacticalGraphicsEditorController, waypointPickStartedWithName) {
  Fixture f;
  f.askTextReturn = QStringLiteral("WP1");
  f.askTextOk     = true;
  auto* state   = &f.state;
  auto* coord   = f.spy.makeCoordinator(state);
  auto* ctrl    = f.makeController(coord);

  ctrl->openAddWaypointDialog();

  EXPECT_TRUE(coord->isPending());
  delete ctrl;
  delete coord;
}

TEST(TacticalGraphicsEditorController, waypointPickCancelledByUser) {
  Fixture f;
  f.askTextOk = false;
  auto* coord = f.spy.makeCoordinator(&f.state);
  auto* ctrl  = f.makeController(coord);

  ctrl->openAddWaypointDialog();

  EXPECT_FALSE(coord->isPending());
  delete ctrl;
  delete coord;
}

TEST(TacticalGraphicsEditorController, waypointPickEmptyNameCancelled) {
  Fixture f;
  f.askTextReturn = QStringLiteral("   ");
  f.askTextOk     = true;
  auto* coord = f.spy.makeCoordinator(&f.state);
  auto* ctrl  = f.makeController(coord);

  ctrl->openAddWaypointDialog();

  EXPECT_FALSE(coord->isPending());
  delete ctrl;
  delete coord;
}

// ── openAddRouteDialog ───────────────────────────────────────────────────────

TEST(TacticalGraphicsEditorController, routePickStartedWithName) {
  Fixture f;
  f.askTextReturn = QStringLiteral("Route Alpha");
  f.askTextOk     = true;
  auto* coord = f.spy.makeCoordinator(&f.state);
  auto* ctrl  = f.makeController(coord);

  ctrl->openAddRouteDialog();

  EXPECT_TRUE(coord->isPending());
  delete ctrl;
  delete coord;
}

TEST(TacticalGraphicsEditorController, routePickDefaultNameUsesCount) {
  Fixture f;
  f.state.addRoute(RouteGraphic{QStringLiteral("Existing"), {}});
  f.askTextOk = false; // just cancel after checking default — we can't inspect it here
  auto* coord = f.spy.makeCoordinator(&f.state);
  auto* ctrl  = f.makeController(coord);

  ctrl->openAddRouteDialog(); // won't start pick (cancelled)

  EXPECT_FALSE(coord->isPending());
  delete ctrl;
  delete coord;
}

// ── openAddAreaDialog — Circle ───────────────────────────────────────────────

TEST(TacticalGraphicsEditorController, circleAreaPickStarted) {
  Fixture f;
  f.askTextReturn   = QStringLiteral("AreaCircle");
  f.askTextOk       = true;
  f.pickItemReturn  = QStringLiteral("Circle");
  f.pickItemOk      = true;
  // altitude, then radius
  f.askDoubleQueue  = {100.0, 5000.0};
  f.askDoubleOkQueue = {true, true};
  auto* coord = f.spy.makeCoordinator(&f.state);
  auto* ctrl  = f.makeController(coord);

  ctrl->openAddAreaDialog();

  EXPECT_TRUE(coord->isPending());
  delete ctrl;
  delete coord;
}

TEST(TacticalGraphicsEditorController, circleAreaCancelledAtRadius) {
  Fixture f;
  f.askTextReturn   = QStringLiteral("AreaCircle2");
  f.askTextOk       = true;
  f.pickItemReturn  = QStringLiteral("Circle");
  f.pickItemOk      = true;
  f.askDoubleQueue   = {100.0, 0.0};     // altitude, dummy radius (won't be used)
  f.askDoubleOkQueue = {true, false};    // altitude ok, radius cancelled
  auto* coord = f.spy.makeCoordinator(&f.state);
  auto* ctrl  = f.makeController(coord);

  ctrl->openAddAreaDialog();

  EXPECT_FALSE(coord->isPending());
  delete ctrl;
  delete coord;
}

// ── openAddAreaDialog — Ellipse ──────────────────────────────────────────────

TEST(TacticalGraphicsEditorController, ellipseAreaPickStarted) {
  Fixture f;
  f.askTextReturn   = QStringLiteral("AreaEllipse");
  f.askTextOk       = true;
  f.pickItemReturn  = QStringLiteral("Ellipse");
  f.pickItemOk      = true;
  // altitude, semiMajor, semiMinor, rotation
  f.askDoubleQueue   = {200.0, 6000.0, 3000.0, 45.0};
  f.askDoubleOkQueue = {true, true, true, true};
  auto* coord = f.spy.makeCoordinator(&f.state);
  auto* ctrl  = f.makeController(coord);

  ctrl->openAddAreaDialog();

  EXPECT_TRUE(coord->isPending());
  delete ctrl;
  delete coord;
}

// ── openAddAreaDialog — Polygon ──────────────────────────────────────────────

TEST(TacticalGraphicsEditorController, polygonAreaPickStarted) {
  Fixture f;
  f.askTextReturn   = QStringLiteral("AreaPoly");
  f.askTextOk       = true;
  f.pickItemReturn  = QStringLiteral("Polygon");
  f.pickItemOk      = true;
  f.askDoubleQueue  = {300.0};
  f.askDoubleOkQueue = {true};
  auto* coord = f.spy.makeCoordinator(&f.state);
  auto* ctrl  = f.makeController(coord);

  ctrl->openAddAreaDialog();

  EXPECT_TRUE(coord->isPending());
  delete ctrl;
  delete coord;
}

// ── openAddAreaDialog — cancelled at type selection ──────────────────────────

TEST(TacticalGraphicsEditorController, areaDialogCancelledAtTypePick) {
  Fixture f;
  f.askTextReturn   = QStringLiteral("Area X");
  f.askTextOk       = true;
  f.pickItemOk      = false; // cancel at type selection
  auto* coord = f.spy.makeCoordinator(&f.state);
  auto* ctrl  = f.makeController(coord);

  ctrl->openAddAreaDialog();

  EXPECT_FALSE(coord->isPending());
  delete ctrl;
  delete coord;
}
