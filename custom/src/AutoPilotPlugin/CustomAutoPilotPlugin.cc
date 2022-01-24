#include "CustomAutoPilotPlugin.h"

CustomAutoPilotPlugin::CustomAutoPilotPlugin(Vehicle* vehicle, QObject* parent)
    : AutoPilotPlugin(vehicle, parent)
{
}

const QVariantList& CustomAutoPilotPlugin::vehicleComponents(void)
{
  return _components;
}

QString CustomAutoPilotPlugin::prerequisiteSetup(VehicleComponent* component) const
{
  return QString();
}
