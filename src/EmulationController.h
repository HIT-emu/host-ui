#pragma once
#include <QObject>
#include <QElapsedTimer>
#include "EmDeviceCommunicator.h"
#include "BatteryEngine.h"

class EmulationController : public QObject {
    Q_OBJECT

public:
    explicit EmulationController(QObject *parent = nullptr);

    bool connectHardware(const QString &portName);
    void configureModel(const BatteryModelParams &params);

    EmDeviceCommunicator &communicator() { return m_communicator; }

    // Cutoff voltage below which emulation stops, mV
    void setCutoffVoltage(double mv) { m_cutoff_mv = mv; }

public slots:
    void startEmulation();
    void stopEmulation();

signals:
    void stateUpdated(double target_mv, const TelemetryFrame &telemetry);
    void emulationFinished(const QString &reason);

private slots:
    void handleIncomingTelemetry(const TelemetryFrame &frame);

private:
    EmDeviceCommunicator m_communicator;
    BatteryEngine        m_engine;

    bool     m_isRunning    = false;
    double   m_cutoff_mv    = 2800.0;  // 2.8V default cutoff
    uint32_t m_last_ts_ms   = 0;
};
