#pragma once

#include "infrastructure/interoperability/dis/DisTypes.h"

#include <QByteArray>

namespace tactical::dis {

class DisEntityStateCodec {
public:
  static QByteArray encode(const EntityState& state, QString* error = nullptr);
  static bool decode(
      const QByteArray& datagram,
      EntityState& state,
      QString* error = nullptr);
};

} // namespace tactical::dis
