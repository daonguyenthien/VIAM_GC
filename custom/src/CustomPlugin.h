#pragma once
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickView>
#include <QGeoCircle>
#include <QList>
#include <QObject>
#include <QGeoCoordinate>

#include "QGCApplication.h"
#include "QGCCorePlugin.h"
#include "QGCLoggingCategory.h"
#include "QGCOptions.h"
#include "QmlComponentInfo.h"
#include "PlanMasterController.h"
#include "SettingsManager.h"
#include "QGCPalette.h"
#include "QGCQGeoCoordinate.h"
#include "QGCMapPolyline.h"

#include "Custom/PathPlanningDubins.h"
#include "Custom/PathController.h"

#include <fstream>
#include <iomanip>
Q_DECLARE_LOGGING_CATEGORY(CustomLog)

class CustomPlugin;

class CustomOptions : public QGCOptions
{
public:
  CustomOptions(CustomPlugin*, QObject* parent = nullptr);
    QUrl                    flyViewOverlay                  (void) const final;
//private:
//  QUrl _flyViewOverlay;
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
  QString                 brandImageIndoor                (void) const final;
  QString                 brandImageOutdoor               (void) const final;
  void                    paletteOverride                 (QString colorName, QGCPalette::PaletteColorInfo_t& colorInfo) final;
  QQmlApplicationEngine*  createRootWindow                (QObject* parent) final;
  signals:
  void pathCreated();

private:
  CustomOptions* _pOptions;
  QVariantList _customSettingsList;
  QmlObjectListModel _customMapItems;

  void addSettingsEntry(const QString& title, const char* qmlFile, const char* iconFile = nullptr);
};
