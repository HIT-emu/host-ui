#pragma once
#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include "TelemetryFrame.h"

class EmDeviceCommunicator : public QObject {
    Q_OBJECT

public:
    explicit EmDeviceCommunicator(QObject *parent = nullptr);
    ~EmDeviceCommunicator();

    bool openDevice(const QString &portName, int baudRate = 115200);
    void closeDevice();

public slots:
    void sendPowerState(bool state);
    void sendTelemetryPeriod(uint16_t ms);
    void sendTargetVoltage(uint32_t voltage_mv);

signals:
    void telemetryReceived(const TelemetryFrame &frame);
    void deviceLogEmitted(const QString &message);
    void connectionError(const QString &errorDetails);
    void rawLineReceived(const QString &line);   // every line before parsing

private slots:
    void onReadyRead();

private:
    void processRawLine(const QByteArray &line);

    QSerialPort m_serial;
    QByteArray m_rxBuffer;
    TelemetryFrame m_currentFrame = {};
    int m_fieldsReceived = 0;
};
