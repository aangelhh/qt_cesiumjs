#include "infrastructure/interoperability/dis/DisGateway.h"
#include "infrastructure/interoperability/dis/DisSimulationControlCodec.h"
#include "geospatial/GeographicLibGeospatialService.h"

#include <QApplication>
#include <QCheckBox>
#include <QCommandLineParser>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QTabWidget>
#include <QElapsedTimer>
#include <QVBoxLayout>
#include <array>
#include <algorithm>
#include <cmath>

using namespace tactical::dis;

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  QCoreApplication::setApplicationName(QStringLiteral("qttest-dis-tester"));
  QCommandLineParser parser;
  parser.setApplicationDescription(QStringLiteral("Periodic DIS 7 PDU tester"));
  parser.addHelpOption();
  parser.addOptions({
      {{"a", "address"}, "UDP destination", "address", "239.1.2.3"},
      {{"p", "port"}, "UDP port", "port", "3000"},
      {"exercise", "Exercise ID", "id", "1"},
      {"site", "Tester site ID", "id", "1"},
      {"application", "Tester application ID", "id", "2"},
      {"target-site", "Target site ID", "id", "1"},
      {"target-application", "Target application ID", "id", "1"},
      {"interval", "Seconds between batches", "seconds", "5"},
      {"auto-start", "Start sending on launch"},
      {"batches", "Exit after this many batches (0: unlimited)", "count", "0"}});
  parser.process(app);
  const auto integer = [&](const char* option, int minimum, int maximum) {
    bool ok = false;
    const int value = parser.value(QString::fromLatin1(option)).toInt(&ok);
    if (!ok || value < minimum || value > maximum) qFatal("Invalid --%s", option);
    return value;
  };
  const int maxBatches = integer("batches", 0, 1000000);
  bool intervalOk = false;
  const double initialInterval = parser.value(QStringLiteral("interval")).toDouble(&intervalOk);
  if (!intervalOk || !std::isfinite(initialInterval) || initialInterval < .1 || initialInterval > 3600)
    qFatal("Invalid --interval (0.1 to 3600 seconds)");

  QWidget window;
  window.setWindowTitle(QStringLiteral("DIS PDU Tester"));
  window.resize(650, 780);
  auto* layout = new QVBoxLayout(&window);
  auto* configuration = new QWidget(&window);
  auto* form = new QFormLayout(configuration);
  auto* address = new QLineEdit(parser.value(QStringLiteral("address")), configuration);
  form->addRow(QStringLiteral("Address"), address);
  auto* port = new QSpinBox(configuration);
  port->setRange(1, 65535); port->setValue(integer("port", 1, 65535));
  form->addRow(QStringLiteral("UDP port"), port);
  const auto spin = [&](const QString& label, int value, int maximum) {
    auto* input = new QSpinBox(configuration);
    input->setRange(1, maximum); input->setValue(value); form->addRow(label, input); return input;
  };
  auto* exercise = spin(QStringLiteral("Exercise"), integer("exercise", 1, 255), 255);
  auto* site = spin(QStringLiteral("Tester site"), integer("site", 1, 65534), 65534);
  auto* application = spin(QStringLiteral("Tester application"), integer("application", 1, 65534), 65534);
  auto* targetSite = spin(QStringLiteral("Target site"), integer("target-site", 1, 65534), 65534);
  auto* targetApplication = spin(QStringLiteral("Target application"), integer("target-application", 1, 65534), 65534);
  auto* entityNumber = spin(QStringLiteral("Tester entity"), 100, 32767);
  auto* targetEntity = spin(QStringLiteral("Target entity"), 100, 32767);
  auto* settingsTabs = new QTabWidget(&window);
  settingsTabs->addTab(configuration, QStringLiteral("Connection"));
  auto* motionSettings = new QWidget(&window);
  auto* motionForm = new QFormLayout(motionSettings);
  const auto number = [&](const QString& label, double value, double minimum, double maximum, int decimals) {
    auto* input = new QDoubleSpinBox(motionSettings);
    input->setRange(minimum, maximum); input->setDecimals(decimals); input->setValue(value);
    motionForm->addRow(label, input); return input;
  };
  auto* latitude = number(QStringLiteral("Initial latitude"), 40.4168, -90, 90, 6);
  auto* longitude = number(QStringLiteral("Initial longitude"), -3.7038, -180, 180, 6);
  auto* altitude = number(QStringLiteral("Altitude (m)"), 3000, -1000, 100000, 1);
  auto* heading = number(QStringLiteral("Heading (deg)"), 90, 0, 359.99, 2);
  auto* speed = number(QStringLiteral("Speed (kt)"), 250, 0, 5000, 1);
  auto* moving = new QCheckBox(QStringLiteral("Moving entity"), motionSettings);
  moving->setChecked(true); motionForm->addRow(moving);
  settingsTabs->addTab(motionSettings, QStringLiteral("Entity movement"));
  layout->addWidget(settingsTabs);
  auto* interval = new QDoubleSpinBox(&window);
  interval->setRange(.1, 3600); interval->setDecimals(1); interval->setSuffix(QStringLiteral(" s")); interval->setValue(initialInterval);
  auto* intervalForm = new QFormLayout;
  intervalForm->addRow(QStringLiteral("Batch interval"), interval);
  layout->addLayout(intervalForm);

  const QStringList names{QStringLiteral("Entity State"), QStringLiteral("Fire"), QStringLiteral("Detonation"),
      QStringLiteral("Collision"), QStringLiteral("Create Entity"), QStringLiteral("Remove Entity"),
      QStringLiteral("Start/Resume"), QStringLiteral("Stop/Freeze"), QStringLiteral("Acknowledge"),
      QStringLiteral("Electromagnetic Emission"), QStringLiteral("IFF")};
  std::array<QCheckBox*, 11> selected;
  auto* pduGrid = new QGridLayout;
  for (int i = 0; i < 11; ++i) {
    selected[i] = new QCheckBox(names[i], &window);
    selected[i]->setChecked(true);
    pduGrid->addWidget(selected[i], i / 2, i % 2);
  }
  layout->addLayout(pduGrid);
  auto* commands = new QHBoxLayout;
  auto* start = new QPushButton(QStringLiteral("Start periodic send"), &window);
  auto* stop = new QPushButton(QStringLiteral("Stop"), &window);
  auto* once = new QPushButton(QStringLiteral("Send batch"), &window);
  stop->setEnabled(false);
  commands->addWidget(start); commands->addWidget(stop); commands->addWidget(once);
  layout->addLayout(commands);
  auto* stats = new QLabel(QStringLiteral("Batches: 0 | TX: 0 | RX: 0"), &window);
  layout->addWidget(stats);
  auto* log = new QPlainTextEdit(&window);
  log->setReadOnly(true); log->setMaximumBlockCount(250); layout->addWidget(log, 1);
  const auto report = [&](const QString& message) {
    log->appendPlainText(QTime::currentTime().toString(QStringLiteral("HH:mm:ss.zzz ")) + message);
    qInfo().noquote() << message;
  };
  DisGateway gateway;
  QUdpSocket rawSender;
  QTimer sendTimer, pollTimer;
  pollTimer.setInterval(20);
  std::uint16_t eventNumber = 0;
  int batches = 0;
  int extraTx = 0;
  geospatial::GeodeticCoordinate position;
  QElapsedTimer motionClock;
  const auto open = [&]() {
    if (gateway.isActive()) return true;
    if (site->value() == targetSite->value() && application->value() == targetApplication->value()) {
      report(QStringLiteral("Tester and target must use different site/application IDs.")); return false;
    }
    const auto result = gateway.start({address->text(), static_cast<std::uint16_t>(port->value()),
        static_cast<std::uint8_t>(exercise->value()), static_cast<std::uint16_t>(site->value()),
        static_cast<std::uint16_t>(application->value()), 12});
    if (!result.success) { report(result.message); return false; }
    position = {latitude->value(), longitude->value(), altitude->value()};
    motionClock.start();
    configuration->setEnabled(false); motionSettings->setEnabled(false);
    stop->setEnabled(true); pollTimer.start(); return true;
  };
  const auto sendBatch = [&]() {
    if (!open()) { sendTimer.stop(); if (maxBatches) app.exit(1); return; }
    if (std::none_of(selected.begin(), selected.end(), [](auto* value) { return value->isChecked(); })) {
      report(QStringLiteral("Select at least one PDU.")); sendTimer.stop(); return;
    }
    ++batches;
    if (++eventNumber == 0) ++eventNumber;
    const EntityIdentifier source{static_cast<std::uint16_t>(site->value()), static_cast<std::uint16_t>(application->value()),
        static_cast<std::uint16_t>(entityNumber->value())};
    const EntityIdentifier target{static_cast<std::uint16_t>(targetSite->value()), static_cast<std::uint16_t>(targetApplication->value()),
        static_cast<std::uint16_t>(targetEntity->value())};
    const EntityIdentifier event{source.site, source.application, eventNumber};
    const auto ex = static_cast<std::uint8_t>(exercise->value());
    bool success = true;
    const auto sent = [&](int index, const Result& result) {
      success = success && result.success;
      report(result.success ? QStringLiteral("TX %1").arg(names[index]) : QStringLiteral("ERROR %1: %2").arg(names[index], result.message));
    };
    // A batch exercises both lifecycle directions; disable Remove/Stop when observing persistent state.
    if (selected[4]->isChecked()) sent(4, gateway.publishEntityManagement({false, {}, target}));
    EntityState state;
    state.identifier = source; state.exerciseId = ex; state.marking = QStringLiteral("DIS-TESTER");
    state.entityKind = 1; state.entityDomain = 2; state.category = 1;
    const double elapsed = motionClock.restart() / 1000.0;
    state.speedKnots = moving->isChecked() ? speed->value() : 0;
    state.headingDegrees = heading->value();
    // Move using elapsed wall time, so changing the send interval does not change speed.
    position = geospatial::wgs84GeospatialService().directGeodesic(position,
        state.headingDegrees, state.speedKnots * 0.514444 * elapsed);
    state.latitudeDegrees = position.latitude; state.longitudeDegrees = position.longitude;
    state.altitudeMeters = altitude->value();
    if (selected[0]->isChecked()) sent(0, gateway.publish({state}));
    if (selected[9]->isChecked()) {
      RadarEmission emission; emission.host = source; emission.exerciseId = ex;
      RadarBeam beam; beam.emitting = true; beam.frequencyHertz = 10e9; beam.effectiveRadiatedPowerDbm = 80;
      emission.beams.push_back(beam); sent(9, gateway.publishRadar(emission));
    }
    if (selected[10]->isChecked()) sent(10, gateway.publishIff({source, event, ex, 1, 1, true, true}));
    WarfareEvent warfare; warfare.exerciseId = ex; warfare.firingEntity = source; warfare.targetEntity = target;
    warfare.munitionEntity = {source.site, source.application, 32768}; warfare.eventIdentifier = event;
    warfare.munitionType = QStringLiteral("Missile"); warfare.latitudeDegrees = state.latitudeDegrees;
    warfare.longitudeDegrees = state.longitudeDegrees; warfare.altitudeMeters = state.altitudeMeters;
    if (selected[1]->isChecked()) sent(1, gateway.publishWarfareEvent(warfare));
    if (selected[3]->isChecked()) {
      CollisionEvent collision; collision.source = source; collision.target = target; collision.event = event;
      collision.exerciseId = ex; collision.massKilograms = 1000;
      sent(3, gateway.publishCollision(collision));
    }
    warfare.kind = WarfareEventKind::Detonation; warfare.detonationResult = 1;
    if (selected[2]->isChecked()) sent(2, gateway.publishWarfareEvent(warfare));
    if (selected[6]->isChecked()) sent(6, gateway.publishSimulationControl(SimulationControl::StartResume, 0));
    if (selected[7]->isChecked()) sent(7, gateway.publishSimulationControl(SimulationControl::Pause, 0));
    if (selected[5]->isChecked()) sent(5, gateway.publishEntityManagement({true, {}, target}));
    if (selected[8]->isChecked()) {
      // Deliberately unsolicited: exercises the peer's unknown-request ACK filter.
      Acknowledgement ack; ack.source = source; ack.destination = {target.site, target.application, 0};
      ack.exerciseId = ex; ack.requestId = eventNumber;
      const auto data = DisSimulationControlCodec::encodeAcknowledgement(ack);
      const bool ok = rawSender.writeDatagram(data, QHostAddress(address->text()), port->value()) == data.size();
      if (ok) ++extraTx;
      sent(8, ok ? Result{} : Result::fail(rawSender.errorString()));
    }
    if (maxBatches && batches >= maxBatches) {
      sendTimer.stop(); QTimer::singleShot(250, &app, [&, success]() { app.exit(success ? 0 : 1); });
    }
  };
  QObject::connect(&pollTimer, &QTimer::timeout, &window, [&]() {
    const auto result = gateway.poll();
    if (!result.success) report(result.message);
    for (const auto& ack : gateway.takeRemoteAcknowledgements())
      report(QStringLiteral("RX Acknowledge from %1 request=%2 flag=%3 response=%4")
          .arg(ack.source.key()).arg(ack.requestId).arg(ack.acknowledgeFlag).arg(ack.responseFlag));
    for (const auto& change : gateway.takeRemoteEntityChanges())
      report(QStringLiteral("RX Entity %1%2").arg(change.state.identifier.key(), change.removed ? QStringLiteral(" expired") : QString()));
    gateway.takeRemoteWarfareEvents(); gateway.takeRemoteRadarEmissions(); gateway.takeRemoteIffStates();
    gateway.takeRemoteCollisions(); gateway.takeRemoteSimulationControls();
    stats->setText(QStringLiteral("Batches: %1 | TX: %2 | RX: %3").arg(batches).arg(gateway.publishedPduCount() + extraTx).arg(gateway.receivedPduCount()));
  });
  QObject::connect(&sendTimer, &QTimer::timeout, &window, sendBatch);
  QObject::connect(once, &QPushButton::clicked, &window, sendBatch);
  const auto begin = [&]() {
    if (!open()) { if (maxBatches) app.exit(1); return; }
    sendTimer.start(qRound(interval->value() * 1000)); start->setEnabled(false); sendBatch();
  };
  QObject::connect(start, &QPushButton::clicked, &window, begin);
  QObject::connect(interval, &QDoubleSpinBox::valueChanged, &window, [&](double seconds) {
    if (sendTimer.isActive()) sendTimer.setInterval(qRound(seconds * 1000));
  });
  QObject::connect(stop, &QPushButton::clicked, &window, [&]() {
    sendTimer.stop(); pollTimer.stop(); gateway.stop(); configuration->setEnabled(true);
    motionSettings->setEnabled(true);
    start->setEnabled(true); stop->setEnabled(false); report(QStringLiteral("Stopped."));
  });
  window.show();
  if (qEnvironmentVariableIsSet("DIS_TESTER_SCREENSHOT")) QTimer::singleShot(100, &window, [&]() {
    window.grab().save(qEnvironmentVariable("DIS_TESTER_SCREENSHOT"));
  });
  if (parser.isSet(QStringLiteral("auto-start"))) QTimer::singleShot(0, &window, begin);
  return app.exec();
}
