#pragma once
#include <cstdint>

struct TelemetryFrame {
    uint32_t timestamp_ms;
    double voltage_mv;
    double current_ma;
    double consumed_mah;
};
