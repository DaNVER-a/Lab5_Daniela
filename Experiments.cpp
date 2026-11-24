#include "PhysicsModel.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    const std::string filename = argc > 1 ? argv[1] : "experiment_results.csv";
    std::ofstream csv(filename);
    if (!csv) {
        std::cerr << "Cannot write CSV: " << filename << '\n';
        return 1;
    }
    csv << "length_m,angle_deg,period_numeric_s,period_small_angle_s,relative_error_percent,samples\n";
    std::cout << "Pendulum experiments (RK4, dt=1/240 s, 30 s)\n";
    std::cout << std::fixed << std::setprecision(4);
    for (double length : std::vector<double>{0.6, 1.0, 1.5, 2.0}) {
        for (double angle : std::vector<double>{15.0, 45.0}) {
            PendulumModel m;
            m.setLength(length);
            m.setInitialAngleDegrees(angle);
            const PeriodMeasurement measured = measurePeriod(length, angle);
            const double approx = m.smallAnglePeriod();
            const double error = 100.0 * (measured.measuredPeriod - approx) / approx;
            std::cout << "L=" << length << " m | start=" << angle
                      << " deg | numeric=" << measured.measuredPeriod
                      << " s | small-angle=" << approx << " s | diff=" << error << "%\n";
            csv << std::fixed << std::setprecision(6) << length << ',' << angle << ','
                << measured.measuredPeriod << ',' << approx << ',' << error << ','
                << measured.samples << '\n';
        }
    }
    std::cout << "Saved CSV: " << filename << '\n';
    return 0;
}
