#include <QCoreApplication>
#include <QDebug>
#include "emulationcontroller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    EmulationController controller;

    // Настраиваем параметры модели (те, что были в твоем battery_fixed.c)
    BatteryModelParams p;
    p.E0 = 12.8;
    p.k1 = 0.1;
    p.k2 = 0.05;
    p.A = 0.2;
    p.B = 1.5;
    p.capacity_ah = 10.0;

    // Подписываемся на обновление данных, чтобы видеть результат в консоли
    QObject::connect(&controller, &EmulationController::statusUpdated,
                     [](double vModel, double soc){
                         qDebug() << "Model Voltage:" << vModel << "V | Consumed:" << soc << "Ah";
                     });

    // Запускаем. Путь к порту возьми из вывода Python-скрипта (например /dev/ttys001)
    if (!controller.start("/dev/ttys002", p)) {
        qDebug() << "Could not open port!";
        return -1;
    }

    return a.exec();
}
