#pragma once

#include "AutoPilotPlugin.h"
#include "Vehicle.h"

class CustomAutoPilotPlugin : public AutoPilotPlugin
{
  Q_OBJECT
public:
  CustomAutoPilotPlugin(Vehicle* vehicle, QObject* parent = nullptr);

  const QVariantList& vehicleComponents(void) override;
  QString prerequisiteSetup(VehicleComponent* component) const override;

private:
    QVariantList                _components;
};
