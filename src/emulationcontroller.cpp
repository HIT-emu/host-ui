#include "emulationcontroller.h"

EmulationController::EmulationController(QObject *parent) : QObject(parent) {
    connect(&m_comm, &EmDeviceCommunicator::telemetryReceived, this, &EmulationController::processStep);
}

bool EmulationController::start(const QString& port, const BatteryModelParams& params) {
    m_engine.setParameters(params);
    m_consumed_ah = 0.0;
    m_last_ts = 0;
    if (!m_comm.openDevice(port))
        return false;
    m_comm.sendPowerState(true);
    return true;
}

void EmulationController::processStep(const TelemetryFrame& frame) {
    if (m_last_ts == 0) { m_last_ts = frame.timestamp_ms; return; }

    // 1. Интегрируем ток для получения расхода емкости (Ah)
    double dt_h = (frame.timestamp_ms - m_last_ts) / 3600000.0;
    m_consumed_ah += (frame.current_ma / 1000.0) * dt_h;
    m_last_ts = frame.timestamp_ms;

    // 2. Считаем целевое напряжение через C-модель
    double targetV = m_engine.calculateVoltage(frame.current_ma / 1000.0, m_consumed_ah);

    // 3. Отправляем команду на железку
    m_comm.sendTargetVoltage((uint32_t)(targetV * 1000.0));

    emit statusUpdated(targetV, m_consumed_ah);
}
