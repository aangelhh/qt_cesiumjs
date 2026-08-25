#include "application/FlightDynamicsEngine.h"
#include "application/RuntimeBenchmarkStatistics.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include <QSysInfo>
#include <QTextStream>

#include <chrono>
#include <cmath>
#include <unordered_map>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

struct BenchmarkConfiguration {
  QVector<int> entityCounts;
  QStringList backends;
  QString jsbsimModel;
  int warmupTicks = 60;
  int measuredTicks = 300;
  double frequencyHertz = 60.0;
  QString outputPath;
};

struct BenchmarkCaseResult {
  QString backend;
  QString model;
  int entityCount = 0;
  int warmupTicks = 0;
  int measuredTicks = 0;
  double frequencyHertz = 0.0;
  double initializationMilliseconds = 0.0;
  application::RuntimeBenchmarkStatistics tickStatistics;
  application::RuntimeBenchmarkStatistics modelStepStatistics;
  int fallbackCount = 0;
};

double elapsedMilliseconds(Clock::time_point start, Clock::time_point end) {
  return std::chrono::duration<double, std::milli>(end - start).count();
}

bool parsePositiveInteger(const QString& text, int* value) {
  bool ok = false;
  const int parsed = text.toInt(&ok);
  if (!ok || parsed <= 0) {
    return false;
  }
  *value = parsed;
  return true;
}

QVector<int> parseEntityCounts(const QString& text, QString* error) {
  QVector<int> counts;
  QSet<int> seen;
  for (const QString& token : text.split(',', Qt::SkipEmptyParts)) {
    int count = 0;
    if (!parsePositiveInteger(token.trimmed(), &count)) {
      *error = QStringLiteral("Invalid entity count: %1").arg(token);
      return {};
    }
    if (!seen.contains(count)) {
      counts.push_back(count);
      seen.insert(count);
    }
  }
  if (counts.isEmpty()) {
    *error = QStringLiteral("At least one entity count is required");
  }
  return counts;
}

QStringList parseBackends(const QString& text, QString* error) {
  const QString normalized = text.trimmed().toLower();
  if (normalized == QStringLiteral("both")) {
    return {QStringLiteral("kinematic"), QStringLiteral("jsbsim")};
  }
  if (normalized == QStringLiteral("kinematic") ||
      normalized == QStringLiteral("jsbsim")) {
    return {normalized};
  }
  *error = QStringLiteral("Backend must be kinematic, jsbsim, or both");
  return {};
}

Entity makeBenchmarkEntity(
    const QString& backend,
    const QString& model,
    int index) {
  Entity entity;
  entity.entityId = QStringLiteral("benchmark-%1-%2").arg(backend).arg(index);
  entity.name = QStringLiteral("Benchmark %1 %2").arg(backend).arg(index);
  entity.domain = QStringLiteral("Air");
  entity.category = QStringLiteral("Fighter");
  entity.latitude = 40.0 + static_cast<double>(index) * 0.002;
  entity.longitude = -3.0;
  entity.altitude = 3000 + index * 10;
  entity.headingDegrees = 90.0;
  entity.speedKnots = 320.0;
  entity.flightDynamicsEnabled = backend == QStringLiteral("jsbsim");
  entity.flightDynamicsMode = backend;
  entity.jsbsimAircraftModel = model;
  entity.controlProfileId = QStringLiteral("fighter-generic");
  entity.fuelCapacityKilograms = 3000.0;
  entity.fuelRemainingKilograms = 2000.0;
  entity.currentTask.enabled = true;
  entity.currentTask.status = QStringLiteral("Running");
  entity.currentTask.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
  entity.currentTask.targetHeadingDegrees = 110.0;
  entity.currentTask.targetAltitudeMeters = 3600;
  entity.currentTask.targetSpeedKnots = 360.0;
  return entity;
}

void updateSetpoints(QVector<Entity>& entities, int tick) {
  const int phase = (tick / 120) % 3;
  const double heading = phase == 0 ? 110.0 : (phase == 1 ? 70.0 : 145.0);
  const int altitude = phase == 0 ? 3600 : (phase == 1 ? 2800 : 4200);
  const double speed = phase == 0 ? 360.0 : (phase == 1 ? 300.0 : 400.0);
  for (Entity& entity : entities) {
    entity.currentTask.targetHeadingDegrees = heading;
    entity.currentTask.targetAltitudeMeters = altitude;
    entity.currentTask.targetSpeedKnots = speed;
  }
}

BenchmarkCaseResult runBenchmarkCase(
    const BenchmarkConfiguration& configuration,
    const QString& backend,
    int entityCount) {
  FlightDynamicsEngine::clearDynamicsModels();
  QVector<Entity> entities;
  entities.reserve(entityCount);
  for (int index = 0; index < entityCount; ++index) {
    entities.push_back(makeBenchmarkEntity(
        backend,
        configuration.jsbsimModel,
        index));
  }

  std::unordered_map<QString, domain::TaskStack> taskStacks;
  const FlightDynamicsExecutionPolicy deterministicPolicy{
      /*enforceWallClockStepBudget=*/false};
  const double deltaSeconds = 1.0 / configuration.frequencyHertz;
  double simulationTimeSeconds = 0.0;
  int tick = 0;

  const auto advanceOneTick = [&]() {
    updateSetpoints(entities, tick);
    simulationTimeSeconds += deltaSeconds;
    const auto start = Clock::now();
    FlightDynamicsEngine::advanceEntities(
        entities,
        taskStacks,
        simulationTimeSeconds,
        deltaSeconds,
        deterministicPolicy);
    const auto end = Clock::now();
    ++tick;
    return elapsedMilliseconds(start, end);
  };

  BenchmarkCaseResult result;
  result.backend = backend;
  result.model = backend == QStringLiteral("jsbsim")
      ? configuration.jsbsimModel
      : QStringLiteral("built-in");
  result.entityCount = entityCount;
  result.warmupTicks = configuration.warmupTicks;
  result.measuredTicks = configuration.measuredTicks;
  result.frequencyHertz = configuration.frequencyHertz;
  result.initializationMilliseconds = advanceOneTick();

  for (int index = 0; index < configuration.warmupTicks; ++index) {
    advanceOneTick();
  }

  std::vector<double> tickDurations;
  std::vector<double> modelStepDurations;
  tickDurations.reserve(configuration.measuredTicks);
  modelStepDurations.reserve(
      static_cast<std::size_t>(configuration.measuredTicks * entityCount));
  for (int index = 0; index < configuration.measuredTicks; ++index) {
    tickDurations.push_back(advanceOneTick());
    if (backend == QStringLiteral("jsbsim")) {
      for (const Entity& entity : entities) {
        modelStepDurations.push_back(
            entity.dynamicsStepDurationMilliseconds);
      }
    }
  }

  result.tickStatistics =
      application::summarizeRuntimeDurations(tickDurations);
  result.modelStepStatistics =
      application::summarizeRuntimeDurations(modelStepDurations);
  for (const Entity& entity : entities) {
    const QString activeBackend = entity.activeDynamicsBackend.trimmed();
    const bool expectedBackend = backend == QStringLiteral("jsbsim")
        ? activeBackend == QStringLiteral("jsbsim")
        : activeBackend.startsWith(QStringLiteral("kinematic"));
    if (!expectedBackend) {
      ++result.fallbackCount;
    }
  }

  FlightDynamicsEngine::clearDynamicsModels();
  return result;
}

QJsonObject statisticsToJson(
    const application::RuntimeBenchmarkStatistics& statistics) {
  return {
      {QStringLiteral("samples"), static_cast<qint64>(statistics.sampleCount)},
      {QStringLiteral("minimumMilliseconds"), statistics.minimumMilliseconds},
      {QStringLiteral("meanMilliseconds"), statistics.meanMilliseconds},
      {QStringLiteral("p50Milliseconds"), statistics.p50Milliseconds},
      {QStringLiteral("p95Milliseconds"), statistics.p95Milliseconds},
      {QStringLiteral("p99Milliseconds"), statistics.p99Milliseconds},
      {QStringLiteral("maximumMilliseconds"), statistics.maximumMilliseconds},
  };
}

QJsonObject resultToJson(const BenchmarkCaseResult& result) {
  const double meanTick = result.tickStatistics.meanMilliseconds;
  const double entityStepsPerSecond = meanTick > 0.0
      ? static_cast<double>(result.entityCount) * 1000.0 / meanTick
      : 0.0;
  const double realtimeFactor = meanTick > 0.0
      ? (1000.0 / result.frequencyHertz) / meanTick
      : 0.0;
  return {
      {QStringLiteral("backend"), result.backend},
      {QStringLiteral("model"), result.model},
      {QStringLiteral("entityCount"), result.entityCount},
      {QStringLiteral("warmupTicks"), result.warmupTicks},
      {QStringLiteral("measuredTicks"), result.measuredTicks},
      {QStringLiteral("frequencyHertz"), result.frequencyHertz},
      {QStringLiteral("initializationMilliseconds"), result.initializationMilliseconds},
      {QStringLiteral("initializationMillisecondsPerEntity"),
       result.initializationMilliseconds / result.entityCount},
      {QStringLiteral("tick"), statisticsToJson(result.tickStatistics)},
      {QStringLiteral("modelStep"), statisticsToJson(result.modelStepStatistics)},
      {QStringLiteral("meanMillisecondsPerEntity"), meanTick / result.entityCount},
      {QStringLiteral("entityStepsPerSecond"), entityStepsPerSecond},
      {QStringLiteral("realtimeFactor"), realtimeFactor},
      {QStringLiteral("sustainableAt30HzP95"),
       result.tickStatistics.p95Milliseconds <= 1000.0 / 30.0},
      {QStringLiteral("sustainableAt60HzP95"),
       result.tickStatistics.p95Milliseconds <= 1000.0 / 60.0},
      {QStringLiteral("fallbackCount"), result.fallbackCount},
  };
}

void printResult(QTextStream& output, const BenchmarkCaseResult& result) {
  const double meanTick = result.tickStatistics.meanMilliseconds;
  const double entityStepsPerSecond = meanTick > 0.0
      ? static_cast<double>(result.entityCount) * 1000.0 / meanTick
      : 0.0;
  const double realtimeFactor = meanTick > 0.0
      ? (1000.0 / result.frequencyHertz) / meanTick
      : 0.0;
  output << result.backend.leftJustified(10) << ' '
         << QString::number(result.entityCount).rightJustified(8) << ' '
         << QString::number(result.initializationMilliseconds, 'f', 2).rightJustified(10) << ' '
         << QString::number(meanTick, 'f', 3).rightJustified(10) << ' '
         << QString::number(result.tickStatistics.p95Milliseconds, 'f', 3).rightJustified(10) << ' '
         << QString::number(result.tickStatistics.p99Milliseconds, 'f', 3).rightJustified(10) << ' '
         << QString::number(meanTick / result.entityCount, 'f', 4).rightJustified(11) << ' '
         << QString::number(entityStepsPerSecond, 'f', 0).rightJustified(11) << ' '
         << QString::number(realtimeFactor, 'f', 2).rightJustified(8) << ' '
         << (result.tickStatistics.p95Milliseconds <= 1000.0 / 60.0 ? "yes" : "no ") << ' '
         << result.fallbackCount << '\n';
  output.flush();
}

bool writeJsonReport(
    const BenchmarkConfiguration& configuration,
    const QVector<BenchmarkCaseResult>& results,
    QString* error) {
  if (configuration.outputPath.trimmed().isEmpty()) {
    return true;
  }

  QJsonArray cases;
  for (const BenchmarkCaseResult& result : results) {
    cases.push_back(resultToJson(result));
  }
  QJsonArray entityCounts;
  for (const int count : configuration.entityCounts) {
    entityCounts.push_back(count);
  }
  QJsonArray backends;
  for (const QString& backend : configuration.backends) {
    backends.push_back(backend);
  }
  const QJsonObject report{
      {QStringLiteral("schemaVersion"), 1},
      {QStringLiteral("metadata"), QJsonObject{
          {QStringLiteral("generatedAtUtc"),
           QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)},
          {QStringLiteral("buildType"),
           QStringLiteral(QTTEST_BENCHMARK_BUILD_TYPE)},
          {QStringLiteral("operatingSystem"), QSysInfo::prettyProductName()},
          {QStringLiteral("kernelType"), QSysInfo::kernelType()},
          {QStringLiteral("kernelVersion"), QSysInfo::kernelVersion()},
          {QStringLiteral("cpuArchitecture"),
           QSysInfo::currentCpuArchitecture()},
          {QStringLiteral("qtVersion"), QString::fromLatin1(qVersion())},
          {QStringLiteral("clock"),
           QStringLiteral("std::chrono::steady_clock")},
      }},
      {QStringLiteral("configuration"), QJsonObject{
          {QStringLiteral("entityCounts"), entityCounts},
          {QStringLiteral("backends"), backends},
          {QStringLiteral("jsbsimModel"), configuration.jsbsimModel},
          {QStringLiteral("warmupTicks"), configuration.warmupTicks},
          {QStringLiteral("measuredTicks"), configuration.measuredTicks},
          {QStringLiteral("frequencyHertz"), configuration.frequencyHertz},
          {QStringLiteral("wallClockBudgetEnforced"), false},
      }},
      {QStringLiteral("cases"), cases},
  };

  QFile file(configuration.outputPath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    *error = QStringLiteral("Cannot write report: %1").arg(file.errorString());
    return false;
  }
  file.write(QJsonDocument(report).toJson(QJsonDocument::Indented));
  return true;
}

} // namespace

int main(int argc, char* argv[]) {
  QCoreApplication application(argc, argv);
  QCoreApplication::setApplicationName(QStringLiteral("qttest-jsbsim-benchmark"));
  qputenv("JSBSIM_DEBUG", QByteArrayLiteral("0"));

  QCommandLineParser parser;
  parser.setApplicationDescription(
      QStringLiteral("Headless FlightDynamicsEngine scaling benchmark"));
  parser.addHelpOption();
  parser.addOption({
      {QStringLiteral("e"), QStringLiteral("entities")},
      QStringLiteral("Comma-separated entity counts"),
      QStringLiteral("counts"),
      QStringLiteral("1,5,10,25")});
  parser.addOption({
      {QStringLiteral("b"), QStringLiteral("backend")},
      QStringLiteral("kinematic, jsbsim, or both"),
      QStringLiteral("backend"),
      QStringLiteral("both")});
  parser.addOption({
      QStringLiteral("model"),
      QStringLiteral("JSBSim aircraft model"),
      QStringLiteral("model"),
      QStringLiteral("f16")});
  parser.addOption({
      QStringLiteral("warmup"),
      QStringLiteral("Warm-up ticks after model initialization"),
      QStringLiteral("ticks"),
      QStringLiteral("60")});
  parser.addOption({
      QStringLiteral("ticks"),
      QStringLiteral("Measured ticks per case"),
      QStringLiteral("ticks"),
      QStringLiteral("300")});
  parser.addOption({
      QStringLiteral("hz"),
      QStringLiteral("Simulation frequency"),
      QStringLiteral("hertz"),
      QStringLiteral("60")});
  parser.addOption({
      {QStringLiteral("o"), QStringLiteral("output")},
      QStringLiteral("Optional JSON report path"),
      QStringLiteral("path")});
  parser.process(application);

  BenchmarkConfiguration configuration;
  QString error;
  configuration.entityCounts =
      parseEntityCounts(parser.value(QStringLiteral("entities")), &error);
  configuration.backends =
      parseBackends(parser.value(QStringLiteral("backend")), &error);
  configuration.jsbsimModel = parser.value(QStringLiteral("model")).trimmed();
  configuration.outputPath = parser.value(QStringLiteral("output")).trimmed();
  if (!parsePositiveInteger(
          parser.value(QStringLiteral("warmup")),
          &configuration.warmupTicks) ||
      !parsePositiveInteger(
          parser.value(QStringLiteral("ticks")),
          &configuration.measuredTicks)) {
    error = QStringLiteral("Warm-up and measured ticks must be positive integers");
  }
  bool frequencyOk = false;
  configuration.frequencyHertz =
      parser.value(QStringLiteral("hz")).toDouble(&frequencyOk);
  if (!frequencyOk || !std::isfinite(configuration.frequencyHertz) ||
      configuration.frequencyHertz <= 0.0) {
    error = QStringLiteral("Frequency must be a positive number");
  }
  if (configuration.jsbsimModel.isEmpty()) {
    error = QStringLiteral("JSBSim model cannot be empty");
  }
  if (!error.isEmpty()) {
    QTextStream(stderr) << error << '\n';
    return 1;
  }

  QTextStream output(stdout);
  output << "backend   entities    init_ms    mean_ms     p95_ms     p99_ms  ms/entity  entity/s rt_factor 60Hz fallback\n";

  QVector<BenchmarkCaseResult> results;
  bool valid = true;
  for (const QString& backend : configuration.backends) {
    for (const int entityCount : configuration.entityCounts) {
      const BenchmarkCaseResult result =
          runBenchmarkCase(configuration, backend, entityCount);
      printResult(output, result);
      valid = valid && result.fallbackCount == 0;
      results.push_back(result);
    }
  }

  if (!writeJsonReport(configuration, results, &error)) {
    QTextStream(stderr) << error << '\n';
    return 1;
  }
  return valid ? 0 : 2;
}
