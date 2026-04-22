#include "emdevicecommunicator.h"

EmDeviceCommunicator::EmDeviceCommunicator(QObject *parent) : QObject(parent) {}

bool EmDeviceCommunicator::openDevice(const QString& portName, int baudRate) {
    m_serial.setPortName(portName);
    m_serial.setBaudRate(baudRate);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);
    if (m_serial.open(QIODevice::ReadWrite)) {
        connect(&m_serial, &QSerialPort::readyRead, this, &EmDeviceCommunicator::onReadyRead);
        return true;
    }
    return false;
}

void EmDeviceCommunicator::sendPowerState(bool on) {
    m_serial.write(on ? "power on\n" : "power off\n");
    m_serial.flush();
}

void EmDeviceCommunicator::sendTargetVoltage(uint32_t voltage_mv) {
    if (m_serial.isOpen()) {
        m_serial.write(QString("vout %1\n").arg(voltage_mv).toUtf8());
        m_serial.flush();
    }
}

void EmDeviceCommunicator::onReadyRead() {
    m_rxBuffer.append(m_serial.readAll());
    while (m_rxBuffer.contains('\n')) {
        int pos = m_rxBuffer.indexOf('\n');
        QByteArray line = m_rxBuffer.left(pos).trimmed();
        m_rxBuffer.remove(0, pos + 1);
        if (!line.isEmpty())
            processLine(line);
    }
}

// Firmware telemetry format (one tag per line):
//   [SEC] <seconds>  [VOL] <mV>  [CUR] <µA.frac>  [AHR] <µAh>  [WHR] <µWh>
void EmDeviceCommunicator::processLine(const QByteArray& line) {
    if (line.startsWith("[INF]") || line.startsWith("[ERR]") || line.startsWith("[VER]")) {
        emit logMessage(QString::fromUtf8(line));
        return;
    }
    if (line.startsWith("[VDD]"))
        return;
    if (line.length() < 6 || line[0] != '[')
        return;

    const QByteArray tag   = line.left(5);
    const QByteArray value = line.mid(6);
    bool ok = false;

    if (tag == "[SEC]") {
        m_frame.timestamp_ms = value.toULong(&ok) * 1000;
        m_fields = 1;
    } else if (tag == "[VOL]") {
        m_frame.voltage_mv = value.toDouble(&ok);
        if (ok) m_fields++;
    } else if (tag == "[CUR]") {
        m_frame.current_ma = value.toDouble(&ok) / 1000.0;   // µA → mA
        if (ok) m_fields++;
    } else if (tag == "[AHR]") {
        ok = true;
        m_fields++;
    } else if (tag == "[WHR]") {
        ok = true;
        m_fields++;
    }

    if (m_fields >= 5) {
        emit telemetryReceived(m_frame);
        m_fields = 0;
    }
}
