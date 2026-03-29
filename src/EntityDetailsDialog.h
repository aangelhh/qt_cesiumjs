#pragma once

#include <QDialog>
#include <QVariantMap>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QPlainTextEdit;
class QTableWidget;

class EntityDetailsDialog : public QDialog {
  Q_OBJECT

public:
  explicit EntityDetailsDialog(const QVariantMap& summary, QWidget* parent = nullptr);

private slots:
  void updateSection(QListWidgetItem* current, QListWidgetItem* previous);

private:
  void buildUi();
  void populateHeader();
  void populateStateData();
  void populateSensorInformation();
  void populatePlaceholderSection(const QString& sectionName);
  void setTableRows(const QList<QPair<QString, QString>>& rows);
  QString value(const char* key, const QString& fallback = QStringLiteral("-")) const;
  QString forceGlyph() const;
  QString locationText() const;

  QVariantMap _summary;
  QLabel* _iconLabel;
  QLabel* _nameLabel;
  QLabel* _typeLabel;
  QLabel* _groundSpeedLabel;
  QLabel* _altitudeLabel;
  QLabel* _tasLabel;
  QLabel* _statusLabel;
  QListWidget* _sectionsList;
  QTableWidget* _detailsTable;
  QPlainTextEdit* _console;
};
