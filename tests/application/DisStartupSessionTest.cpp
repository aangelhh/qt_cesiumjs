#include <gtest/gtest.h>

#include "infrastructure/interoperability/dis/DisEntityStateCodec.h"
#include "infrastructure/interoperability/dis/DisGateway.h"
#include "infrastructure/interoperability/dis/DisRemoteEntityRegistry.h"
#include "infrastructure/interoperability/dis/DisWarfareCodec.h"
#include "infrastructure/interoperability/dis/DisSimulationControlCodec.h"
#include "infrastructure/interoperability/dis/DisEmissionCodec.h"
#include "infrastructure/interoperability/dis/DisInteractionCodec.h"
#include "application/DisStartupSession.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QThread>
#include <QUdpSocket>
#include <QProcess>
#include <QProcessEnvironment>

TEST(DisPduTester, SendsAllElevenTypesInTwoPeriodicBatches) {
  int argc = 1;
  char name[] = "dis-tester-test";
  char* argv[] = {name, nullptr};
  std::unique_ptr<QCoreApplication> app;
  if (!QCoreApplication::instance()) app = std::make_unique<QCoreApplication>(argc, argv);
  QUdpSocket observer;
  ASSERT_TRUE(observer.bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint));
  const QHostAddress group(QStringLiteral("239.255.42.98"));
  ASSERT_TRUE(observer.joinMulticastGroup(group));
  QProcess tester;
  auto environment = QProcessEnvironment::systemEnvironment();
  environment.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("offscreen"));
  tester.setProcessEnvironment(environment);
  tester.start(QStringLiteral(QTTEST_DIS_TESTER_PATH),
      {QStringLiteral("--address"), group.toString(), QStringLiteral("--port"), QString::number(observer.localPort()),
       QStringLiteral("--interval"), QStringLiteral("0.1"), QStringLiteral("--auto-start"), QStringLiteral("--batches"), QStringLiteral("2")});
  ASSERT_TRUE(tester.waitForStarted(2000));
  QHash<int, int> counts;
  std::vector<tactical::dis::EntityState> positions;
  QElapsedTimer timeout;
  timeout.start();
  while (timeout.elapsed() < 5000) {
    QCoreApplication::processEvents();
    while (observer.hasPendingDatagrams()) {
      QByteArray data(static_cast<int>(observer.pendingDatagramSize()), '\0');
      observer.readDatagram(data.data(), data.size());
      if (data.size() >= 12) {
        EXPECT_EQ(static_cast<unsigned char>(data[0]), 7);
        EXPECT_EQ(static_cast<unsigned char>(data[8]) * 256 + static_cast<unsigned char>(data[9]), data.size());
        ++counts[static_cast<unsigned char>(data[2])];
        if (data[2] == 1) {
          tactical::dis::EntityState state;
          ASSERT_TRUE(tactical::dis::DisEntityStateCodec::decode(data, state));
          positions.push_back(state);
        }
      }
    }
    if (tester.state() == QProcess::NotRunning) break;
    QThread::msleep(5);
  }
  if (tester.state() != QProcess::NotRunning) { tester.kill(); tester.waitForFinished(1000); FAIL() << "Tester did not finish"; }
  EXPECT_EQ(tester.exitStatus(), QProcess::NormalExit);
  EXPECT_EQ(tester.exitCode(), 0) << tester.readAllStandardError().toStdString();
  EXPECT_EQ(counts.size(), 11);
  for (int type : {1, 2, 3, 4, 11, 12, 13, 14, 15, 23, 28}) EXPECT_EQ(counts.value(type), 2) << "PDU " << type;
  ASSERT_EQ(positions.size(), 2U);
  EXPECT_EQ(positions[0].identifier, positions[1].identifier);
  EXPECT_GT(positions[1].longitudeDegrees, positions[0].longitudeDegrees);
  EXPECT_NEAR(positions[1].speedKnots, 250, .01);
}

TEST(DisStartupSession, ExchangesControlsAndRadarLifecycleOverUdp) {
  int argc = 1;
  char name[] = "dis-udp-test";
  char* argv[] = {name, nullptr};
  std::unique_ptr<QCoreApplication> app;
  if (!QCoreApplication::instance()) app = std::make_unique<QCoreApplication>(argc, argv);

  QUdpSocket probe;
  ASSERT_TRUE(probe.bind(QHostAddress::AnyIPv4, 0));
  const auto port = probe.localPort();
  probe.close();
  application::DisStartupConfiguration config;
  config.address = QStringLiteral("239.255.42.99");
  config.port = port;
  config.siteId = 42;
  config.applicationId = 1;
  application::DisStartupSession sender, receiver;
  auto result = sender.start(config);
  ASSERT_TRUE(result.success) << result.message.toStdString();
  config.applicationId = 2;
  result = receiver.start(config);
  ASSERT_TRUE(result.success) << result.message.toStdString();
  auto waitFor = [&](auto ready) {
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 2000) {
      QCoreApplication::processEvents();
      if (!receiver.poll().success || !sender.poll().success) return false;
      if (ready()) return true;
      QThread::msleep(5);
    }
    return false;
  };
  for (const auto control : {tactical::dis::SimulationControl::StartResume,
           tactical::dis::SimulationControl::Pause, tactical::dis::SimulationControl::Stop}) {
    ASSERT_TRUE(sender.publishSimulationControl(control, 12).success);
    std::vector<tactical::dis::SimulationControl> controls;
    ASSERT_TRUE(waitFor([&]() { controls = receiver.takeRemoteSimulationControls(); return !controls.empty(); }));
    ASSERT_EQ(controls.size(), 1U);
    EXPECT_EQ(controls.front(), control);
    EXPECT_TRUE(sender.takeRemoteSimulationControls().empty());
    std::vector<tactical::dis::Acknowledgement> acknowledgements;
    ASSERT_TRUE(waitFor([&]() {
      acknowledgements = sender.takeRemoteAcknowledgements();
      return !acknowledgements.empty();
    }));
    ASSERT_EQ(acknowledgements.size(), 1U);
    EXPECT_EQ(acknowledgements.front().source.application, 2);
    EXPECT_EQ(acknowledgements.front().destination.application, 1);
    EXPECT_EQ(acknowledgements.front().responseFlag, 1);
    EXPECT_EQ(acknowledgements.front().acknowledgeFlag,
        control == tactical::dis::SimulationControl::StartResume ? 3 : 4);
    EXPECT_TRUE(receiver.takeRemoteAcknowledgements().empty());
  }
  Entity entity;
  entity.entityId = QStringLiteral("udp-radar-host");
  SensorDefinition sensor;
  sensor.id = QStringLiteral("primary");
  entity.sensors.push_back(sensor);
  std::uint8_t systemId = 0;
  for (int stage = 0; stage < 3; ++stage) {
    if (stage == 1) entity.sensors[0].emitting = false;
    if (stage == 2) entity.sensors.clear();
    ASSERT_TRUE(sender.publishSensors({entity}).success);
    std::vector<tactical::dis::RadarEmission> emissions;
    ASSERT_TRUE(waitFor([&]() { emissions = receiver.takeRemoteRadarEmissions(); return !emissions.empty(); }));
    ASSERT_EQ(emissions.size(), 1U);
    EXPECT_EQ(emissions.front().host.site, 42);
    EXPECT_EQ(emissions.front().host.application, 1);
    if (stage == 2) {
      EXPECT_TRUE(emissions.front().beams.empty());
    } else {
      ASSERT_EQ(emissions.front().beams.size(), 1U);
      const auto& beam = emissions.front().beams.front();
      if (stage == 0) systemId = beam.systemId;
      EXPECT_EQ(beam.systemId, systemId);
      EXPECT_EQ(beam.emitting, stage == 0);
    }
    EXPECT_TRUE(sender.takeRemoteRadarEmissions().empty());
  }
  std::vector<bool> applied;
  receiver.setEntityManagementHandler([&](const auto& request) { applied.push_back(request.remove); return true; });
  for (bool remove : {false, true}) {
    tactical::dis::EntityManagementRequest request;
    request.remove = remove; request.destination = {42, 2, 100};
    ASSERT_TRUE(sender.publishEntityManagement(request).success);
    std::vector<tactical::dis::Acknowledgement> replies;
    ASSERT_TRUE(waitFor([&]() { replies = sender.takeRemoteAcknowledgements(); return !replies.empty(); }));
    ASSERT_EQ(replies.size(), 1U);
    EXPECT_EQ(replies[0].acknowledgeFlag, remove ? 2 : 1);
    EXPECT_EQ(replies[0].responseFlag, 1);
    EXPECT_EQ(replies[0].source.entity, 100);
  }
  EXPECT_EQ(applied, (std::vector<bool>{false, true}));
  tactical::dis::IffState iff;
  iff.host.entity = 30; iff.on = true; iff.operational = true;
  ASSERT_TRUE(sender.publishIff(iff).success);
  std::vector<tactical::dis::IffState> iffStates;
  ASSERT_TRUE(waitFor([&]() { iffStates = receiver.takeRemoteIffStates(); return !iffStates.empty(); }));
  EXPECT_TRUE(iffStates[0].on);
  EXPECT_EQ(iffStates[0].host.application, 1);
  EXPECT_TRUE(sender.takeRemoteIffStates().empty());
  tactical::dis::CollisionEvent collision;
  collision.source.entity = 30; collision.target = {42, 2, 100}; collision.massKilograms = 1500;
  ASSERT_TRUE(sender.publishCollision(collision).success);
  std::vector<tactical::dis::CollisionEvent> collisions;
  ASSERT_TRUE(waitFor([&]() { collisions = receiver.takeRemoteCollisions(); return !collisions.empty(); }));
  EXPECT_EQ(collisions[0].target, collision.target);
  EXPECT_EQ(collisions[0].massKilograms, 1500);
  EXPECT_TRUE(sender.takeRemoteCollisions().empty());
}

TEST(DisInteractionCodec, RoundTripsManagementIffAndCollision) {
  using namespace tactical::dis;
  for (bool remove : {false, true}) {
    EntityManagementRequest request{remove, {10, 20, 0}, {1, 1, 100}, 1, 99}, decoded;
    const auto data = DisInteractionCodec::encode(request);
    ASSERT_EQ(data.size(), 28);
    ASSERT_TRUE(DisInteractionCodec::decode(data, decoded));
    EXPECT_EQ(decoded.remove, remove); EXPECT_EQ(decoded.destination, request.destination);
    EXPECT_EQ(decoded.requestId, 99);
    EXPECT_FALSE(DisInteractionCodec::decode(data.chopped(1), decoded));
  }
  IffState iff{{10, 20, 30}, {10, 20, 9}, 1, 2, 1, true, false}, iffDecoded;
  const auto iffData = DisInteractionCodec::encode(iff);
  ASSERT_EQ(iffData.size(), 60);
  ASSERT_TRUE(DisInteractionCodec::decode(iffData, iffDecoded));
  EXPECT_EQ(iffDecoded.host, iff.host); EXPECT_EQ(iffDecoded.systemId, 2);
  EXPECT_TRUE(iffDecoded.on); EXPECT_FALSE(iffDecoded.operational);
  EXPECT_FALSE(DisInteractionCodec::decode(iffData.chopped(1), iffDecoded));
  CollisionEvent collision;
  collision.source = {10, 20, 30}; collision.target = {1, 1, 40}; collision.event = {10, 20, 7};
  collision.massKilograms = 1234; collision.velocityX = -40; collision.locationZ = 3;
  CollisionEvent decoded;
  const auto data = DisInteractionCodec::encode(collision);
  ASSERT_EQ(data.size(), 60);
  ASSERT_TRUE(DisInteractionCodec::decode(data, decoded));
  EXPECT_EQ(decoded.source, collision.source); EXPECT_EQ(decoded.target, collision.target);
  EXPECT_EQ(decoded.event, collision.event); EXPECT_EQ(decoded.massKilograms, 1234);
  EXPECT_EQ(decoded.velocityX, -40); EXPECT_EQ(decoded.locationZ, 3);
  EXPECT_FALSE(DisInteractionCodec::decode(data.chopped(1), decoded));
  collision.massKilograms = -1;
  EXPECT_TRUE(DisInteractionCodec::encode(collision).isEmpty());
}

TEST(DisGateway, RoutesManagementOnceAndFiltersOtherDestinations) {
  using namespace tactical::dis;
  DisGateway gateway;
  int applied = 0;
  gateway.setEntityManagementHandler([&](const auto&) { ++applied; return true; });
  EntityManagementRequest request{false, {10, 20, 0}, {1, 1, 100}, 1, 99};
  auto data = DisInteractionCodec::encode(request);
  ASSERT_TRUE(gateway.acceptDatagram(data));
  ASSERT_TRUE(gateway.acceptDatagram(data));
  EXPECT_EQ(applied, 1);
  request.destination.application = 2;
  request.requestId++;
  ASSERT_TRUE(gateway.acceptDatagram(DisInteractionCodec::encode(request)));
  EXPECT_EQ(applied, 1);
  request.destination.application = 1; request.exerciseId = 2;
  ASSERT_TRUE(gateway.acceptDatagram(DisInteractionCodec::encode(request)));
  EXPECT_EQ(applied, 1);
  request.exerciseId = 1; request.source = {1, 1, 0};
  ASSERT_TRUE(gateway.acceptDatagram(DisInteractionCodec::encode(request)));
  EXPECT_EQ(applied, 1);
}

TEST(DisGateway, FiltersIffAndDeduplicatesCollisionEvents) {
  using namespace tactical::dis;
  DisGateway gateway;
  IffState iff; iff.host = {10, 20, 30};
  ASSERT_TRUE(gateway.acceptDatagram(DisInteractionCodec::encode(iff)));
  EXPECT_EQ(gateway.takeRemoteIffStates().size(), 1U);
  iff.host = {1, 1, 30};
  ASSERT_TRUE(gateway.acceptDatagram(DisInteractionCodec::encode(iff)));
  EXPECT_TRUE(gateway.takeRemoteIffStates().empty());
  CollisionEvent event; event.source = {10, 20, 30}; event.event = {10, 20, 1};
  const auto data = DisInteractionCodec::encode(event);
  ASSERT_TRUE(gateway.acceptDatagram(data)); ASSERT_TRUE(gateway.acceptDatagram(data));
  EXPECT_EQ(gateway.takeRemoteCollisions().size(), 1U);
  EXPECT_TRUE(gateway.takeRemoteEntityChanges().empty());
}

TEST(DisSimulationControlCodec, RoundTripsControlsAndRejectsTruncation) {
  using namespace tactical::dis;
  for (const auto control : {SimulationControl::StartResume, SimulationControl::Pause, SimulationControl::Stop}) {
    SimulationControlMessage message;
    message.control = control;
    message.source = {10, 20, 0};
    message.requestId = 123;
    message.simulationTimeSeconds = 7201.25;
    const auto data = DisSimulationControlCodec::encode(message);
    SimulationControlMessage decoded;
    ASSERT_TRUE(DisSimulationControlCodec::decode(data, decoded));
    EXPECT_EQ(decoded.control, control);
    EXPECT_EQ(decoded.source, message.source);
    EXPECT_EQ(decoded.destination, message.destination);
    EXPECT_EQ(decoded.requestId, 123U);
    if (control == SimulationControl::StartResume) EXPECT_NEAR(decoded.simulationTimeSeconds, 7201.25, 0.00001);
    EXPECT_FALSE(DisSimulationControlCodec::decode(data.chopped(1), decoded));
  }
}

TEST(DisSimulationControlCodec, RoundTripsAcknowledgementsAndRejectsMalformedHeaders) {
  using namespace tactical::dis;
  Acknowledgement ack;
  ack.source = {1, 2, 0};
  ack.destination = {3, 4, 0};
  ack.requestId = 987;
  ack.acknowledgeFlag = 4;
  ack.responseFlag = 2;
  const auto data = DisSimulationControlCodec::encodeAcknowledgement(ack);
  ASSERT_EQ(data.size(), 32);
  Acknowledgement decoded;
  ASSERT_TRUE(DisSimulationControlCodec::decodeAcknowledgement(data, decoded));
  EXPECT_EQ(decoded.source, ack.source);
  EXPECT_EQ(decoded.destination, ack.destination);
  EXPECT_EQ(decoded.requestId, ack.requestId);
  EXPECT_EQ(decoded.acknowledgeFlag, 4);
  EXPECT_EQ(decoded.responseFlag, 2);
  EXPECT_FALSE(DisSimulationControlCodec::decodeAcknowledgement(data.chopped(1), decoded));
  auto malformed = data;
  malformed[2] = 13;
  EXPECT_FALSE(DisSimulationControlCodec::decodeAcknowledgement(malformed, decoded));
  DisGateway gateway;
  ASSERT_TRUE(gateway.acceptDatagram(data));
  EXPECT_TRUE(gateway.takeRemoteAcknowledgements().empty());
  EXPECT_TRUE(gateway.takeRemoteSimulationControls().empty());
}

TEST(DisGateway, FiltersControlRecipientsAndDuplicates) {
  using namespace tactical::dis;
  DisGateway gateway;
  SimulationControlMessage message;
  message.source = {10, 20, 0};
  auto send = [&]() { ASSERT_TRUE(gateway.acceptDatagram(DisSimulationControlCodec::encode(message))); };
  message.destination = {2, 1, 0};
  send();
  EXPECT_TRUE(gateway.takeRemoteSimulationControls().empty());
  message.destination = {1, 1, 0};
  send(); send();
  ASSERT_EQ(gateway.takeRemoteSimulationControls().size(), 1U);
  message.source = {1, 1, 0};
  send();
  EXPECT_TRUE(gateway.takeRemoteSimulationControls().empty());
  message.source = {10, 20, 0};
  message.exerciseId = 2;
  message.requestId = 1;
  send();
  EXPECT_TRUE(gateway.takeRemoteSimulationControls().empty());
}

TEST(DisEmissionCodec, RoundTripsActiveAndInactiveBeams) {
  using namespace tactical::dis;
  RadarEmission emission;
  emission.host = {10, 20, 30};
  RadarBeam beam;
  beam.emitting = true;
  beam.frequencyHertz = 10e9;
  beam.effectiveRadiatedPowerDbm = 95;
  beam.azimuthWidthDegrees = 120;
  emission.beams.push_back(beam);
  beam.systemId = 2;
  beam.emitting = false;
  emission.beams.push_back(beam);
  const auto data = DisEmissionCodec::encode(emission);
  RadarEmission decoded;
  ASSERT_TRUE(DisEmissionCodec::decode(data, decoded));
  ASSERT_EQ(decoded.beams.size(), 2U);
  EXPECT_EQ(decoded.host, emission.host);
  EXPECT_TRUE(decoded.beams[0].emitting);
  EXPECT_FALSE(decoded.beams[1].emitting);
  EXPECT_NEAR(decoded.beams[0].azimuthWidthDegrees, 120, 0.001);
  EXPECT_NEAR(decoded.beams[0].frequencyHertz, 10e9, 1024);
  EXPECT_FALSE(DisEmissionCodec::decode(data.chopped(1), decoded));
  DisGateway gateway;
  ASSERT_TRUE(gateway.acceptDatagram(data));
  EXPECT_EQ(gateway.takeRemoteRadarEmissions().size(), 1U);
  EXPECT_TRUE(gateway.takeRemoteEntityChanges().empty());
  emission.host = {1, 1, 30};
  ASSERT_TRUE(gateway.acceptDatagram(DisEmissionCodec::encode(emission)));
  EXPECT_TRUE(gateway.takeRemoteRadarEmissions().empty());
}

namespace {

tactical::dis::EntityState sampleState() {
  tactical::dis::EntityState state;
  state.identifier = {10, 20, 30};
  state.exerciseId = 7;
  state.marking = QStringLiteral("Mirage2000");
  state.forceIdentifier = 1;
  state.entityKind = 1;
  state.entityDomain = 2;
  state.country = 71;
  state.category = 1;
  state.subcategory = 4;
  state.specific = 1;
  state.latitudeDegrees = 40.4168;
  state.longitudeDegrees = -3.7038;
  state.altitudeMeters = 3200.0;
  state.headingDegrees = 275.0;
  state.pitchDegrees = 8.0;
  state.rollDegrees = -22.0;
  state.speedKnots = 410.0;
  state.verticalSpeedMetersPerSecond = 12.5;
  state.damagePercent = 50.0;
  return state;
}

tactical::dis::WarfareEvent sampleWarfareEvent(
    tactical::dis::WarfareEventKind kind) {
  tactical::dis::WarfareEvent event;
  event.kind = kind;
  event.exerciseId = 7;
  event.firingEntity = {10, 20, 30};
  event.targetEntity = {11, 21, 31};
  event.munitionEntity = {10, 20, 32768};
  event.eventIdentifier = {10, 20, 42};
  event.munitionType = QStringLiteral("Missile");
  event.latitudeDegrees = 40.4168;
  event.longitudeDegrees = -3.7038;
  event.altitudeMeters = 3200.0;
  event.headingDegrees = 275.0;
  event.pitchDegrees = 8.0;
  event.speedMetersPerSecond = 300.0;
  event.detonationResult = 1;
  return event;
}

} // namespace

TEST(DisEntityStateCodec, RoundTripsDis7EntityStatePdu) {
  const tactical::dis::EntityState expected = sampleState();
  QString error;
  const QByteArray datagram =
      tactical::dis::DisEntityStateCodec::encode(expected, &error);

  ASSERT_TRUE(error.isEmpty()) << error.toStdString();
  ASSERT_EQ(datagram.size(), 144);
  EXPECT_EQ(static_cast<unsigned char>(datagram.at(0)), 7U);

  tactical::dis::EntityState actual;
  ASSERT_TRUE(tactical::dis::DisEntityStateCodec::decode(
      datagram, actual, &error)) << error.toStdString();
  EXPECT_EQ(actual.identifier, expected.identifier);
  EXPECT_EQ(actual.exerciseId, expected.exerciseId);
  EXPECT_EQ(actual.marking, expected.marking.left(11));
  EXPECT_EQ(actual.entityDomain, expected.entityDomain);
  EXPECT_EQ(actual.country, expected.country);
  EXPECT_NEAR(actual.latitudeDegrees, expected.latitudeDegrees, 1e-6);
  EXPECT_NEAR(actual.longitudeDegrees, expected.longitudeDegrees, 1e-6);
  EXPECT_NEAR(actual.altitudeMeters, expected.altitudeMeters, 0.02);
  EXPECT_NEAR(actual.headingDegrees, expected.headingDegrees, 0.01);
  EXPECT_NEAR(actual.pitchDegrees, expected.pitchDegrees, 0.01);
  EXPECT_NEAR(actual.rollDegrees, expected.rollDegrees, 0.01);
  EXPECT_NEAR(actual.speedKnots, expected.speedKnots, 0.02);
  EXPECT_NEAR(
      actual.verticalSpeedMetersPerSecond,
      expected.verticalSpeedMetersPerSecond,
      0.01);
  EXPECT_DOUBLE_EQ(actual.damagePercent, 50.0);
  EXPECT_FALSE(actual.destroyed);
}

TEST(DisWarfareCodec, RoundTripsDis7FirePdu) {
  const auto expected = sampleWarfareEvent(
      tactical::dis::WarfareEventKind::Fire);
  QString error;
  const QByteArray datagram =
      tactical::dis::DisWarfareCodec::encodeFire(expected, &error);

  ASSERT_TRUE(error.isEmpty()) << error.toStdString();
  ASSERT_EQ(datagram.size(), 96);
  EXPECT_EQ(static_cast<unsigned char>(datagram.at(0)), 7U);
  EXPECT_EQ(static_cast<unsigned char>(datagram.at(2)), 2U);

  tactical::dis::WarfareEvent actual;
  ASSERT_TRUE(tactical::dis::DisWarfareCodec::decode(
      datagram, actual, &error)) << error.toStdString();
  EXPECT_EQ(actual.kind, tactical::dis::WarfareEventKind::Fire);
  EXPECT_EQ(actual.firingEntity, expected.firingEntity);
  EXPECT_EQ(actual.targetEntity, expected.targetEntity);
  EXPECT_EQ(actual.munitionEntity, expected.munitionEntity);
  EXPECT_EQ(actual.eventIdentifier, expected.eventIdentifier);
  EXPECT_EQ(actual.munitionType, expected.munitionType);
  EXPECT_NEAR(actual.latitudeDegrees, expected.latitudeDegrees, 1e-6);
  EXPECT_NEAR(actual.longitudeDegrees, expected.longitudeDegrees, 1e-6);
  EXPECT_NEAR(actual.altitudeMeters, expected.altitudeMeters, 0.02);
  EXPECT_NEAR(actual.headingDegrees, expected.headingDegrees, 0.01);
  EXPECT_NEAR(actual.pitchDegrees, expected.pitchDegrees, 0.01);
  EXPECT_NEAR(actual.speedMetersPerSecond, expected.speedMetersPerSecond, 0.02);
}

TEST(DisWarfareCodec, RoundTripsDis7DetonationPdu) {
  auto expected = sampleWarfareEvent(
      tactical::dis::WarfareEventKind::Detonation);
  expected.munitionType = QStringLiteral("Bomb");
  expected.detonationResult = 3;
  QString error;
  const QByteArray datagram =
      tactical::dis::DisWarfareCodec::encodeDetonation(expected, &error);

  ASSERT_TRUE(error.isEmpty()) << error.toStdString();
  ASSERT_EQ(datagram.size(), 104);
  EXPECT_EQ(static_cast<unsigned char>(datagram.at(0)), 7U);
  EXPECT_EQ(static_cast<unsigned char>(datagram.at(2)), 3U);

  tactical::dis::WarfareEvent actual;
  ASSERT_TRUE(tactical::dis::DisWarfareCodec::decode(
      datagram, actual, &error)) << error.toStdString();
  EXPECT_EQ(actual.kind, tactical::dis::WarfareEventKind::Detonation);
  EXPECT_EQ(actual.firingEntity, expected.firingEntity);
  EXPECT_EQ(actual.targetEntity, expected.targetEntity);
  EXPECT_EQ(actual.munitionEntity, expected.munitionEntity);
  EXPECT_EQ(actual.eventIdentifier, expected.eventIdentifier);
  EXPECT_EQ(actual.munitionType, expected.munitionType);
  EXPECT_EQ(actual.detonationResult, expected.detonationResult);
  EXPECT_NEAR(actual.latitudeDegrees, expected.latitudeDegrees, 1e-6);
  EXPECT_NEAR(actual.longitudeDegrees, expected.longitudeDegrees, 1e-6);
  EXPECT_NEAR(actual.altitudeMeters, expected.altitudeMeters, 0.02);
}

TEST(DisRemoteEntityRegistry, FiltersOwnApplicationAndOtherExercises) {
  tactical::dis::DisRemoteEntityRegistry registry;
  registry.configure(7, 10, 20, 1000);

  tactical::dis::EntityState own = sampleState();
  const QByteArray ownDatagram =
      tactical::dis::DisEntityStateCodec::encode(own);
  tactical::dis::EntityState decoded;
  ASSERT_TRUE(tactical::dis::DisEntityStateCodec::decode(ownDatagram, decoded));
  registry.accept(decoded, 0);
  EXPECT_TRUE(registry.takeChanges().empty());
  EXPECT_EQ(registry.size(), 0);

  tactical::dis::EntityState otherExercise = own;
  otherExercise.exerciseId = 8;
  otherExercise.identifier = {11, 21, 31};
  registry.accept(otherExercise, 0);
  EXPECT_TRUE(registry.takeChanges().empty());

  tactical::dis::EntityState remote = own;
  remote.identifier = {11, 21, 31};
  registry.accept(remote, 0);
  const auto changes = registry.takeChanges();
  ASSERT_EQ(changes.size(), 1U);
  EXPECT_FALSE(changes.front().removed);
  EXPECT_EQ(changes.front().state.identifier, remote.identifier);
  EXPECT_EQ(registry.size(), 1);
}

TEST(DisRemoteEntityRegistry, ExpiresRemoteEntityAfterConfiguredTimeout) {
  tactical::dis::DisRemoteEntityRegistry registry;
  registry.configure(7, 1, 1, 5);
  registry.accept(sampleState(), 100);
  ASSERT_EQ(registry.takeChanges().size(), 1U);

  registry.expire(106);
  const auto changes = registry.takeChanges();
  ASSERT_EQ(changes.size(), 1U);
  EXPECT_TRUE(changes.front().removed);
  EXPECT_EQ(changes.front().state.identifier, sampleState().identifier);
  EXPECT_EQ(registry.size(), 0);
}

TEST(DisGateway, CountsDecodedEntityStatePdus) {
  tactical::dis::DisGateway gateway;
  QString error;
  EXPECT_EQ(gateway.receivedPduCount(), 0U);

  ASSERT_TRUE(gateway.acceptDatagram(
      tactical::dis::DisEntityStateCodec::encode(sampleState()), &error))
      << error.toStdString();
  EXPECT_EQ(gateway.receivedPduCount(), 1U);
}

TEST(DisGateway, QueuesRemoteWarfarePdusWithoutTreatingThemAsEntities) {
  tactical::dis::DisGateway gateway;
  auto fire = sampleWarfareEvent(tactical::dis::WarfareEventKind::Fire);
  fire.exerciseId = 1;
  QString error;

  ASSERT_TRUE(gateway.acceptDatagram(
      tactical::dis::DisWarfareCodec::encodeFire(fire), &error))
      << error.toStdString();
  const auto events = gateway.takeRemoteWarfareEvents();
  ASSERT_EQ(events.size(), 1U);
  EXPECT_EQ(events.front().kind, tactical::dis::WarfareEventKind::Fire);
  EXPECT_EQ(events.front().eventIdentifier, fire.eventIdentifier);
  EXPECT_TRUE(gateway.takeRemoteEntityChanges().empty());
  EXPECT_EQ(gateway.remoteEntityCount(), 0);
  EXPECT_EQ(gateway.receivedPduCount(), 1U);
}

TEST(DisGateway, FiltersOwnAndOtherExerciseWarfarePdus) {
  tactical::dis::DisGateway gateway;
  auto fire = sampleWarfareEvent(tactical::dis::WarfareEventKind::Fire);
  QString error;

  fire.exerciseId = 2;
  ASSERT_TRUE(gateway.acceptDatagram(
      tactical::dis::DisWarfareCodec::encodeFire(fire), &error));
  EXPECT_TRUE(gateway.takeRemoteWarfareEvents().empty());

  fire.exerciseId = 1;
  fire.firingEntity = {1, 1, 30};
  ASSERT_TRUE(gateway.acceptDatagram(
      tactical::dis::DisWarfareCodec::encodeFire(fire), &error));
  EXPECT_TRUE(gateway.takeRemoteWarfareEvents().empty());
  EXPECT_EQ(gateway.receivedPduCount(), 2U);
}
