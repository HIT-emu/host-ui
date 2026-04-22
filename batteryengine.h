#pragma once
#include <stdint.h>
#include "TelemetryFrame.h"

class BatteryEngine {
public:
    void setParameters(const BatteryModelParams& params) { m_params = params; }

    double calculateVoltage(double current_a, double consumed_ah);

private:
    BatteryModelParams m_params;
};
