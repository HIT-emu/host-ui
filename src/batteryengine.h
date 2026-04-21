#pragma once
#include <stdint.h>
#include "TelemetryFrame.h"

class BatteryEngine {
public:
    void setParameters(const BatteryModelParams& params) { m_params = params; }

    double calculateVoltage(double current_a, double consumed_ah);

private:
    int32_t f_ln(int32_t x);
    int32_t f_exp(int32_t x);
    int32_t f_mul(int32_t a, int32_t b);
    int32_t f_div(int32_t a, int32_t b);

    BatteryModelParams m_params;
};
