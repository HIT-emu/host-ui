#pragma once
#include <QObject>
#include "emdevicecommunicator.h"
#include "batteryengine.h"

class EmulationController : public QObject {
    Q_OBJECT
    Q_PROPERTY(double modelVoltage  READ modelVoltage  NOTIFY statusChanged)
    Q_PROPERTY(double consumedAh   READ consumedAh   NOTIFY statusChanged)
    Q_PROPERTY(double lastCurrentUa READ lastCurrentUa NOTIFY statusChanged)
    Q_PROPERTY(bool   active        READ active        NOTIFY activeChanged)
    Q_PROPERTY(bool   connected     READ connected     NOTIFY connectedChanged)
    Q_PROPERTY(bool   emulating     READ emulating     NOTIFY emulatingChanged)

public:
    explicit EmulationController(QObject *parent = nullptr);

    double modelVoltage()   const { return m_modelV; }
    double consumedAh()    const { return m_consumed_ah; }
    double lastCurrentUa() const { return m_last_current_ua; }
    bool   active()        const { return m_active; }
    bool   connected()     const { return m_connected; }
    bool   emulating()    const { return m_emulating; }

    Q_INVOKABLE bool openPort(const QString& port);
    Q_INVOKABLE void closePort();
    Q_INVOKABLE bool start(const QString& port,
                           double e0, double k1, double k2,
                           double a,  double b,  double r,
                           double cap,
                           double vout_min_v, double vout_max_v);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void sendRawCommand(const QString& cmd);
    Q_INVOKABLE void setEmulating(bool on);

signals:
    void statusChanged();
    void activeChanged();
    void connectedChanged();
    void emulatingChanged();
    void logReceived(QString msg);
    void pointsUpdated(double t, double v, double i_ua);

private slots:
    void processStep(const TelemetryFrame& frame);
    void onConnectionLost();

private:
    void doConnect();
    void tryReconnect();

    EmDeviceCommunicator m_comm;
    BatteryEngine m_engine;
    QString  m_port;
    double   m_consumed_ah     = 0.0;
    double   m_modelV          = 0.0;
    double   m_last_current_ua = 0.0;
    uint32_t m_last_ts = 0;
    bool     m_active     = false;
    bool     m_connected  = false;
    bool     m_emulating  = false;
};
