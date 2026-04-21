#include "EmDeviceCommunicator.h"
#include <QDebug>

EmDeviceCommunicator::EmDeviceCommunicator(QObject *parent)
    : QObject(parent)
{
    connect(&m_serial, &QSerialPort::readyRead, this, &EmDeviceCommunicator::onReadyRead);
    connect(&m_serial, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError error) {
        if (error != QSerialPort::NoError)
            emit connectionError(m_serial.errorString());
    });
}

EmDeviceCommunicator::~EmDeviceCommunicator()
{
    closeDevice();
}

bool EmDeviceCommunicator::openDevice(const QString &portName, int baudRate)
{
    m_serial.setPortName(portName);
    m_serial.setBaudRate(baudRate);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serial.open(QIODevice::ReadWrite)) {
        emit connectionError(m_serial.errorString());
        return false;
    }
    return true;
}

void EmDeviceCommunicator::closeDevice()
{
    if (m_serial.isOpen())
        m_serial.close();
}

void EmDeviceCommunicator::sendPowerState(bool state)
{
    m_serial.write(state ? "power on\n" : "power off\n");
    m_serial.flush();
}

void EmDeviceCommunicator::sendTelemetryPeriod(uint16_t ms)
{
    m_serial.write(QByteArray("period ") + QByteArray::number(ms) + "\n");
    m_serial.flush();
}

void EmDeviceCommunicator::sendTargetVoltage(uint32_t voltage_mv)
{
    m_serial.write(QByteArray("vout ") + QByteArray::number(voltage_mv) + "\n");
    m_serial.flush();
}

void EmDeviceCommunicator::onReadyRead()
{
    m_rxBuffer += m_serial.readAll();

    int idx;
    while ((idx = m_rxBuffer.indexOf('\n')) != -1) {
        QByteArray line = m_rxBuffer.left(idx).trimmed();
        m_rxBuffer.remove(0, idx + 1);
        if (!line.isEmpty())
            processRawLine(line);
    }
}

// Firmware output format (one tag per line):
//   [SEC] <seconds>
//   [VOL] <millivolts>
//   [CUR] <microamps as X.Y>
//   [AHR] <microamp-hours>
//   [WHR] <microwatt-hours>
// Log lines: [INF]/[ERR]/[VER] — forwarded as-is.
// A complete frame is emitted once all 5 tags have been received.
void EmDeviceCommunicator::processRawLine(const QByteArray &line)
{
    emit rawLineReceived(QString::fromUtf8(line));

    if (line.startsWith("[INF]") || line.startsWith("[ERR]") || line.startsWith("[VER]")) {
        emit deviceLogEmitted(QString::fromUtf8(line));
        return;
    }

    if (line.length() < 6 || line[0] != '[')
        return;

    // [VDD] is board supply voltage — informational only, not part of telemetry frame
    if (line.startsWith("[VDD]"))
        return;

    const QByteArray tag   = line.left(5);   // e.g. "[SEC]"
    const QByteArray value = line.mid(6);    // everything after "[TAG] "

    bool ok = false;

    if (tag == "[SEC]") {
        m_currentFrame.timestamp_ms = value.toULong(&ok) * 1000;
        m_fieldsReceived = 1;
    } else if (tag == "[VOL]") {
        m_currentFrame.voltage_mv = value.toDouble(&ok);
        if (ok) m_fieldsReceived++;
    } else if (tag == "[CUR]") {
        // firmware outputs µA as "integer.fraction" — convert to mA
        m_currentFrame.current_ma = value.toDouble(&ok) / 1000.0;
        if (ok) m_fieldsReceived++;
    } else if (tag == "[AHR]") {
        // µAh → mAh
        m_currentFrame.consumed_mah = value.toDouble(&ok) / 1000.0;
        if (ok) m_fieldsReceived++;
    } else if (tag == "[WHR]") {
        ok = true;
        m_fieldsReceived++;  // WHR not stored in frame but marks end of block
    }

    if (!ok) {
        emit deviceLogEmitted(QString("[WARN] malformed line: %1").arg(QString::fromUtf8(line)));
        return;
    }

    if (m_fieldsReceived >= 5) {
        emit telemetryReceived(m_currentFrame);
        m_fieldsReceived = 0;
    }
}
