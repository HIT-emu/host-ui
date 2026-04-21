#include "emdevicecommunicator.h"

EmDeviceCommunicator::EmDeviceCommunicator(QObject *parent) : QObject(parent) {}

bool EmDeviceCommunicator::openDevice(const QString& portName, int baudRate) {
    m_serial.setPortName(portName);
    m_serial.setBaudRate(baudRate);
    if (m_serial.open(QIODevice::ReadWrite)) {
        connect(&m_serial, &QSerialPort::readyRead, this, &EmDeviceCommunicator::onReadyRead);
        return true;
    }
    return false;
}

void EmDeviceCommunicator::onReadyRead() {
    m_rxBuffer.append(m_serial.readAll());
    while (m_rxBuffer.contains('\n')) {
        int pos = m_rxBuffer.indexOf('\n');
        QByteArray line = m_rxBuffer.left(pos).trimmed();
        m_rxBuffer.remove(0, pos + 1);

        // Парсинг строки вида "T:1234 V:12500 I:500"
        TelemetryFrame frame;
        float v, i;
        if (sscanf(line.data(), "T:%u V:%f I:%f", &frame.timestamp_ms, &v, &i) == 3) {
            frame.voltage_mv = v;
            frame.current_ma = i;
            emit telemetryReceived(frame);
        }
    }
}

void EmDeviceCommunicator::sendTargetVoltage(uint32_t voltage_mv) {
    if (m_serial.isOpen()) {
        m_serial.write(QString("SETV %1\n").arg(voltage_mv).toUtf8());
    }
}
