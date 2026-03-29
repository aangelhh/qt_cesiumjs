#pragma once

#include "domain/Entity.h"

#include <QHash>
#include <QList>
#include <QString>

struct DisEntityInfo {
  QString entityTypeCode;
  QString kindDescription;
  QString domainDescription;
  QString countryDescription;
  QString categoryDescription;
  QString subcategoryDescription;
  QString specificDescription;
  QString extraDescription;

  bool isValid() const;
  QString displayName() const;
};

struct DisEnumOption {
  int id = 0;
  QString description;
};

class DisEntityCatalog {
public:
  static QString defaultCatalogPath();
  static DisEntityCatalog loadDefault();
  static DisEntityCatalog loadFromFile(const QString& path);

  bool isLoaded() const;
  QString errorString() const;

  DisEntityInfo lookup(const Entity& entity) const;
  DisEntityInfo lookup(
      int kind,
      int domainCode,
      int country,
      int categoryCode,
      int subcategory,
      int specific,
      int extra) const;
  DisEntityInfo lookup(const QString& entityTypeCode) const;
  QList<DisEnumOption> kinds() const;
  QList<DisEnumOption> countries() const;
  QList<DisEnumOption> domains(int kind) const;
  QList<DisEnumOption> categories(int kind, int domainCode) const;
  QList<DisEnumOption> subcategories(int kind, int domainCode, int country, int categoryCode) const;
  QList<DisEnumOption> specifics(
      int kind,
      int domainCode,
      int country,
      int categoryCode,
      int subcategory) const;
  QList<DisEnumOption> extras(
      int kind,
      int domainCode,
      int country,
      int categoryCode,
      int subcategory,
      int specific) const;

  // Internal parser support kept visible to the translation unit helpers.
  struct Codes {
    int kind = 0;
    int domainCode = 0;
    int country = 0;
    int categoryCode = 0;
    int subcategory = 0;
    int specific = 0;
    int extra = 0;
  };

  struct ExactEntry {
    QString categoryDescription;
    QString subcategoryDescription;
    QString specificDescription;
    QString extraDescription;
  };

  static QString buildCode(const Codes& codes);

  // Internal parser state.
  QHash<int, QString> _kindDescriptions;
  QHash<int, QString> _countryDescriptions;
  QHash<QString, QString> _domainDescriptions;
  QHash<QString, QString> _categoryDescriptions;
  QHash<QString, QString> _subcategoryDescriptions;
  QHash<QString, QString> _specificDescriptions;
  QHash<QString, QString> _extraDescriptions;
  QHash<QString, ExactEntry> _exactEntries;

private:
  QString _errorString;
};
