#pragma once

#include "QGCCorePlugin.h"
#include "QGCLoggingCategory.h"
#include "QGCOptions.h"

Q_DECLARE_LOGGING_CATEGORY(CustomLog)

class CustomPlugin;

class CustomOptions : public QGCOptions
{
public:
  CustomOptions(CustomPlugin*, QObject* parent = nullptr);
};

class CustomPlugin : public QGCCorePlugin
{
  Q_OBJECT
public:
  CustomPlugin(QGCApplication* app, QGCToolbox* toolbox);
  ~CustomPlugin();

  QVariantList& settingsPages() final;
  QGCOptions* options() final;
  QmlObjectListModel* customMapItems(void) final;
  void postLoadFromJson(PlanMasterController* pController, QJsonObject& json) final;

private:
  CustomOptions* _pOptions;
  QVariantList _customSettingsList;
  QmlObjectListModel _customMapItems;

  void addSettingsEntry(const QString& title, const char* qmlFile, const char* iconFile = nullptr);
};
