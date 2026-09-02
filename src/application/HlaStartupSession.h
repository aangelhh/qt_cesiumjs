#pragma once

#include "application/StartupConfiguration.h"
#include "infrastructure/interoperability/hla/HlaRuntime.h"
#include "infrastructure/interoperability/hla/HlaEntityPublisher.h"
#include "infrastructure/interoperability/hla/HlaWarfarePublisher.h"
#include "domain/Entity.h"
#include "domain/Munition.h"

#include <memory>
#include <QVector>

namespace application {

class HlaStartupSession {
public:
  tactical::hla::Result start(const HlaStartupConfiguration& configuration);
  tactical::hla::Result poll(double maximumSeconds);
  tactical::hla::Result publishEntities(const QVector<Entity>& entities);
  tactical::hla::Result publishMunitions(
      const QVector<ActiveMunition>& activeMunitions);
  tactical::hla::Result stop();

  bool isActive() const;
  QString backendId() const;

private:
  std::unique_ptr<tactical::hla::HlaRuntime> _runtime;
  std::unique_ptr<tactical::hla::HlaEntityPublisher> _entityPublisher;
  std::unique_ptr<tactical::hla::HlaWarfarePublisher> _warfarePublisher;
  QString _backendId;
};

} // namespace application
