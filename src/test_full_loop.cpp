#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "EmulationController.h"

static QFile   g_logFile;
static QTextStream g_log;

static void logLine(const QString &line)
{
    const QString ts = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    const QString entry = QString("[%1] %2").arg(ts, line);
    qInfo().noquote() << entry;
    if (g_logFile.isOpen())
        g_log << entry << "\n";
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    if (argc < 2) {
        qCritical() << "Usage: test_full_loop <port>";
        qCritical() << "Example: test_full_loop /dev/ttyACM1";
        return 1;
    }

    // Create logs/ directory next to the binary
    QDir logsDir(QCoreApplication::applicationDirPath() + "/logs");
    logsDir.mkpath(".");

    const QString logPath = logsDir.filePath(
        QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + "_session.log");
    g_logFile.setFileName(logPath);
    if (g_logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        g_log.setDevice(&g_logFile);
        qInfo().noquote() << "Logging to:" << logPath;
    } else {
        qWarning() << "Could not open log file:" << logPath;
    }

    EmulationController ctl;

    // Log every raw line from the board
    QObject::connect(&ctl.communicator(), &EmDeviceCommunicator::rawLineReceived,
                     [](const QString &line) {
        logLine("[RAW] " + line);
    });

    QObject::connect(&ctl.communicator(), &EmDeviceCommunicator::deviceLogEmitted,
                     [](const QString &msg) {
        logLine("[FW]  " + msg);
    });

    QObject::connect(&ctl, &EmulationController::stateUpdated,
                     [](double target_mv, const TelemetryFrame &f) {
        logLine(QString("[LOOP] t=%1ms  V_meas=%2mV  I=%3mA  C=%4mAh  -> V_target=%5mV")
                .arg(f.timestamp_ms)
                .arg(f.voltage_mv,   0, 'f', 1)
                .arg(f.current_ma,   0, 'f', 2)
                .arg(f.consumed_mah, 0, 'f', 3)
                .arg(target_mv,      0, 'f', 1));
    });

    QObject::connect(&ctl, &EmulationController::emulationFinished,
                     [&app](const QString &reason) {
        logLine("[DONE] " + reason);
        app.exit(0);
    });

    if (!ctl.connectHardware(argv[1])) {
        qCritical() << "Failed to open" << argv[1];
        return 1;
    }

    ctl.configureModel(BatteryModelParams::liIonDefault());
    ctl.setCutoffVoltage(2800.0);

    ctl.startEmulation();
    logLine("[INFO] Emulation started on " + QString(argv[1]));

    return app.exec();
}
