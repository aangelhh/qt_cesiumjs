#pragma once

#include <QString>
#include <QUrl>

class CesiumScenePage {
public:
  static QString readConfigValue(const QString& key);
  static QString defaultAccessToken();
  static QString defaultAssetId();
  static QUrl cesiumSourceBaseUrl();
  static QString buildHtml(const QString& accessToken);
  static QUrl writeLocalHtmlPage(const QString& accessToken);
};
