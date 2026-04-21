#include "BatteryEngine.h"
#include <cmath>
#include <algorithm>

void BatteryEngine::setParameters(const BatteryModelParams &params)
{
    m_params = params;
    resetState();
}

void BatteryEngine::resetState()
{
    m_consumed_ah = 0.0;
    m_depleted    = false;
}

// Shepherd discharge model (see THEORY/battery_emulation_report.md):
//   V(C) = E0 + k1 * ln(1 - C/Q) + k2 * ln(C/Q) - R*I + A * exp(-B*C)
// where C — consumed capacity (Ah), Q — nominal capacity (Ah),
//       I — instantaneous current (A), R — series resistance (Ohm).
double BatteryEngine::computeNextVoltage(double current_ma, double delta_time_sec)
{
    if (m_depleted)
        return 0.0;

    const double I = current_ma / 1000.0;                         // mA → A
    const double Q = m_params.capacity;
    const double C = std::clamp(m_consumed_ah, 1e-6, Q - 1e-6);   // keep ln() argument strictly positive

    const double soc    = C / Q;
    const double ln_1ms = std::log(1.0 - soc);
    const double ln_s   = std::log(soc);

    const double v = m_params.E0
                   + m_params.k1 * ln_1ms
                   + m_params.k2 * ln_s
                   - m_params.internal_resistance * I
                   + m_params.A * std::exp(-m_params.B * C);

    // Integrate consumed capacity (A * s → Ah).
    m_consumed_ah += I * (delta_time_sec / 3600.0);

    if (m_consumed_ah >= Q) {
        m_consumed_ah = Q;
        m_depleted = true;
    }

    return v * 1000.0;  // V → mV
}
