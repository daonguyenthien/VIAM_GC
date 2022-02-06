#include "CustomFirmwarePlugin.h"
#include "CustomAutoPilotPlugin.h"
#include "QGCApplication.h"

QGC_LOGGING_CATEGORY(CustomFirmwarePluginLog, "CustomFirmwarePluginLog")

CustomMode::CustomMode(uint32_t mode, bool settable) : _mode(mode), _settable(settable)
{
  QMap<uint32_t, QString> enumToString;
  enumToString.insert(AUTO_HEADING, "Auto Heading");
  enumToString.insert(LOS_STRAIGHT, "LOS Straight");
  enumToString.insert(LOS_BSPLINE, "LOS BSpline");
  enumToString.insert(LOS_SBG, "LOS SBG");
  enumToString.insert(LOS_DUBINS, "LOS Dubins");
  enumToString.insert(LOS_FERMAT, "LOS Fermat");
  setEnumToStringMapping(enumToString);
}

void CustomMode::setEnumToStringMapping(const QMap<uint32_t, QString>& enumToString) { _enumToString = enumToString; }

QString CustomMode::modeString() const
{
  QString mode = _enumToString.value(modeAsInt());
  if (mode.isEmpty())
    mode = "mode" + QString::number(modeAsInt());
  return mode;
}

CustomFirmwarePlugin::CustomFirmwarePlugin()
{
  QList<CustomMode> supportedFlightModes;
  supportedFlightModes << CustomMode(CustomMode::AUTO_HEADING, true);
  supportedFlightModes << CustomMode(CustomMode::LOS_STRAIGHT, true);
  supportedFlightModes << CustomMode(CustomMode::LOS_BSPLINE, true);
  supportedFlightModes << CustomMode(CustomMode::LOS_SBG, true);
  supportedFlightModes << CustomMode(CustomMode::LOS_DUBINS, true);
  supportedFlightModes << CustomMode(CustomMode::LOS_FERMAT, true);
  setSupportedModes(supportedFlightModes);

  _factRenameMap[QStringLiteral("flightTime")] = QStringLiteral("Travelled Time");
  _factRenameMap[QStringLiteral("altitudeAMSL")] = QStringLiteral("Depth");
}


AutoPilotPlugin* CustomFirmwarePlugin::autopilotPlugin(Vehicle* vehicle)
{
  return new CustomAutoPilotPlugin(vehicle, vehicle);
}

bool CustomFirmwarePlugin::isCapable(const Vehicle* vehicle, FirmwareCapabilities capabilities)
{
  Q_UNUSED(vehicle);
  uint32_t available = SetFlightModeCapability | PauseVehicleCapability | GuidedModeCapability;
  return (capabilities & available) == capabilities;
}

QStringList CustomFirmwarePlugin::flightModes(Vehicle* vehicle)
{
  Q_UNUSED(vehicle)
  QStringList flightModesList;
  foreach (const CustomMode& customMode, _supportedModes)
  {
    if (customMode.canBeSet())
      flightModesList << customMode.modeString();
  }
  return flightModesList;
}

QString CustomFirmwarePlugin::flightMode(uint8_t base_mode, uint32_t custom_mode) const
{
  QString flightMode = "Unknown";

  if (base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED)
  {
    foreach (const CustomMode& customMode, _supportedModes)
    {
      if (customMode.modeAsInt() == custom_mode)
        flightMode = customMode.modeString();
    }
  }
  return flightMode;
}

bool CustomFirmwarePlugin::setFlightMode(const QString& flightMode, uint8_t* base_mode, uint32_t* custom_mode)
{
  *base_mode = 0;
  *custom_mode = 0;

  bool found = false;

  foreach (const CustomMode& mode, _supportedModes)
  {
    if (flightMode.compare(mode.modeString(), Qt::CaseInsensitive) == 0)
    {
      *base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
      *custom_mode = mode.modeAsInt();
      found = true;
      break;
    }
  }

  if (!found)
    qCWarning(CustomFirmwarePluginLog) << "Unknown flight Mode" << flightMode;

  return found;
}

void CustomFirmwarePlugin::startMission(Vehicle* vehicle)
{
  if (!vehicle->armed())
    if (!_armVehicleAndValidate(vehicle))
    {
      qgcApp()->showMessage(tr("Unable to start mission: Vehicle failed to arm."));
      return;
    }
  vehicle->sendMavCommand(vehicle->defaultComponentId(), MAV_CMD_MISSION_START, true /*show error */);
}

bool CustomFirmwarePlugin::adjustIncomingMavlinkMessage(Vehicle* vehicle, mavlink_message_t* message)
{
  if (message->compid != MAV_COMP_ID_AUTOPILOT1)
    return true;

  switch (message->msgid)
  {
  case MAVLINK_MSG_ID_VIAM_ODOMETRY:
    _handleOdometry(vehicle, message);
    break;
  }
  return true;
}

bool CustomFirmwarePlugin::sendHomePositionToVehicle(void) { return true; }

const QVariantList& CustomFirmwarePlugin::toolBarIndicators(const Vehicle* vehicle)
{
  Q_UNUSED(vehicle);

  if (_toolBarIndicatorList.size() == 0)
  {
    _toolBarIndicatorList.append(QVariant::fromValue(QUrl::fromUserInput("qrc:/toolbar/MessageIndicator.qml")));
    _toolBarIndicatorList.append(QVariant::fromValue(QUrl::fromUserInput("qrc:/qml/BatteryHeadIndicator.qml")));
    _toolBarIndicatorList.append(QVariant::fromValue(QUrl::fromUserInput("qrc:/qml/BatteryTailIndicator.qml")));
    _toolBarIndicatorList.append(QVariant::fromValue(QUrl::fromUserInput("qrc:/toolbar/JoystickIndicator.qml")));
    _toolBarIndicatorList.append(QVariant::fromValue(QUrl::fromUserInput("qrc:/toolbar/ModeIndicator.qml")));
    _toolBarIndicatorList.append(QVariant::fromValue(QUrl::fromUserInput("qrc:/toolbar/ArmedIndicator.qml")));
  }
  return _toolBarIndicatorList;
}

QString CustomFirmwarePlugin::brandImageIndoor(const Vehicle* vehicle) const
{
  Q_UNUSED(vehicle);
  return QStringLiteral("/res/APMBrandImageAUV2000.png");
}
QString CustomFirmwarePlugin::brandImageOutdoor(const Vehicle* vehicle) const
{
  Q_UNUSED(vehicle);
  return QStringLiteral("/res/APMBrandImageAUV2000.png");
}

void CustomFirmwarePlugin::adjustMetaData(MAV_TYPE vehicleType, FactMetaData* metaData)
{
  Q_UNUSED(vehicleType);

  if (!metaData)
    return;

  if (_factRenameMap.contains(metaData->name()))
    metaData->setShortDescription(QString(_factRenameMap[metaData->name()]));
}

QString CustomFirmwarePlugin::_getLatestVersionFileUrl(Vehicle* vehicle)
{
  Q_UNUSED(vehicle);
  return QString();
}

void CustomFirmwarePlugin::setSupportedModes(QList<CustomMode> supportedModes) { _supportedModes = supportedModes; }

void CustomFirmwarePlugin::_handleOdometry(Vehicle* vehicle, mavlink_message_t* message)
{
  mavlink_viam_odometry_t odom;
  mavlink_msg_viam_odometry_decode(message, &odom);

  // Update altitude
  vehicle->getFact("altitudeAMSL")->setRawValue(odom.alt);

  // Update attitude
  vehicle->roll()->setRawValue(qRadiansToDegrees(odom.roll));
  vehicle->pitch()->setRawValue(qRadiansToDegrees(odom.pitch));
  auto heading = qRadiansToDegrees(odom.yaw);
  if (heading < 0.0f)
    heading += 360.0f;
  heading = trunc(heading);
  vehicle->heading()->setRawValue(heading);

  // Update angular rate
  vehicle->rollRate()->setRawValue(qRadiansToDegrees(odom.vroll));
  vehicle->pitchRate()->setRawValue(qRadiansToDegrees(odom.vpitch));
  vehicle->yawRate()->setRawValue(qRadiansToDegrees(odom.vyaw));

  // Update ground speed
  vehicle->getFact("groundSpeed")->setRawValue(odom.vx);
}
