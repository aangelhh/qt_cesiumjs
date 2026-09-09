#pragma once

#include "infrastructure/interoperability/dis/DisTypes.h"

#include <QByteArray>

namespace tactical::dis {

class DisWarfareCodec {
public:
  static QByteArray encodeFire(
      const WarfareEvent& event,
      QString* error = nullptr);
  static QByteArray encodeDetonation(
      const WarfareEvent& event,
      QString* error = nullptr);
  static bool decode(
      const QByteArray& datagram,
      WarfareEvent& event,
      QString* error = nullptr);
};

} // namespace tactical::dis
