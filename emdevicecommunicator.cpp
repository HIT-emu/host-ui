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
        connect(&m_serial, &QSerialPort::readyRead,
                this, &EmDeviceCommunicator::onReadyRead);
        connect(&m_serial, &QSerialPort::errorOccurred,
                this, [this](QSerialPort::SerialPortError e) {
            if (e == QSerialPort::ResourceError) {
                emit logMessage("[WARN] Port lost, reconnecting...");
                m_serial.close();
                emit connectionLost();
            } else if (e != QSerialPort::NoError) {
                emit logMessage(QString("[ERR] Serial error %1: %2")
                                .arg(e).arg(m_serial.errorString()));
            }
        });
        return true;
    }
    return false;
}

void EmDeviceCommunicator::closeDevice() {
    if (m_serial.isOpen())
        m_serial.close();
}

void EmDeviceCommunicator::sendPowerState(bool on) {
    m_serial.write(on ? "power on\n" : "power off\n");
    m_serial.flush();
}

void EmDeviceCommunicator::setVoutRange(uint32_t min_mv, uint32_t max_mv) {
    m_vout_min_mv = min_mv;
    m_vout_max_mv = max_mv;
}

void EmDeviceCommunicator::sendTargetVoltage(uint32_t voltage_mv) {
    if (!m_serial.isOpen()) return;
    // Firmware PWM period = 480. Inverted: duty 480 = min voltage, duty 0 = max voltage.
    const uint32_t period = 480;
    uint32_t mv   = qBound(m_vout_min_mv, voltage_mv, m_vout_max_mv);
    uint32_t duty = period * (m_vout_max_mv - mv) / (m_vout_max_mv - m_vout_min_mv);
    m_serial.write(QString("vout %1\n").arg(duty).toUtf8());
    m_serial.flush();
}

void EmDeviceCommunicator::sendRawString(const QString& data) {
    if (m_serial.isOpen()) {
        m_serial.write(data.toUtf8());
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

void EmDeviceCommunicator::processLine(const QByteArray& line) {
    emit logMessage(QString::fromUtf8(line));

    if (line.length() < 6 || line[0] != '[')
        return;

    const QByteArray tag   = line.left(5);
    const QByteArray value = line.mid(6).trimmed();
    bool ok = false;

    if (tag == "[SEC]") {
        m_frame  = {};
        m_fields = 0;
        uint32_t sec = value.toULong(&ok);
        if (ok) { m_frame.timestamp_ms = sec * 1000; m_fields = 1; }
    } else if (tag == "[VOL]") {
        double vol = value.toDouble(&ok);
        if (ok) { m_frame.voltage_mv = vol; m_fields++; }
    } else if (tag == "[CUR]") {
        double cur = value.toDouble(&ok);
        if (ok) { m_frame.current_ma = cur / 1000.0; m_fields++; }
    }

    if (m_fields >= 3) {
        emit telemetryReceived(m_frame);
        m_fields = 0;
    }
}
