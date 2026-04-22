#include "batteryengine.h"
#include <cmath>
#include <algorithm>

double BatteryEngine::calculateVoltage(double current_a, double consumed_ah) {
    const double Q = m_params.capacity_ah;
    const double soc = std::max(1e-6, std::min(consumed_ah / Q, 1.0 - 1e-6));

    return m_params.E0
         + m_params.k1 * std::log(1.0 - soc)
         + m_params.k2 * std::log(soc)
         - m_params.R * current_a
         + m_params.A  * std::exp(-m_params.B * consumed_ah);
}
