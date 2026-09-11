#include "infrastructure/sensors/SensorModelPluginApi.h"
#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>
#include <cmath>
#include <cstring>

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    if (argc != 2) return 2;
    QLibrary library(QString::fromLocal8Bit(argv[1]));
    library.setLoadHints(QLibrary::ResolveAllSymbolsHint);
    if (!library.load()) { qCritical() << library.errorString(); return 3; }
    auto abi = reinterpret_cast<QttestSensorPluginAbiVersionFn>(library.resolve("qttest_sensor_model_abi_version"));
    auto id = reinterpret_cast<QttestSensorPluginModelIdFn>(library.resolve("qttest_sensor_model_id"));
    auto evaluate = reinterpret_cast<QttestSensorPluginEvaluateFn>(library.resolve("qttest_sensor_model_evaluate"));
    if (!abi || !id || !evaluate || abi() != QTTEST_SENSOR_MODEL_PLUGIN_ABI_VERSION || !id() || std::strcmp(id(), "mixr")) return 4;
    QttestSensorEvaluationInputV1 input{};
    input.structSize = sizeof(input);
    input.rangeMeters = 1000;
    input.configuredProbability = 0.8;
    input.targetSignature = 1;
    input.sensorMaxRangeMeters = 100000;
    QttestSensorEvaluationOutputV1 output{};
    output.structSize = sizeof(output);
    if (evaluate(&input, &output) || output.status || !std::isfinite(output.probability) || output.probability < 0 || output.probability > 1) return 5;
    qInfo() << "MIXR: AVAILABLE (load, ABI, identity and evaluation verified)";
    return 0;
}
