#include "emulationcontroller.h"

EmulationController::EmulationController(QObject *parent) : QObject(parent) {
    connect(&m_comm, &EmDeviceCommunicator::telemetryReceived, this, &EmulationController::processStep);
    connect(&m_comm, &EmDeviceCommunicator::logMessage, this, &EmulationController::logReceived);
}

bool EmulationController::start(const QString& port, double e0, double cap) {
    BatteryModelParams p;
    p.E0 = e0; p.capacity_ah = cap;
    p.k1 = 0.09; p.k2 = 0.05; p.A = 0.56; p.B = 7.5;

    m_engine.setParameters(p);
    m_consumed_ah = 0.0; m_last_ts = 0; m_start_ts = 0;

    if (!m_comm.openDevice(port)) return false;

    m_comm.sendPowerState(true);
    m_active = true;
    emit activeChanged();
    return true;
}

void EmulationController::sendRawCommand(const QString& cmd) {
    if (m_active) {
        // Мы используем уже готовый механизм записи в порт через m_comm
        // Но добавим прямой доступ к записи строки
        m_comm.sendRawString(cmd + "\n");
    }
}

void EmulationController::stop() {
    m_comm.sendPowerState(false);
    m_comm.closeDevice();
    m_active = false;
    emit activeChanged();
}

void EmulationController::processStep(const TelemetryFrame& frame) {
    if (m_last_ts == 0 || frame.timestamp_ms <= m_last_ts) {
        m_last_ts = frame.timestamp_ms;
        if(m_start_ts == 0) m_start_ts = frame.timestamp_ms;
        return;
    }

    // Интервал в часах
    double dt_h = (double)(frame.timestamp_ms - m_last_ts) / 3600000.0;

    // Накопление емкости (Current в мА -> в А)
    double current_a = frame.current_ma / 1000.0;
    m_consumed_ah += current_a * dt_h;
    m_last_ts = frame.timestamp_ms;

    // Расчет новой точки модели
    m_modelV = m_engine.calculateVoltage(current_a, m_consumed_ah);

    // Передача команды «железу»
    m_comm.sendTargetVoltage((uint32_t)(m_modelV * 1000.0));

    emit statusChanged();
    emit pointsUpdated((double)(frame.timestamp_ms - m_start_ts) / 1000.0, m_modelV);
}
