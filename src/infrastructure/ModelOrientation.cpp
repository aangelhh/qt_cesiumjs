#include "infrastructure/ModelOrientation.h"

#include <QHash>

namespace {

QString modelFileName(QString modelUri) {
  modelUri = modelUri.trimmed().replace('\\', '/');
  const int suffixIndex = modelUri.indexOf(QChar('?'));
  if (suffixIndex >= 0) {
    modelUri.truncate(suffixIndex);
  }
  const int fragmentIndex = modelUri.indexOf(QChar('#'));
  if (fragmentIndex >= 0) {
    modelUri.truncate(fragmentIndex);
  }
  return modelUri.mid(modelUri.lastIndexOf(QChar('/')) + 1).toLower();
}

const QHash<QString, QString>& axesByModelFileName() {
  static const QHash<QString, QString> axes = {
      // Authored with the longitudinal axis on +X and vertical axis on +Y.
      {QStringLiteral("b-2_spirit.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("dassault_mirage_2000.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("dassault_mirage_iii.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("dassault_rafale.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("dassault_rafale_m.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("eurofighter_typhoon.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("f-16_fighting_falcon.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("f-22_raptor.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("f-35_lightning_ii.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("fa-18ef_super_hornet.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("fa-18f_super_hornet.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("mig-29_fighter.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("boeing_ah-64d_apache.glb"), QStringLiteral("x-forward-y-up")},
      {QStringLiteral("eurocopter_ec665_tigre.glb"), QStringLiteral("x-forward-y-up")},

      // Standard glTF-style +Z longitudinal axis and +Y vertical axis.
      {QStringLiteral("boeing_b-52_stratofortress.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("rockwell_b-1b_lancer.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("f-2000a_italian.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("a-10_thunderbolt_ii.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("a-10a_thunderbolt_ii.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("a-10c.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("dassault_mirage_f1.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("dassault_mirage_f1c-200.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("dassault_mirage_iiie.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("dassault_rafale_c_f3.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("f-16a.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("f-35b_lightning_ii.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("mig-29_figther_smt.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("mirage_2000-5.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("mirage_2000c-s5.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("mirage_2000d.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("boeing_ch-47_chinook.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("airbus_a400m.glb"), QStringLiteral("z-forward-y-up")},
      {QStringLiteral("c-130j_super_hercules.glb"), QStringLiteral("z-forward-y-up")},
  };
  return axes;
}

} // namespace

namespace ModelOrientation {

QString inferCesiumAxes(const QString& modelUri) {
  return axesByModelFileName().value(modelFileName(modelUri));
}

QString resolveCesiumAxes(
    const QString& configuredAxes,
    const QString& modelUri) {
  const QString normalizedAxes = configuredAxes.trimmed().toLower();
  return normalizedAxes.isEmpty() ? inferCesiumAxes(modelUri) : normalizedAxes;
}

} // namespace ModelOrientation
