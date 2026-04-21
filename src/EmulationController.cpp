#include "EmulationController.h"
#include <QDebug>

EmulationController::EmulationController(QObject *parent)
    : QObject(parent)
{
    connect(&m_communicator, &EmDeviceCommunicator::telemetryReceived,
            this, &EmulationController::handleIncomingTelemetry);

    connect(&m_communicator, &EmDeviceCommunicator::deviceLogEmitted,
            this, [](const QString &msg) { qInfo() << msg; });

    connect(&m_communicator, &EmDeviceCommunicator::connectionError,
            this, [this](const QString &err) {
        qCritical() << "[HW ERROR]" << err;
        stopEmulation();
    });
}

bool EmulationController::connectHardware(const QString &portName)
{
    return m_communicator.openDevice(portName);
}

void EmulationController::configureModel(const BatteryModelParams &params)
{
    m_engine.setParameters(params);
}

void EmulationController::startEmulation()
{
    if (m_isRunning)
        return;

    m_engine.resetState();
    m_last_ts_ms = 0;
    m_isRunning  = true;
    m_communicator.sendPowerState(true);
    qInfo() << "[EMU] Emulation started";
}

void EmulationController::stopEmulation()
{
    if (!m_isRunning)
        return;

    m_isRunning = false;
    m_communicator.sendPowerState(false);
    qInfo() << "[EMU] Emulation stopped";
}

void EmulationController::handleIncomingTelemetry(const TelemetryFrame &frame)
{
    if (!m_isRunning)
        return;

    // Delta time since last frame, seconds
    double dt = (m_last_ts_ms == 0)
                ? 0.1
                : (frame.timestamp_ms - m_last_ts_ms) / 1000.0;
    m_last_ts_ms = frame.timestamp_ms;

    double target_mv = m_engine.computeNextVoltage(frame.current_ma, dt);

    if (target_mv <= m_cutoff_mv || m_engine.isDepleted()) {
        stopEmulation();
        emit emulationFinished("Battery depleted (cutoff voltage reached)");
        return;
    }

    m_communicator.sendTargetVoltage(static_cast<uint32_t>(target_mv));
    emit stateUpdated(target_mv, frame);
}
