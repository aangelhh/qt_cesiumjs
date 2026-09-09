#pragma once
#include "infrastructure/interoperability/dis/DisTypes.h"
#include <QByteArray>

namespace tactical::dis {
class DisEmissionCodec {
public:
  static QByteArray encode(const RadarEmission& value, QString* error = nullptr);
  static bool decode(const QByteArray& data, RadarEmission& value, QString* error = nullptr);
};
}
