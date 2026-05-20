#include "presentation/MapBridgeScripts.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace presentation {
namespace MapBridgeScripts {

static QString mapToJson(const QVariantMap& map) {
  return QString::fromUtf8(
      QJsonDocument(QJsonObject::fromVariantMap(map)).toJson(QJsonDocument::Compact));
}

static QString quoted(bool value) {
  return value ? QStringLiteral("true") : QStringLiteral("false");
}

static QString nameToJsonString(const QString& name) {
  const QString arr = QString::fromUtf8(
      QJsonDocument(QJsonArray{name}).toJson(QJsonDocument::Compact));
  // strip surrounding [ ] → just the quoted name
  return arr.mid(1).chopped(1);
}

QString buildAddOrUpdateScript(const QVariantMap& summary, bool focus) {
  if (summary.isEmpty()) {
    return {};
  }

  const QString json = mapToJson(summary);
  const QString type = summary.value(QStringLiteral("type")).toString();
  const bool isTactical = type == QStringLiteral("Waypoint") ||
                          type == QStringLiteral("Route") ||
                          type == QStringLiteral("Area");
  if (isTactical) {
    return QStringLiteral(
               "window.addOrUpdateQtGraphic && window.addOrUpdateQtGraphic(%1, %2);")
        .arg(json, quoted(focus));
  }
  return QStringLiteral(
             "window.addOrUpdateQtTrack && window.addOrUpdateQtTrack(%1, %2);")
      .arg(json, quoted(focus));
}

QString buildRemoveScript(const QString& trackName) {
  if (trackName.trimmed().isEmpty()) {
    return {};
  }
  const QString nameJson = nameToJsonString(trackName);
  return QStringLiteral(
             "(window.removeQtTrack && window.removeQtTrack(%1));"
             "(window.removeQtGraphic && window.removeQtGraphic(%1));")
      .arg(nameJson);
}

QString buildAddOrUpdateDraftScript(const QVariantMap& summary) {
  if (summary.isEmpty()) {
    return {};
  }
  const QString json = mapToJson(summary);
  return QStringLiteral(
             "window.addOrUpdateQtDraftGraphic && window.addOrUpdateQtDraftGraphic(%1);")
      .arg(json);
}

QString buildRemoveDraftScript(const QString& name) {
  if (name.trimmed().isEmpty()) {
    return {};
  }
  const QString nameJson = nameToJsonString(name);
  return QStringLiteral(
             "window.removeQtDraftGraphic && window.removeQtDraftGraphic(%1);")
      .arg(nameJson);
}

} // namespace MapBridgeScripts
} // namespace presentation
