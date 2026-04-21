#pragma once
#include <QObject>
#include "emdevicecommunicator.h"
#include "batteryengine.h"

class EmulationController : public QObject {
    Q_OBJECT
public:
    explicit EmulationController(QObject *parent = nullptr);
    bool start(const QString& port, const BatteryModelParams& params);

signals:
    void statusUpdated(double vModel, double soc);

private slots:
    void processStep(const TelemetryFrame& frame);

private:
    EmDeviceCommunicator m_comm;
    BatteryEngine m_engine;
    double m_consumed_ah = 0.0;
    uint32_t m_last_ts = 0;
};
