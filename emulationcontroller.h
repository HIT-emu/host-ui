#pragma once
#include <QObject>
#include "emdevicecommunicator.h"
#include "batteryengine.h"

class EmulationController : public QObject {
    Q_OBJECT
    Q_PROPERTY(double modelVoltage READ modelVoltage NOTIFY statusChanged)
    Q_PROPERTY(double consumedAh READ consumedAh NOTIFY statusChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)

public:
    explicit EmulationController(QObject *parent = nullptr);

    double modelVoltage() const { return m_modelV; }
    double consumedAh() const { return m_consumed_ah; }
    bool active() const { return m_active; }

    Q_INVOKABLE bool start(const QString& port, double e0, double cap);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void sendRawCommand(const QString& cmd);

signals:
    void statusChanged();
    void activeChanged();
    void logReceived(QString msg);
    void pointsUpdated(double t, double v); // Для графика

private slots:
    void processStep(const TelemetryFrame& frame);

private:
    EmDeviceCommunicator m_comm;
    BatteryEngine m_engine;
    double m_consumed_ah = 0.0;
    double m_modelV = 0.0;
    uint32_t m_last_ts = 0, m_start_ts = 0;
    bool m_active = false;
};
