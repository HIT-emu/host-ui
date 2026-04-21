#pragma once
#include <cstdint>

// Shepherd battery model parameters. SI units:
//   voltages    — volts (V)
//   capacity    — ampere-hours (Ah)
//   current     — amperes (A)
//   B           — 1/Ah
//   resistance  — ohms
struct BatteryModelParams {
    double E0;                     // OCV at full charge, V
    double k1;                     // polarization coefficient, V
    double k2;                     // exponential coefficient, V
    double A;                      // exponential zone amplitude, V
    double B;                      // exponential zone time constant, 1/Ah
    double internal_resistance;    // series resistance, Ohm
    double capacity;               // nominal capacity, Ah

    // Default Li-ion preset from battery_emulation_report.md.
    static BatteryModelParams liIonDefault() {
        return {
            /* E0                  */ 3.781272,
            /* k1                  */ 0.093608,
            /* k2                  */ 0.058901,
            /* A                   */ 0.563978,
            /* B                   */ 7.504662,
            /* internal_resistance */ 0.109,
            /* capacity            */ 0.5087,
        };
    }
};

class BatteryEngine {
public:
    void setParameters(const BatteryModelParams &params);
    void resetState();

    // I/O units are mA / sec → mV, to match TelemetryFrame contract.
    // Internally the model operates in A / Ah / V.
    double computeNextVoltage(double current_ma, double delta_time_sec);

    double getConsumedCapacityMah() const { return m_consumed_ah * 1000.0; }
    bool   isDepleted()             const { return m_depleted; }

private:
    BatteryModelParams m_params = BatteryModelParams::liIonDefault();
    double m_consumed_ah = 0.0;
    bool   m_depleted    = false;
};
