#pragma once

#include "FirmwarePlugin.h"

class CustomFirmwarePlugin;

class CustomFirmwarePluginFactory : public FirmwarePluginFactory
{
  Q_OBJECT

public:
  CustomFirmwarePluginFactory(void);

  QList<MAV_AUTOPILOT> supportedFirmwareTypes(void) const final;
  FirmwarePlugin* firmwarePluginForAutopilot(MAV_AUTOPILOT autopilotType,
                                             MAV_TYPE vehicleType) final;

private:
  CustomFirmwarePlugin* _customPluginInstance;
};
