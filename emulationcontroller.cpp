#include "emulationcontroller.h"
#include <QTimer>

EmulationController::EmulationController(QObject *parent) : QObject(parent) {
    connect(&m_comm, &EmDeviceCommunicator::telemetryReceived, this, &EmulationController::processStep);
    connect(&m_comm, &EmDeviceCommunicator::logMessage, this, &EmulationController::logReceived);
}

bool EmulationController::start(const QString& port,
                                double e0, double k1, double k2,
                                double a,  double b,  double r,
                                double cap,
                                double vout_min_v, double vout_max_v) {
    BatteryModelParams p;
    p.E0          = e0;
    p.k1          = k1;
    p.k2          = k2;
    p.A           = a;
    p.B           = b;
    p.R           = r;
    p.capacity_ah = cap;

    m_engine.setParameters(p);
    m_consumed_ah = 0.0;
    m_last_ts     = 0;
    m_start_ts    = 0;

    m_comm.setVoutRange((uint32_t)(vout_min_v * 1000.0),
                        (uint32_t)(vout_max_v * 1000.0));

    if (!m_comm.openDevice(port)) return false;

    m_comm.sendPowerState(true);

    // Ask firmware to report all telemetry fields (300ms delay for power-on settle)
    QTimer::singleShot(300, this, [this]() {
        m_comm.sendRawString("show all\n");
    });

    m_active = true;
    emit activeChanged();
    return true;
}

void EmulationController::stop() {
    m_comm.sendPowerState(false);
    m_comm.closeDevice();
    m_active = false;
    emit activeChanged();
    if (m_emulating) {
        m_emulating = false;
        emit emulatingChanged();
    }
}

void EmulationController::setEmulating(bool on) {
    if (m_emulating == on) return;
    m_emulating = on;
    emit emulatingChanged();
}

void EmulationController::sendRawCommand(const QString& cmd) {
    if (m_active)
        m_comm.sendRawString(cmd + "\n");
}

void EmulationController::processStep(const TelemetryFrame& frame) {
    if (m_last_ts == 0) {
        m_last_ts  = frame.timestamp_ms;
        m_start_ts = frame.timestamp_ms;
        return;
    }
    if (frame.timestamp_ms <= m_last_ts)
        return;

    double dt_h     = (double)(frame.timestamp_ms - m_last_ts) / 3600000.0;
    double current_a = frame.current_ma / 1000.0;
    m_consumed_ah      += current_a * dt_h;
    m_last_ts           = frame.timestamp_ms;
    m_last_current_ua   = frame.current_ma * 1000.0;

    m_modelV = m_engine.calculateVoltage(current_a, m_consumed_ah);

    if (m_emulating)
        m_comm.sendTargetVoltage((uint32_t)(m_modelV * 1000.0));

    double t = (double)(frame.timestamp_ms - m_start_ts) / 1000.0;
    emit statusChanged();
    emit pointsUpdated(t, m_modelV, m_last_current_ua);
}
