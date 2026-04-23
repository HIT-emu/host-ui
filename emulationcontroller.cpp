#include "emulationcontroller.h"
#include <QTimer>

EmulationController::EmulationController(QObject *parent) : QObject(parent) {
    connect(&m_comm, &EmDeviceCommunicator::telemetryReceived, this, &EmulationController::processStep);
    connect(&m_comm, &EmDeviceCommunicator::logMessage,        this, &EmulationController::logReceived);
    connect(&m_comm, &EmDeviceCommunicator::connectionLost,    this, &EmulationController::onConnectionLost);
}

bool EmulationController::openPort(const QString& port) {
    if (m_connected || m_active) return true;
    m_port = port;
    if (!m_comm.openDevice(port)) return false;
    m_connected = true;
    emit connectedChanged();
    return true;
}

void EmulationController::closePort() {
    if (m_active) return;
    m_comm.closeDevice();
    m_connected = false;
    emit connectedChanged();
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
    m_port        = port;

    m_comm.setVoutRange((uint32_t)(vout_min_v * 1000.0),
                        (uint32_t)(vout_max_v * 1000.0));

    if (!m_comm.openDevice(port)) return false;

    m_active = true;
    emit activeChanged();

    doConnect();
    return true;
}

// Sends power on + show all with safe delays.
// power on  → 500ms → board settles, USB may glitch
// show all  → 1200ms → reconnect (if needed) is done by then
void EmulationController::doConnect() {
    QTimer::singleShot(300, this, [this]() {
        if (m_active) m_comm.sendPowerState(true);
    });
    QTimer::singleShot(1200, this, [this]() {
        if (m_active) m_comm.sendRawString("show all\n");
    });
}

void EmulationController::onConnectionLost() {
    if (!m_active) return;
    QTimer::singleShot(700, this, [this]() { tryReconnect(); });
}

void EmulationController::tryReconnect() {
    if (!m_active) return;
    if (m_comm.openDevice(m_port)) {
        emit logReceived("[INFO] Reconnected");
        QTimer::singleShot(300, this, [this]() {
            if (m_active) m_comm.sendRawString("show all\n");
        });
    } else {
        // USB not ready yet — keep retrying
        QTimer::singleShot(700, this, [this]() { tryReconnect(); });
    }
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
    m_comm.sendRawString(cmd + "\n");
}

void EmulationController::processStep(const TelemetryFrame& frame) {
    if (m_last_ts != 0) {
        if (frame.timestamp_ms <= m_last_ts)
            return;
        double dt_h      = (double)(frame.timestamp_ms - m_last_ts) / 3600000.0;
        double current_a = frame.current_ma / 1000.0;
        m_consumed_ah   += current_a * dt_h;
    }

    m_last_ts         = frame.timestamp_ms;
    m_last_current_ua = frame.current_ma * 1000.0;

    double current_a = frame.current_ma / 1000.0;
    m_modelV = m_engine.calculateVoltage(current_a, m_consumed_ah);

    if (m_emulating)
        m_comm.sendTargetVoltage((uint32_t)(m_modelV * 1000.0));

    double t = (double)(frame.timestamp_ms) / 1000.0;
    emit statusChanged();
    emit pointsUpdated(t, m_modelV, m_last_current_ua);
}
