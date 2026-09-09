#pragma once

#include "infrastructure/interoperability/dis/DisTypes.h"
#include <QByteArray>

namespace tactical::dis {
class DisSimulationControlCodec {
public:
  static QByteArray encodeAcknowledgement(const Acknowledgement& message, QString* error = nullptr);
  static bool decodeAcknowledgement(const QByteArray& data, Acknowledgement& message, QString* error = nullptr);
  static QByteArray encode(const SimulationControlMessage& message, QString* error = nullptr);
  static bool decode(const QByteArray& data, SimulationControlMessage& message, QString* error = nullptr);
};
}
