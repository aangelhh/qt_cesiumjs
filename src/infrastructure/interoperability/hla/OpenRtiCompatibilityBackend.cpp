#include "infrastructure/interoperability/hla/OpenRtiCompatibilityBackend.h"

#include <QDomDocument>
#include <QDir>
#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>

#include <algorithm>
#include <utility>

namespace tactical::hla {
namespace {

QString directName(const QDomElement& element) {
  for (QDomNode node = element.firstChild(); !node.isNull();
       node = node.nextSibling()) {
    const QDomElement child = node.toElement();
    if (!child.isNull() && child.tagName() == QStringLiteral("name")) {
      return child.text();
    }
  }
  return {};
}

QDomElement matchingChild(
    const QDomElement& target,
    const QDomElement& sourceChild) {
  const QString sourceName = directName(sourceChild);
  for (QDomNode node = target.firstChild(); !node.isNull();
       node = node.nextSibling()) {
    const QDomElement targetChild = node.toElement();
    if (targetChild.isNull() ||
        targetChild.tagName() != sourceChild.tagName()) {
      continue;
    }
    if (directName(targetChild) == sourceName) {
      return targetChild;
    }
  }
  return {};
}

void mergeElement(
    QDomElement target,
    const QDomElement& source,
    QDomDocument& targetDocument) {
  const QDomNamedNodeMap attributes = source.attributes();
  for (int index = 0; index < attributes.count(); ++index) {
    const QDomAttr attribute = attributes.item(index).toAttr();
    if (!target.hasAttribute(attribute.name())) {
      target.setAttribute(attribute.name(), attribute.value());
    }
  }

  bool sourceHasElementChildren = false;
  for (QDomNode node = source.firstChild(); !node.isNull();
       node = node.nextSibling()) {
    const QDomElement sourceChild = node.toElement();
    if (sourceChild.isNull()) {
      continue;
    }
    sourceHasElementChildren = true;
    QDomElement targetChild = matchingChild(target, sourceChild);
    if (targetChild.isNull()) {
      target.appendChild(targetDocument.importNode(sourceChild, true));
      continue;
    }
    mergeElement(targetChild, sourceChild, targetDocument);
  }

  if (!sourceHasElementChildren && target.text().trimmed().isEmpty() &&
      !source.text().trimmed().isEmpty()) {
    target.appendChild(targetDocument.createTextNode(source.text()));
  }
}

bool loadDocument(
    const QString& path,
    QDomDocument& document,
    QString& error) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    error = QStringLiteral("Unable to open FOM module: %1").arg(path);
    return false;
  }
  const QDomDocument::ParseResult parseResult = document.setContent(&file);
  if (!parseResult) {
    error = QStringLiteral("Invalid FOM module %1 at %2:%3: %4")
                .arg(path)
                .arg(parseResult.errorLine)
                .arg(parseResult.errorColumn)
                .arg(parseResult.errorMessage);
    return false;
  }
  return true;
}

bool mergeFomModules(
    const std::vector<std::string>& modules,
    QTemporaryFile& output,
    std::string& error) {
  QDomDocument mergedDocument;
  QString qtError;
  if (!loadDocument(QString::fromStdString(modules.front()), mergedDocument, qtError)) {
    error = qtError.toStdString();
    return false;
  }

  QDomElement mergedRoot = mergedDocument.documentElement();
  for (std::size_t index = 1; index < modules.size(); ++index) {
    QDomDocument additionalDocument;
    if (!loadDocument(
            QString::fromStdString(modules[index]),
            additionalDocument,
            qtError)) {
      error = qtError.toStdString();
      return false;
    }
    if (additionalDocument.documentElement().tagName() != mergedRoot.tagName()) {
      error = "FOM modules do not share the same root element";
      return false;
    }
    mergeElement(
        mergedRoot,
        additionalDocument.documentElement(),
        mergedDocument);
  }

  output.setFileTemplate(
      QDir::tempPath() + QStringLiteral("/qttest-netn-fom-XXXXXX.xml"));
  if (!output.open()) {
    error = "Unable to create the temporary merged FOM module";
    return false;
  }
  QTextStream stream(&output);
  mergedDocument.save(stream, 2);
  stream.flush();
  output.flush();
  return true;
}

std::string normalizeLocalSettings(const std::string& value) {
  static const std::string legacyPrefix = "crcAddress=";
  if (value.compare(0, legacyPrefix.size(), legacyPrefix) == 0) {
    return "rti://" + value.substr(legacyPrefix.size());
  }
  return value;
}

} // namespace

OpenRtiCompatibilityBackend::OpenRtiCompatibilityBackend(
    std::unique_ptr<IHlaBackend> backend)
    : _backend(std::move(backend)) {}

std::string OpenRtiCompatibilityBackend::id() const {
  return _backend->id();
}

std::string OpenRtiCompatibilityBackend::version() const {
  return _backend->version();
}

std::vector<std::string> OpenRtiCompatibilityBackend::capabilities() const {
  std::vector<std::string> values = _backend->capabilities();
  values.push_back("legacy-netn-fom-merge");
  values.push_back("legacy-crc-address");
  return values;
}

Result OpenRtiCompatibilityBackend::connect(
    const std::string& localSettingsDesignator) {
  _compatibilityError.clear();
  return _backend->connect(normalizeLocalSettings(localSettingsDesignator));
}

Result OpenRtiCompatibilityBackend::createFederation(
    const std::string& federationName,
    const std::vector<std::string>& fomModules) {
  _compatibilityError.clear();
  if (fomModules.size() < 2) {
    return _backend->createFederation(federationName, fomModules);
  }

  QTemporaryFile mergedFom;
  if (!mergeFomModules(fomModules, mergedFom, _compatibilityError)) {
    return Result::failure(_compatibilityError);
  }
  return _backend->createFederation(
      federationName,
      {mergedFom.fileName().toStdString()});
}

Result OpenRtiCompatibilityBackend::joinFederation(
    const std::string& federateName,
    const std::string& federateType,
    const std::string& federationName,
    const std::vector<std::string>& additionalFomModules) {
  _compatibilityError.clear();
  if (additionalFomModules.size() < 2) {
    return _backend->joinFederation(
        federateName,
        federateType,
        federationName,
        additionalFomModules);
  }

  QTemporaryFile mergedFom;
  if (!mergeFomModules(
          additionalFomModules,
          mergedFom,
          _compatibilityError)) {
    return Result::failure(_compatibilityError);
  }
  return _backend->joinFederation(
      federateName,
      federateType,
      federationName,
      {mergedFom.fileName().toStdString()});
}

Result OpenRtiCompatibilityBackend::publishObjectClass(
    const std::string& objectClassName,
    const std::vector<std::string>& attributeNames) {
  return _backend->publishObjectClass(objectClassName, attributeNames);
}

Result OpenRtiCompatibilityBackend::subscribeObjectClass(
    const std::string& objectClassName,
    const std::vector<std::string>& attributeNames) {
  return _backend->subscribeObjectClass(objectClassName, attributeNames);
}

Result OpenRtiCompatibilityBackend::registerObjectInstance(
    const std::string& objectClassName,
    const std::string& instanceName,
    ObjectInstanceId& instanceId) {
  return _backend->registerObjectInstance(
      objectClassName, instanceName, instanceId);
}

Result OpenRtiCompatibilityBackend::updateObjectAttributes(
    ObjectInstanceId instanceId,
    const std::vector<NamedValue>& attributes,
    const ByteBuffer& tag) {
  return _backend->updateObjectAttributes(instanceId, attributes, tag);
}

Result OpenRtiCompatibilityBackend::deleteObjectInstance(
    ObjectInstanceId instanceId,
    const ByteBuffer& tag) {
  return _backend->deleteObjectInstance(instanceId, tag);
}

Result OpenRtiCompatibilityBackend::publishInteractionClass(
    const std::string& interactionClassName) {
  return _backend->publishInteractionClass(interactionClassName);
}

Result OpenRtiCompatibilityBackend::subscribeInteractionClass(
    const std::string& interactionClassName,
    const std::vector<std::string>& parameterNames) {
  return _backend->subscribeInteractionClass(
      interactionClassName, parameterNames);
}

Result OpenRtiCompatibilityBackend::sendInteraction(
    const std::string& interactionClassName,
    const std::vector<NamedValue>& parameters,
    const ByteBuffer& tag) {
  return _backend->sendInteraction(interactionClassName, parameters, tag);
}

Result OpenRtiCompatibilityBackend::registerSynchronizationPoint(
    const std::string& label,
    const ByteBuffer& tag) {
  return _backend->registerSynchronizationPoint(label, tag);
}

Result OpenRtiCompatibilityBackend::achieveSynchronizationPoint(
    const std::string& label) {
  return _backend->achieveSynchronizationPoint(label);
}

Result OpenRtiCompatibilityBackend::poll(double maximumSeconds) {
  return _backend->poll(maximumSeconds);
}

void OpenRtiCompatibilityBackend::setEventSink(IHlaEventSink* eventSink) {
  _backend->setEventSink(eventSink);
}

Result OpenRtiCompatibilityBackend::resign() {
  return _backend->resign();
}

Result OpenRtiCompatibilityBackend::disconnect() {
  return _backend->disconnect();
}

BackendState OpenRtiCompatibilityBackend::state() const {
  return _backend->state();
}

std::string OpenRtiCompatibilityBackend::lastError() const {
  return _compatibilityError.empty() ? _backend->lastError()
                                     : _compatibilityError;
}

} // namespace tactical::hla
