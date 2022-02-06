#pragma once

#include "FirmwarePlugin.h"
#include "QGCLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(CustomFirmwarePluginLog)

class CustomMode
{
public:
  CustomMode(uint32_t mode, bool settable);
  uint32_t modeAsInt() const { return _mode; }
  bool canBeSet() const { return _settable; }
  QString modeString() const;

  enum Mode
  {
    AUTO_HEADING = 1,
    LOS_STRAIGHT = 2,
    LOS_BSPLINE = 3,
    LOS_SBG = 4,
    LOS_DUBINS = 5,
    LOS_FERMAT = 6,
  };

protected:
  void setEnumToStringMapping(const QMap<uint32_t, QString>& enumToString);

private:
  uint32_t _mode;
  bool _settable;
  QMap<uint32_t, QString> _enumToString;
};

class CustomFirmwarePlugin : public FirmwarePlugin
{
  Q_OBJECT
public:
  CustomFirmwarePlugin();
  //~CustomFirmwarePlugin() override;

  AutoPilotPlugin* autopilotPlugin(Vehicle* vehicle) override;
  bool isCapable(const Vehicle* vehicle, FirmwareCapabilities capabilities) override;
  QStringList flightModes(Vehicle* vehicle) override;
  QString flightMode(uint8_t base_mode, uint32_t custom_mode) const override;
  bool setFlightMode(const QString& flightMode, uint8_t* base_mode, uint32_t* custom_mode) override;
  void startMission(Vehicle* vehicle) override;
  bool adjustIncomingMavlinkMessage(Vehicle* vehicle, mavlink_message_t* message) override;
  bool sendHomePositionToVehicle(void) override;
  const QVariantList& toolBarIndicators(const Vehicle* vehicle) override;
  QString brandImageIndoor(const Vehicle* vehicle) const override;
  QString brandImageOutdoor(const Vehicle* vehicle) const override;
  void adjustMetaData(MAV_TYPE vehicleType, FactMetaData* metaData) override;

protected:
  QString _getLatestVersionFileUrl(Vehicle* vehicle) override;

private:
  void setSupportedModes(QList<CustomMode> supportedModes);
  void _handleOdometry(Vehicle* vehicle, mavlink_message_t* message);

  QList<CustomMode> _supportedModes;
  QVariantList _toolBarIndicatorList;
  QMap<QString, QString> _factRenameMap;
};
