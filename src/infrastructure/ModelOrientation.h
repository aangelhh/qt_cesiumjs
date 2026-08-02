#pragma once

#include <QString>

namespace ModelOrientation {

QString inferCesiumAxes(const QString& modelUri);
QString resolveCesiumAxes(
    const QString& configuredAxes,
    const QString& modelUri);

} // namespace ModelOrientation
