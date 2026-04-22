#ifndef TELEMETRYFRAME_H
#define TELEMETRYFRAME_H

#include <stdint.h>

struct TelemetryFrame {
    uint32_t timestamp_ms; // Время с МК
    double voltage_mv;     // Напряжение (мВ)
    double current_ma;     // Ток (мА)
};

struct BatteryModelParams {
    double E0, k1, k2, A, B, R;
    double capacity_ah;
};

#endif
