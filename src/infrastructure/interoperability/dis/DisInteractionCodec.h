#pragma once
#include "infrastructure/interoperability/dis/DisTypes.h"
#include <QByteArray>

namespace tactical::dis {
class DisInteractionCodec {
public:
  static QByteArray encode(const EntityManagementRequest& value, QString* error = nullptr);
  static QByteArray encode(const IffState& value, QString* error = nullptr);
  static QByteArray encode(const CollisionEvent& value, QString* error = nullptr);
  static bool decode(const QByteArray& data, EntityManagementRequest& value, QString* error = nullptr);
  static bool decode(const QByteArray& data, IffState& value, QString* error = nullptr);
  static bool decode(const QByteArray& data, CollisionEvent& value, QString* error = nullptr);
};
}
