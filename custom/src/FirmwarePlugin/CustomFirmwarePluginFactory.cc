#include "CustomFirmwarePluginFactory.h"
#include "CustomFirmwarePlugin.h"

CustomFirmwarePluginFactory CustomFirmwarePluginFactory;

CustomFirmwarePluginFactory::CustomFirmwarePluginFactory(void) : _customPluginInstance(nullptr) {}

QList<MAV_AUTOPILOT> CustomFirmwarePluginFactory::supportedFirmwareTypes(void) const
{
  QList<MAV_AUTOPILOT> list;

  list.append(MAV_AUTOPILOT_GENERIC);
  return list;
}

FirmwarePlugin* CustomFirmwarePluginFactory::firmwarePluginForAutopilot(MAV_AUTOPILOT autopilotType,
                                                                        MAV_TYPE vehicleType)
{
  Q_UNUSED(vehicleType);
  if (autopilotType == MAV_AUTOPILOT_GENERIC)
  {
    _customPluginInstance = new CustomFirmwarePlugin;
    return _customPluginInstance;
  }

  return nullptr;
}
