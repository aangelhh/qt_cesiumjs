#pragma once

#include <QDialog>
#include <QVariantMap>

#include <functional>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QPlainTextEdit;
class QTableWidget;

class EntityDetailsDialog : public QDialog {
  Q_OBJECT

public:
  using SummaryProvider = std::function<QVariantMap()>;

  explicit EntityDetailsDialog(
      const QVariantMap& summary,
      SummaryProvider summaryProvider = {},
      QWidget* parent = nullptr);

private slots:
  void updateSection(QListWidgetItem* current, QListWidgetItem* previous);

private:
  void buildUi();
  void populateHeader();
  void populateStateData();
  void populateTaskInformation();
  void populateAppearanceInformation();
  void populateResourcesInformation();
  void populateSensorInformation();
  void populateEmitterInformation();
  void populateSubsystemInformation();
  void refreshSummary();
  void refreshCurrentSection(bool logTransition);
  void setTableRows(const QList<QPair<QString, QString>>& rows);
  QString value(const char* key, const QString& fallback = QStringLiteral("-")) const;
  QString forceGlyph() const;
  QString locationText() const;

  QVariantMap _summary;
  SummaryProvider _summaryProvider;
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
