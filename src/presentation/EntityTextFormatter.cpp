#include "presentation/EntityTextFormatter.h"

namespace {
QString forceIdentifierLabel(int forceIdentifier) {
  switch (forceIdentifier) {
    case 1:
      return QStringLiteral("Friendly");
    case 2:
      return QStringLiteral("Opposing");
    case 3:
      return QStringLiteral("Neutral");
    default:
      return QStringLiteral("Unknown");
  }
}
} // namespace

QString EntityTextFormatter::listLabel(const Entity& entity) {
  QString label = QStringLiteral("%1 [%2]").arg(entity.name, entity.type);
  if (!entity.callsign.isEmpty()) {
    label += QStringLiteral(" - %1").arg(entity.callsign);
  }
  label += QStringLiteral(" (%1)").arg(forceIdentifierLabel(entity.forceIdentifier));
  label += QStringLiteral(" @ lat %1 lon %2 alt %3 m")
               .arg(entity.latitude, 0, 'f', 4)
               .arg(entity.longitude, 0, 'f', 4)
               .arg(entity.altitude);
  return label;
}

QString EntityTextFormatter::statusMessage(const Entity& entity) {
  return QStringLiteral("Entidad anadida: %1").arg(entity.name);
}
