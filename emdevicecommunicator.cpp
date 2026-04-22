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

void EmDeviceCommunicator::closeDevice() {
    if (m_serial.isOpen()) {
        m_serial.close();
    }
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
    const QByteArray value = line.mid(6).trimmed();
    bool ok = false;

    if (tag == "[SEC]") {
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
        // Формируем текстовое представление структуры для лога
        QString logEntry = QString("DATA FRAME: Time=%1ms, V=%2mV, I=%3mA")
                               .arg(m_frame.timestamp_ms)
                               .arg(m_frame.voltage_mv, 0, 'f', 2)
                               .arg(m_frame.current_ma, 0, 'f', 2);

        emit logMessage(logEntry); // Отправляем в лог
        emit telemetryReceived(m_frame); // Отправляем на расчет
        m_fields = 0;
    }
}

void EmDeviceCommunicator::sendRawString(const QString& data) {
    if (m_serial.isOpen()) {
        m_serial.write(data.toUtf8() + "\n"); // Важно добавить \n, чтобы МК понял конец команды
        m_serial.flush();
    }
}
