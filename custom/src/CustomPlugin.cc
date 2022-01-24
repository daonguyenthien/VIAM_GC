#include "CustomPlugin.h"
#include "QmlComponentInfo.h"

QGC_LOGGING_CATEGORY(CustomLog, "CustomLog")

CustomOptions::CustomOptions(CustomPlugin*, QObject* parent) : QGCOptions(parent) {}

CustomPlugin::CustomPlugin(QGCApplication* app, QGCToolbox* toolbox) : QGCCorePlugin(app, toolbox)
{
  _pOptions = new CustomOptions(this, this);
}

CustomPlugin::~CustomPlugin() {}

QVariantList& CustomPlugin::settingsPages()
{
  if (_customSettingsList.isEmpty())
  {
    addSettingsEntry(tr("General"), "qrc:/qml/GeneralSettings.qml", "qrc:/res/gear-white.svg");
    addSettingsEntry(tr("Comm Links"), "qrc:/qml/LinkSettings.qml", "qrc:/res/waves.svg");
    addSettingsEntry(tr("Offline Maps"), "qrc:/qml/OfflineMap.qml", "qrc:/res/waves.svg");
    addSettingsEntry(tr("MAVLink"), "qrc:/qml/MavlinkSettings.qml", "qrc:/res/waves.svg");
    addSettingsEntry(tr("Console"), "qrc:/qml/QGroundControl/Controls/AppMessages.qml");
  }
  return _customSettingsList;
}

QGCOptions* CustomPlugin::options() { return _pOptions; }

QmlObjectListModel* CustomPlugin::customMapItems(void)
{
  _customMapItems.append(new QmlComponentInfo(
      tr("Custom Mass-Shifter/Piston"), QUrl::fromUserInput("qrc:/qml/CustomSliderMassPiston.qml"), QUrl(), this));
  _customMapItems.append(new QmlComponentInfo(
      tr("Custom Thruster"), QUrl::fromUserInput("qrc:/qml/CustomThrusterFlightView.qml"), QUrl(), this));
  _customMapItems.append(
      new QmlComponentInfo(tr("Custom Rudder"), QUrl::fromUserInput("qrc:/qml/CustomRudderStatus.qml"), QUrl(), this));

  return &_customMapItems;
}

void CustomPlugin::postLoadFromJson(PlanMasterController* pController, QJsonObject& json)
{
  Q_UNUSED(pController);
  Q_UNUSED(json);
}

void CustomPlugin::addSettingsEntry(const QString& title, const char* qmlFile, const char* iconFile)
{
  Q_CHECK_PTR(qmlFile);
  _customSettingsList.append(QVariant::fromValue(new QmlComponentInfo(
      title, QUrl::fromUserInput(qmlFile), iconFile == nullptr ? QUrl() : QUrl::fromUserInput(iconFile), this)));
}
