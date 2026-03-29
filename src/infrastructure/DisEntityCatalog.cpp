#include "infrastructure/DisEntityCatalog.h"

#include <QDir>
#include <QFile>
#include <algorithm>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>

namespace {

QString projectRoot() {
#ifdef QTTEST_SOURCE_DIR
  return QString::fromUtf8(QTTEST_SOURCE_DIR);
#else
  return QDir::currentPath();
#endif
}

int readIntAttribute(const QXmlStreamAttributes& attributes, const char* name) {
  bool ok = false;
  const int value = attributes.value(QLatin1String(name)).toInt(&ok);
  return ok ? value : 0;
}

QString readStringAttribute(const QXmlStreamAttributes& attributes, const char* name) {
  return attributes.value(QLatin1String(name)).toString().trimmed();
}

QString domainKey(int kind, int domainCode) {
  return QStringLiteral("%1:%2").arg(kind).arg(domainCode);
}

QString categoryKey(int kind, int domainCode, int categoryCode) {
  return QStringLiteral("%1:%2:%3").arg(kind).arg(domainCode).arg(categoryCode);
}

QString subcategoryKey(int kind, int domainCode, int country, int categoryCode, int subcategory) {
  return QStringLiteral("%1:%2:%3:%4:%5")
      .arg(kind)
      .arg(domainCode)
      .arg(country)
      .arg(categoryCode)
      .arg(subcategory);
}

QString specificKey(
    int kind,
    int domainCode,
    int country,
    int categoryCode,
    int subcategory,
    int specific) {
  return QStringLiteral("%1:%2:%3:%4:%5:%6")
      .arg(kind)
      .arg(domainCode)
      .arg(country)
      .arg(categoryCode)
      .arg(subcategory)
      .arg(specific);
}

bool parseCategoryKey(
    const QString& cname,
    int* kind,
    int* domainCode,
    int* categoryCode) {
  const QStringList parts = cname.split('.');
  if (parts.size() == 7 &&
      parts.at(0) == QStringLiteral("es") &&
      parts.at(1) == QStringLiteral("type") &&
      parts.at(2) == QStringLiteral("kind") &&
      parts.at(4) == QStringLiteral("domain") &&
      parts.at(6) == QStringLiteral("cat")) {
    bool okKind = false;
    bool okDomain = false;
    const int parsedKind = parts.at(3).toInt(&okKind);
    const int parsedDomain = parts.at(5).toInt(&okDomain);
    if (okKind && okDomain) {
      *kind = parsedKind;
      *domainCode = parsedDomain;
      *categoryCode = 0;
      return true;
    }
  }

  if (parts.size() == 5 &&
      parts.at(0) == QStringLiteral("es") &&
      parts.at(1) == QStringLiteral("type") &&
      parts.at(2) == QStringLiteral("kind") &&
      parts.at(4) == QStringLiteral("cat")) {
    bool okKind = false;
    const int parsedKind = parts.at(3).toInt(&okKind);
    if (okKind) {
      *kind = parsedKind;
      *domainCode = 0;
      *categoryCode = 0;
      return true;
    }
  }

  return false;
}

bool parseDomainKey(const QString& cname, int* kind) {
  const QStringList parts = cname.split('.');
  if (parts.size() == 5 &&
      parts.at(0) == QStringLiteral("es") &&
      parts.at(1) == QStringLiteral("type") &&
      parts.at(2) == QStringLiteral("kind") &&
      parts.at(4) == QStringLiteral("domain")) {
    bool okKind = false;
    const int parsedKind = parts.at(3).toInt(&okKind);
    if (okKind) {
      *kind = parsedKind;
      return true;
    }
  }
  return false;
}

void registerExactEntry(
    DisEntityCatalog& catalog,
    const DisEntityCatalog::Codes& codes,
    const DisEntityCatalog::ExactEntry& entry) {
  catalog._exactEntries.insert(DisEntityCatalog::buildCode(codes), entry);
}

void parseSpecificElement(
    QXmlStreamReader& xml,
    DisEntityCatalog& catalog,
    const DisEntityCatalog::Codes& baseCodes,
    const DisEntityCatalog::ExactEntry& baseEntry) {
  DisEntityCatalog::Codes specificCodes = baseCodes;
  specificCodes.specific = readIntAttribute(xml.attributes(), "id");

  DisEntityCatalog::ExactEntry specificEntry = baseEntry;
  specificEntry.specificDescription = readStringAttribute(xml.attributes(), "description");
  registerExactEntry(catalog, specificCodes, specificEntry);
  catalog._specificDescriptions.insert(
      specificKey(
          specificCodes.kind,
          specificCodes.domainCode,
          specificCodes.country,
          specificCodes.categoryCode,
          specificCodes.subcategory,
          specificCodes.specific),
      specificEntry.specificDescription);

  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("extra")) {
      DisEntityCatalog::Codes extraCodes = specificCodes;
      extraCodes.extra = readIntAttribute(xml.attributes(), "id");

      DisEntityCatalog::ExactEntry extraEntry = specificEntry;
      extraEntry.extraDescription = readStringAttribute(xml.attributes(), "description");
      registerExactEntry(catalog, extraCodes, extraEntry);
      catalog._extraDescriptions.insert(
          DisEntityCatalog::buildCode(extraCodes),
          extraEntry.extraDescription);
      xml.skipCurrentElement();
      continue;
    }
    xml.skipCurrentElement();
  }
}

void parseSubcategoryElement(
    QXmlStreamReader& xml,
    DisEntityCatalog& catalog,
    const DisEntityCatalog::Codes& baseCodes,
    const DisEntityCatalog::ExactEntry& baseEntry) {
  DisEntityCatalog::Codes subcategoryCodes = baseCodes;
  subcategoryCodes.subcategory = readIntAttribute(xml.attributes(), "id");

  DisEntityCatalog::ExactEntry subcategoryEntry = baseEntry;
  subcategoryEntry.subcategoryDescription = readStringAttribute(xml.attributes(), "description");
  registerExactEntry(catalog, subcategoryCodes, subcategoryEntry);
  catalog._subcategoryDescriptions.insert(
      subcategoryKey(
          subcategoryCodes.kind,
          subcategoryCodes.domainCode,
          subcategoryCodes.country,
          subcategoryCodes.categoryCode,
          subcategoryCodes.subcategory),
      subcategoryEntry.subcategoryDescription);

  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("specific")) {
      parseSpecificElement(xml, catalog, subcategoryCodes, subcategoryEntry);
      continue;
    }
    xml.skipCurrentElement();
  }
}

void parseCategoryElement(
    QXmlStreamReader& xml,
    DisEntityCatalog& catalog,
    const DisEntityCatalog::Codes& baseCodes) {
  DisEntityCatalog::Codes categoryCodes = baseCodes;
  categoryCodes.categoryCode = readIntAttribute(xml.attributes(), "id");

  DisEntityCatalog::ExactEntry categoryEntry;
  categoryEntry.categoryDescription = readStringAttribute(xml.attributes(), "description");
  registerExactEntry(catalog, categoryCodes, categoryEntry);

  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("subcategory")) {
      parseSubcategoryElement(xml, catalog, categoryCodes, categoryEntry);
      continue;
    }
    xml.skipCurrentElement();
  }
}

void parseEntityElement(QXmlStreamReader& xml, DisEntityCatalog& catalog) {
  DisEntityCatalog::Codes entityCodes;
  entityCodes.kind = readIntAttribute(xml.attributes(), "kind");
  entityCodes.domainCode = readIntAttribute(xml.attributes(), "domain");
  entityCodes.country = readIntAttribute(xml.attributes(), "country");

  registerExactEntry(catalog, entityCodes, {});

  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("category")) {
      parseCategoryElement(xml, catalog, entityCodes);
      continue;
    }
    xml.skipCurrentElement();
  }
}

void parseEnums(QXmlStreamReader& xml, DisEntityCatalog& catalog) {
  const QString cname = readStringAttribute(xml.attributes(), "cname");
  const bool isKind = cname == QStringLiteral("es.type.kind");
  const bool isCountry = cname == QStringLiteral("es.type.country");
  int domainKind = 0;
  int categoryKind = 0;
  int categoryDomain = 0;
  int unusedCategory = 0;
  const bool isDomain = parseDomainKey(cname, &domainKind);
  const bool isCategory = parseCategoryKey(cname, &categoryKind, &categoryDomain, &unusedCategory);

  while (xml.readNextStartElement()) {
    if (xml.name() != QLatin1String("enumrow")) {
      xml.skipCurrentElement();
      continue;
    }

    const int id = readIntAttribute(xml.attributes(), "id");
    const QString description = readStringAttribute(xml.attributes(), "description");
    if (isKind) {
      catalog._kindDescriptions.insert(id, description);
    } else if (isCountry) {
      catalog._countryDescriptions.insert(id, description);
    } else if (isDomain) {
      catalog._domainDescriptions.insert(domainKey(domainKind, id), description);
    } else if (isCategory) {
      catalog._categoryDescriptions.insert(categoryKey(categoryKind, categoryDomain, id), description);
    }
    xml.skipCurrentElement();
  }
}

} // namespace

bool DisEntityInfo::isValid() const {
  return !displayName().isEmpty() ||
      !kindDescription.isEmpty() ||
      !domainDescription.isEmpty() ||
      !countryDescription.isEmpty();
}

QString DisEntityInfo::displayName() const {
  if (!extraDescription.isEmpty()) {
    return extraDescription;
  }
  if (!specificDescription.isEmpty()) {
    return specificDescription;
  }
  if (!subcategoryDescription.isEmpty()) {
    return subcategoryDescription;
  }
  return categoryDescription;
}

QString DisEntityCatalog::defaultCatalogPath() {
  return QDir(projectRoot()).absoluteFilePath(QStringLiteral("Data/siso-std-010.xml"));
}

DisEntityCatalog DisEntityCatalog::loadDefault() {
  return loadFromFile(defaultCatalogPath());
}

DisEntityCatalog DisEntityCatalog::loadFromFile(const QString& path) {
  DisEntityCatalog catalog;

  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    catalog._errorString = QStringLiteral("Could not open DIS catalog: %1").arg(path);
    return catalog;
  }

  QXmlStreamReader xml(&file);
  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("ebv")) {
      while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("enum")) {
          parseEnums(xml, catalog);
          continue;
        }
        if (xml.name() == QLatin1String("cet")) {
          while (xml.readNextStartElement()) {
            if (xml.name() == QLatin1String("entity")) {
              parseEntityElement(xml, catalog);
              continue;
            }
            xml.skipCurrentElement();
          }
          continue;
        }
        xml.skipCurrentElement();
      }
      break;
    }
    xml.skipCurrentElement();
  }

  if (xml.hasError()) {
    catalog = DisEntityCatalog{};
    catalog._errorString = QStringLiteral("Failed to parse DIS catalog: %1").arg(xml.errorString());
  }

  return catalog;
}

bool DisEntityCatalog::isLoaded() const {
  return _errorString.isEmpty() &&
      (!_kindDescriptions.isEmpty() || !_exactEntries.isEmpty());
}

QString DisEntityCatalog::errorString() const {
  return _errorString;
}

DisEntityInfo DisEntityCatalog::lookup(const Entity& entity) const {
  return this->lookup(
      entity.entityKind,
      entity.entityDomain,
      entity.entityCountry,
      entity.entityCategory,
      entity.entitySubcategory,
      entity.entitySpecific,
      entity.entityExtra);
}

DisEntityInfo DisEntityCatalog::lookup(
    int kind,
    int domainCode,
    int country,
    int categoryCode,
    int subcategory,
    int specific,
    int extra) const {
  const QString entityTypeCode = Entity::buildEntityTypeCode(
      kind,
      domainCode,
      country,
      categoryCode,
      subcategory,
      specific,
      extra);

  DisEntityInfo info = this->lookup(entityTypeCode);
  if (info.entityTypeCode.isEmpty()) {
    info.entityTypeCode = entityTypeCode;
  }
  return info;
}

DisEntityInfo DisEntityCatalog::lookup(const QString& entityTypeCode) const {
  DisEntityInfo info;
  info.entityTypeCode = entityTypeCode;

  const QStringList parts = entityTypeCode.split(':');
  if (parts.size() != 7) {
    return info;
  }

  bool ok = false;
  const int kind = parts.at(0).toInt(&ok);
  if (!ok) {
    return info;
  }
  const int domainCode = parts.at(1).toInt(&ok);
  if (!ok) {
    return info;
  }
  const int country = parts.at(2).toInt(&ok);
  if (!ok) {
    return info;
  }
  const int categoryCode = parts.at(3).toInt(&ok);
  if (!ok) {
    return info;
  }
  const int subcategory = parts.at(4).toInt(&ok);
  if (!ok) {
    return info;
  }
  const int specific = parts.at(5).toInt(&ok);
  if (!ok) {
    return info;
  }
  const int extra = parts.at(6).toInt(&ok);
  if (!ok) {
    return info;
  }

  info.kindDescription = _kindDescriptions.value(kind);
  info.domainDescription = _domainDescriptions.value(domainKey(kind, domainCode));
  info.countryDescription = _countryDescriptions.value(country);

  const ExactEntry exact = _exactEntries.value(entityTypeCode);
  info.categoryDescription = exact.categoryDescription;
  info.subcategoryDescription = exact.subcategoryDescription;
  info.specificDescription = exact.specificDescription;
  info.extraDescription = exact.extraDescription;

  if (info.categoryDescription.isEmpty()) {
    info.categoryDescription = _categoryDescriptions.value(categoryKey(kind, domainCode, categoryCode));
  }

  Q_UNUSED(subcategory)
  Q_UNUSED(specific)
  Q_UNUSED(extra)
  return info;
}

QString DisEntityCatalog::buildCode(const Codes& codes) {
  return Entity::buildEntityTypeCode(
      codes.kind,
      codes.domainCode,
      codes.country,
      codes.categoryCode,
      codes.subcategory,
      codes.specific,
      codes.extra);
}

QList<DisEnumOption> DisEntityCatalog::kinds() const {
  QList<DisEnumOption> options;
  for (auto it = _kindDescriptions.constBegin(); it != _kindDescriptions.constEnd(); ++it) {
    options.append({it.key(), it.value()});
  }
  std::sort(options.begin(), options.end(), [](const DisEnumOption& left, const DisEnumOption& right) {
    return left.id < right.id;
  });
  return options;
}

QList<DisEnumOption> DisEntityCatalog::countries() const {
  QList<DisEnumOption> options;
  for (auto it = _countryDescriptions.constBegin(); it != _countryDescriptions.constEnd(); ++it) {
    options.append({it.key(), it.value()});
  }
  std::sort(options.begin(), options.end(), [](const DisEnumOption& left, const DisEnumOption& right) {
    return left.id < right.id;
  });
  return options;
}

QList<DisEnumOption> DisEntityCatalog::domains(int kind) const {
  QList<DisEnumOption> options;
  const QString prefix = QStringLiteral("%1:").arg(kind);
  for (auto it = _domainDescriptions.constBegin(); it != _domainDescriptions.constEnd(); ++it) {
    if (!it.key().startsWith(prefix)) {
      continue;
    }
    bool ok = false;
    const int id = it.key().section(':', 1, 1).toInt(&ok);
    if (ok) {
      options.append({id, it.value()});
    }
  }
  std::sort(options.begin(), options.end(), [](const DisEnumOption& left, const DisEnumOption& right) {
    return left.id < right.id;
  });
  return options;
}

QList<DisEnumOption> DisEntityCatalog::categories(int kind, int domainCode) const {
  QList<DisEnumOption> options;
  const QString prefix = QStringLiteral("%1:%2:").arg(kind).arg(domainCode);
  for (auto it = _categoryDescriptions.constBegin(); it != _categoryDescriptions.constEnd(); ++it) {
    if (!it.key().startsWith(prefix)) {
      continue;
    }
    bool ok = false;
    const int id = it.key().section(':', 2, 2).toInt(&ok);
    if (ok) {
      options.append({id, it.value()});
    }
  }
  std::sort(options.begin(), options.end(), [](const DisEnumOption& left, const DisEnumOption& right) {
    return left.id < right.id;
  });
  return options;
}

QList<DisEnumOption> DisEntityCatalog::subcategories(
    int kind,
    int domainCode,
    int country,
    int categoryCode) const {
  QList<DisEnumOption> options;
  const QString prefix = QStringLiteral("%1:%2:%3:%4:")
                             .arg(kind)
                             .arg(domainCode)
                             .arg(country)
                             .arg(categoryCode);
  for (auto it = _subcategoryDescriptions.constBegin(); it != _subcategoryDescriptions.constEnd(); ++it) {
    if (!it.key().startsWith(prefix)) {
      continue;
    }
    bool ok = false;
    const int id = it.key().section(':', 4, 4).toInt(&ok);
    if (ok) {
      options.append({id, it.value()});
    }
  }
  std::sort(options.begin(), options.end(), [](const DisEnumOption& left, const DisEnumOption& right) {
    return left.id < right.id;
  });
  return options;
}

QList<DisEnumOption> DisEntityCatalog::specifics(
    int kind,
    int domainCode,
    int country,
    int categoryCode,
    int subcategory) const {
  QList<DisEnumOption> options;
  const QString prefix = QStringLiteral("%1:%2:%3:%4:%5:")
                             .arg(kind)
                             .arg(domainCode)
                             .arg(country)
                             .arg(categoryCode)
                             .arg(subcategory);
  for (auto it = _specificDescriptions.constBegin(); it != _specificDescriptions.constEnd(); ++it) {
    if (!it.key().startsWith(prefix)) {
      continue;
    }
    bool ok = false;
    const int id = it.key().section(':', 5, 5).toInt(&ok);
    if (ok) {
      options.append({id, it.value()});
    }
  }
  std::sort(options.begin(), options.end(), [](const DisEnumOption& left, const DisEnumOption& right) {
    return left.id < right.id;
  });
  return options;
}

QList<DisEnumOption> DisEntityCatalog::extras(
    int kind,
    int domainCode,
    int country,
    int categoryCode,
    int subcategory,
    int specific) const {
  QList<DisEnumOption> options;
  const QString prefix = QStringLiteral("%1:%2:%3:%4:%5:%6:")
                             .arg(kind)
                             .arg(domainCode)
                             .arg(country)
                             .arg(categoryCode)
                             .arg(subcategory)
                             .arg(specific);
  for (auto it = _extraDescriptions.constBegin(); it != _extraDescriptions.constEnd(); ++it) {
    if (!it.key().startsWith(prefix)) {
      continue;
    }
    bool ok = false;
    const int id = it.key().section(':', 6, 6).toInt(&ok);
    if (ok) {
      options.append({id, it.value()});
    }
  }
  std::sort(options.begin(), options.end(), [](const DisEnumOption& left, const DisEnumOption& right) {
    return left.id < right.id;
  });
  return options;
}
