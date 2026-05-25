#include "TrackIconProvider.h"
#include <QFileInfo>
#include <QPainter>
#include <QPixmap>

namespace presentation {

QColor forceColorFromLabel(const QString& team) {
  const QString normalized = team.trimmed().toLower();
  if (normalized.contains(QStringLiteral("opposing"))) {
    return QColor(QStringLiteral("#ff3b30"));
  }
  if (normalized.contains(QStringLiteral("neutral"))) {
    return QColor(QStringLiteral("#35c759"));
  }
  if (normalized.contains(QStringLiteral("unknown"))) {
    return QColor(QStringLiteral("#ffd60a"));
  }
  return QColor(QStringLiteral("#55d3ff")); // Friendly (default)
}

QString categoryGlyph(const QString& category) {
  const QString normalized = category.trimmed().toLower();
  const QString compact = QString(normalized)
      .remove(QChar(' '))
      .remove(QChar('_'))
      .remove(QChar('-'));
  if (normalized == QStringLiteral("fighter"))         return QStringLiteral("F");
  if (normalized == QStringLiteral("bomber"))          return QStringLiteral("B");
  if (normalized == QStringLiteral("helicopter"))      return QStringLiteral("H");
  if (normalized == QStringLiteral("transport"))       return QStringLiteral("T");
  if (normalized == QStringLiteral("tank"))            return QStringLiteral("K");
  if (normalized == QStringLiteral("truck"))           return QStringLiteral("R");
  if (compact == QStringLiteral("armoredvehicle") ||
      compact == QStringLiteral("armouredvehicle"))    return QStringLiteral("V");
  if (normalized == QStringLiteral("radar"))           return QStringLiteral("D");
  if (compact == QStringLiteral("samlauncher"))        return QStringLiteral("A");
  if (normalized == QStringLiteral("other"))           return QStringLiteral("O");
  if (normalized == QStringLiteral("side"))            return QStringLiteral("S");
  return QStringLiteral("E");
}

QIcon makeTacticalGraphicIcon(const QString& graphicType) {
  QPixmap pixmap(18, 18);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, true);

  const QString normalized = graphicType.trimmed().toLower();
  if (normalized == QStringLiteral("route")) {
    QPen pen(QColor(QStringLiteral("#ff6c52")));
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    painter.drawLine(3, 15, 15, 3);
  } else if (normalized == QStringLiteral("waypoint")) {
    QPen pen(QColor(QStringLiteral("#b6f4b2")));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(QColor(QStringLiteral("#dff9da")));
    painter.drawRect(5, 3, 8, 10);
  } else if (normalized == QStringLiteral("engagement area")) {
    QPen pen(QColor(QStringLiteral("#ff5656")));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(4, 4, 10, 10);
  } else {
    QPen pen(QColor(QStringLiteral("#ffd85e")));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawEllipse(4, 4, 10, 10);
  }

  return QIcon(pixmap);
}

QIcon makeTrackIcon(const QString& team, const QString& category, bool isGroup) {
  QPixmap pixmap(18, 18);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, true);

  const QColor accent = forceColorFromLabel(team);
  const QRect outerRect(1, 1, 16, 16);
  const QRect innerRect(3, 3, 12, 12);

  QPen pen(accent);
  pen.setWidth(isGroup ? 2 : 1);
  painter.setPen(pen);
  painter.setBrush(QColor(7, 17, 29, isGroup ? 220 : 245));
  painter.drawRoundedRect(outerRect, 4, 4);

  QFont font = painter.font();
  font.setBold(true);
  font.setPixelSize(isGroup ? 10 : 9);
  painter.setFont(font);
  painter.setPen(accent);
  painter.drawText(innerRect, Qt::AlignCenter, categoryGlyph(category));

  return QIcon(pixmap);
}

QIcon makeTaskQuickFallbackIcon(const QString& glyph, const QColor& accent) {
  constexpr int kIconPixels = 18;
  QPixmap pixmap(kIconPixels, kIconPixels);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(22, 30, 40, 220));
  painter.drawRoundedRect(pixmap.rect().adjusted(1, 1, -1, -1), 4, 4);

  QFont font = painter.font();
  font.setBold(true);
  font.setPixelSize(10);
  painter.setFont(font);
  painter.setPen(accent);
  painter.drawText(pixmap.rect(), Qt::AlignCenter, glyph.left(2).toUpper());
  return QIcon(pixmap);
}

QIcon loadTaskQuickBarIcon(
    const QString& iconFilePath,
    const QString& fallbackGlyph,
    const QColor& accent) {
  if (QFileInfo::exists(iconFilePath)) {
    const QIcon icon(iconFilePath);
    if (!icon.isNull()) {
      return icon;
    }
  }
  return makeTaskQuickFallbackIcon(fallbackGlyph, accent);
}

} // namespace presentation
