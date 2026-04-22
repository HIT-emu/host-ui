#pragma once
#include <QObject>
#include <QSerialPort>
#include "TelemetryFrame.h"

class EmDeviceCommunicator : public QObject {
    Q_OBJECT
public:
    explicit EmDeviceCommunicator(QObject *parent = nullptr);
    bool openDevice(const QString& portName, int baudRate = 115200);

public slots:
    void sendTargetVoltage(uint32_t voltage_mv);
    void sendPowerState(bool on);

signals:
    void telemetryReceived(const TelemetryFrame& frame);
    void logMessage(const QString& msg);

private slots:
    void onReadyRead();

private:
    void processLine(const QByteArray& line);

    QSerialPort    m_serial;
    QByteArray     m_rxBuffer;
    TelemetryFrame m_frame = {};
    int            m_fields = 0;
};
