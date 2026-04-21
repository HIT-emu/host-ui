#include <cstdio>
#include "BatteryEngine.h"

// Simulate a 1C discharge (Q=0.5087Ah → I=508.7mA) for 60 minutes,
// printing voltage every 5 minutes.
int main()
{
    BatteryEngine eng;
    eng.setParameters(BatteryModelParams::liIonDefault());

    const double current_ma   = 508.7;   // 1C
    const double step_sec     = 10.0;
    const double print_every  = 300.0;   // 5 min

    double elapsed = 0.0;
    double next_print = 0.0;

    printf("%-8s %-10s %-10s\n", "t(min)", "V(mV)", "C(mAh)");
    printf("------------------------------------\n");

    while (!eng.isDepleted() && elapsed < 7200.0) {
        double v = eng.computeNextVoltage(current_ma, step_sec);

        if (elapsed >= next_print) {
            printf("%-8.1f %-10.1f %-10.1f\n",
                   elapsed / 60.0, v, eng.getConsumedCapacityMah());
            next_print += print_every;
        }

        elapsed += step_sec;
    }

    printf("\nDepleted: %s at %.1f min\n",
           eng.isDepleted() ? "yes" : "no", elapsed / 60.0);
    return 0;
}
