#include <QCoreApplication>
#include <QDebug>
#include "emulationcontroller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    EmulationController controller;

    // Настраиваем параметры модели (те, что были в твоем battery_fixed.c)
    // Li-ion coefficients from battery_emulation_report.md
    BatteryModelParams p;
    p.E0 = 3.781272;
    p.k1 = 0.093608;
    p.k2 = 0.058901;
    p.A  = 0.563978;
    p.B  = 7.504662;
    p.capacity_ah = 0.5087;

    QObject::connect(&controller, &EmulationController::statusUpdated,
                     [](double vModel, double soc){
                         qDebug() << "Model Voltage:" << vModel << "V | Consumed:" << soc << "Ah";
                     });

    if (!controller.start("/dev/ttyACM1", p)) {
        qDebug() << "Could not open port!";
        return -1;
    }

    return a.exec();
}
