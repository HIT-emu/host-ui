#pragma once
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "TelemetryFrame.h"

class EmDeviceCommunicator : public QObject {
    Q_OBJECT
public:
    explicit EmDeviceCommunicator(QObject *parent = nullptr);
    bool openDevice(const QString& portName, int baudRate = 115200);
    void closeDevice();
    void setVoutRange(uint32_t min_mv, uint32_t max_mv);

public slots:
    void sendTargetVoltage(uint32_t voltage_mv);
    void sendPowerState(bool on);
    void sendRawString(const QString& data);

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
    uint32_t       m_vout_min_mv = 1330;
    uint32_t       m_vout_max_mv = 3230;
};
