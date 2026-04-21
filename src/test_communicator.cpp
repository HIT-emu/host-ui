#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include "EmDeviceCommunicator.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    if (argc < 2) {
        qCritical() << "Usage: test_communicator <port>";
        qCritical() << "Example: test_communicator /tmp/ttyV0";
        return 1;
    }

    EmDeviceCommunicator comm;

    QObject::connect(&comm, &EmDeviceCommunicator::telemetryReceived,
                     [](const TelemetryFrame &f) {
        qInfo() << "[TELEMETRY]"
                << "T=" << f.timestamp_ms << "ms"
                << "V=" << f.voltage_mv   << "mV"
                << "I=" << f.current_ma   << "mA"
                << "C=" << f.consumed_mah << "mAh";
    });

    QObject::connect(&comm, &EmDeviceCommunicator::deviceLogEmitted,
                     [](const QString &msg) {
        qInfo() << "[LOG]" << msg;
    });

    QObject::connect(&comm, &EmDeviceCommunicator::connectionError,
                     [&app](const QString &err) {
        qCritical() << "[ERROR]" << err;
        app.exit(1);
    });

    if (!comm.openDevice(argv[1])) {
        qCritical() << "Failed to open" << argv[1];
        return 1;
    }

    qInfo() << "Listening on" << argv[1] << "...";
    return app.exec();
}
