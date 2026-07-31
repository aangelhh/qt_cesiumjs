#include "presentation/KinematicsCockpitWidget.h"

#include "presentation/KinematicsCockpitModel.h"

#include <QLabel>
#include <QTabWidget>
#include <QTabBar>
#include <QStackedWidget>
#include <QVBoxLayout>

#if defined(QTTEST_HAS_MODERN_PFD)
#include <QQuickItem>
#include <QQuickWidget>
#include <QUrl>
#endif

#if defined(QTTEST_HAS_QFLIGHTINSTRUMENTS)
#include <qfi/qfi_EADI.h>

static void initializeQFlightInstrumentsResources() {
  Q_INIT_RESOURCE(qfi);
}
#endif

namespace presentation {

class KinematicsCockpitWidget::Impl {
public:
  PanelMode panelMode = PanelMode::All;
  QLabel* entityLabel = nullptr;
  QLabel* statusLabel = nullptr;
  QStackedWidget* stack = nullptr;
  QLabel* unavailableLabel = nullptr;
  QTabWidget* instrumentTabs = nullptr;
#if defined(QTTEST_HAS_MODERN_PFD)
  QQuickWidget* modernPfd = nullptr;
  QQuickWidget* ecamEngine = nullptr;
#endif
#if defined(QTTEST_HAS_QFLIGHTINSTRUMENTS)
  qfi_EADI* eadi = nullptr;
#endif
};

KinematicsCockpitWidget::KinematicsCockpitWidget(
    QWidget* parent,
    PanelMode panelMode)
    : QWidget(parent),
      _impl(std::make_unique<Impl>()) {
  _impl->panelMode = panelMode;
  setMinimumSize(420, 380);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(6, 6, 6, 6);
  layout->setSpacing(4);

  _impl->entityLabel = new QLabel(QStringLiteral("No air entity selected"), this);
  QFont titleFont = _impl->entityLabel->font();
  titleFont.setBold(true);
  _impl->entityLabel->setFont(titleFont);
  layout->addWidget(_impl->entityLabel);

  _impl->statusLabel = new QLabel(QStringLiteral("Waiting for telemetry"), this);
  _impl->statusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  layout->addWidget(_impl->statusLabel);

  _impl->stack = new QStackedWidget(this);
  _impl->unavailableLabel = new QLabel(
      QStringLiteral("Select an air entity to inspect its kinematics."),
      _impl->stack);
  _impl->unavailableLabel->setAlignment(Qt::AlignCenter);
  _impl->unavailableLabel->setWordWrap(true);
  _impl->stack->addWidget(_impl->unavailableLabel);

  _impl->instrumentTabs = new QTabWidget(_impl->stack);
  _impl->instrumentTabs->setObjectName(QStringLiteral("kinematicsCockpitTabs"));
  _impl->instrumentTabs->setDocumentMode(true);

#if defined(QTTEST_HAS_MODERN_PFD)
  if (panelMode == PanelMode::All || panelMode == PanelMode::ModernPfd) {
    _impl->modernPfd = new QQuickWidget(_impl->instrumentTabs);
    _impl->modernPfd->setObjectName(QStringLiteral("modernKinematicsPfd"));
    _impl->modernPfd->setResizeMode(QQuickWidget::SizeRootObjectToView);
    _impl->modernPfd->setClearColor(QColor(QStringLiteral("#090d10")));
    _impl->modernPfd->setSource(
        QUrl(QStringLiteral("qrc:/kinematics/ModernPfd.qml")));
    _impl->modernPfd->setMinimumSize(400, 320);
    _impl->instrumentTabs->addTab(
        _impl->modernPfd,
        QStringLiteral("Modern PFD"));
  }
#else
  if (panelMode == PanelMode::All || panelMode == PanelMode::ModernPfd) {
    auto* modernUnavailable = new QLabel(
        QStringLiteral("Modern PFD is unavailable in this build."),
        _impl->instrumentTabs);
    modernUnavailable->setAlignment(Qt::AlignCenter);
    _impl->instrumentTabs->addTab(
        modernUnavailable,
        QStringLiteral("Modern PFD"));
  }
#endif

#if defined(QTTEST_HAS_QFLIGHTINSTRUMENTS)
  if (panelMode == PanelMode::All || panelMode == PanelMode::QFlightEadi) {
    initializeQFlightInstrumentsResources();
    _impl->eadi = new qfi_EADI(_impl->instrumentTabs);
    _impl->eadi->setObjectName(QStringLiteral("qflightKinematicsEadi"));
    _impl->eadi->setFocusPolicy(Qt::NoFocus);
    _impl->eadi->setInteractive(false);
    _impl->eadi->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _impl->eadi->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _impl->eadi->setMinimumSize(300, 300);
    _impl->eadi->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _impl->instrumentTabs->addTab(
        _impl->eadi,
        QStringLiteral("QFlight EADI"));
  }
#else
  if (panelMode == PanelMode::All || panelMode == PanelMode::QFlightEadi) {
    auto* qflightUnavailable = new QLabel(
        QStringLiteral("QFlightInstruments is unavailable in this build."),
        _impl->instrumentTabs);
    qflightUnavailable->setAlignment(Qt::AlignCenter);
    _impl->instrumentTabs->addTab(
        qflightUnavailable,
        QStringLiteral("QFlight EADI"));
  }
#endif

#if defined(QTTEST_HAS_MODERN_PFD)
  if (panelMode == PanelMode::All || panelMode == PanelMode::EcamEngine) {
    _impl->ecamEngine = new QQuickWidget(_impl->instrumentTabs);
    _impl->ecamEngine->setObjectName(QStringLiteral("dynamicEcamEngine"));
    _impl->ecamEngine->setResizeMode(QQuickWidget::SizeRootObjectToView);
    _impl->ecamEngine->setClearColor(QColor(QStringLiteral("#070b0d")));
    _impl->ecamEngine->setSource(
        QUrl(QStringLiteral("qrc:/kinematics/DynamicEcam.qml")));
    _impl->ecamEngine->setMinimumSize(400, 320);
    _impl->instrumentTabs->addTab(
        _impl->ecamEngine,
        QStringLiteral("ECAM ENG"));
  }
#else
  if (panelMode == PanelMode::All || panelMode == PanelMode::EcamEngine) {
    auto* ecamUnavailable = new QLabel(
        QStringLiteral("ECAM ENG is unavailable in this build."),
        _impl->instrumentTabs);
    ecamUnavailable->setAlignment(Qt::AlignCenter);
    _impl->instrumentTabs->addTab(
        ecamUnavailable,
        QStringLiteral("ECAM ENG"));
  }
#endif

  if (panelMode != PanelMode::All) {
    _impl->instrumentTabs->tabBar()->hide();
  }

  _impl->stack->addWidget(_impl->instrumentTabs);

  layout->addWidget(_impl->stack, 1);
  clear();
}

KinematicsCockpitWidget::~KinematicsCockpitWidget() = default;

void KinematicsCockpitWidget::applySnapshot(
    const application::KinematicsTelemetrySnapshot& snapshot) {
  const KinematicsCockpitData data = makeKinematicsCockpitData(snapshot);
  _impl->entityLabel->setText(
      data.entityName.isEmpty() ? QStringLiteral("Unnamed entity") : data.entityName);
  _impl->statusLabel->setText(data.statusText);

#if defined(QTTEST_HAS_MODERN_PFD)
  if (data.available && _impl->modernPfd && _impl->modernPfd->rootObject()) {
    QQuickItem* root = _impl->modernPfd->rootObject();
    root->setProperty("entityName", data.entityName);
    root->setProperty("statusText", data.statusText);
    root->setProperty("rollDegrees", data.rollDegrees);
    root->setProperty("pitchDegrees", data.pitchDegrees);
    root->setProperty("headingDegrees", data.headingDegrees);
    root->setProperty("airspeedKnots", data.airspeedKnots);
    root->setProperty("altitudeFeet", data.altitudeFeet);
    root->setProperty(
        "verticalSpeedFpm",
        data.climbRateThousandsFeetPerMinute * 1000.0);
    root->setProperty("selectedHeadingDegrees", data.selectedHeadingDegrees);
    root->setProperty("selectedAirspeedKnots", data.selectedAirspeedKnots);
    root->setProperty("selectedAltitudeFeet", data.selectedAltitudeFeet);
    root->setProperty("flightDirectorActive", data.flightDirectorActive);
  }
  if (data.available && _impl->ecamEngine && _impl->ecamEngine->rootObject()) {
    QQuickItem* root = _impl->ecamEngine->rootObject();
    root->setProperty("entityName", data.entityName);
    root->setProperty("profileId", data.systemsProfileId);
    root->setProperty("dataSource", data.systemsDataSource);
    root->setProperty("engineModel", data.engines);
  }
#endif

#if defined(QTTEST_HAS_QFLIGHTINSTRUMENTS)
  if (data.available && _impl->eadi) {
    _impl->eadi->setRoll(data.rollDegrees);
    _impl->eadi->setPitch(data.pitchDegrees);
    _impl->eadi->setHeading(data.headingDegrees);
    _impl->eadi->setAirspeed(data.airspeedKnots);
    _impl->eadi->setAltitude(data.altitudeFeet);
    _impl->eadi->setClimbRate(data.climbRateThousandsFeetPerMinute);
    _impl->eadi->setHeadingSel(data.selectedHeadingDegrees);
    _impl->eadi->setAirspeedSel(data.selectedAirspeedKnots);
    _impl->eadi->setAltitudeSel(data.selectedAltitudeFeet);
    _impl->eadi->setVne(data.maximumAirspeedKnots);
    _impl->eadi->setFltMode(
        data.flightDirectorActive ? qfi_EADI::FltMode::FD : qfi_EADI::FltMode::Off);
    _impl->eadi->setLNAV(
        data.flightDirectorActive ? qfi_EADI::LNAV::HDG : qfi_EADI::LNAV::Off);
    _impl->eadi->setVNAV(
        data.flightDirectorActive ? qfi_EADI::VNAV::ALT_SEL : qfi_EADI::VNAV::Off);
    _impl->eadi->redraw();
  }
#endif

  if (data.available) {
    _impl->stack->setCurrentWidget(_impl->instrumentTabs);
    return;
  }

  _impl->unavailableLabel->setText(data.statusText);
  _impl->stack->setCurrentWidget(_impl->unavailableLabel);
}

void KinematicsCockpitWidget::clear() {
  _impl->entityLabel->setText(QStringLiteral("No air entity selected"));
  _impl->statusLabel->setText(QStringLiteral("Waiting for telemetry"));
  _impl->unavailableLabel->setText(
      QStringLiteral("Select an air entity to inspect its kinematics."));
  _impl->stack->setCurrentWidget(_impl->unavailableLabel);
}

} // namespace presentation
