#pragma once

#include <algorithm>
#include <cmath>

struct PendulumModel {
    static constexpr double g = 9.81;
    static constexpr double pi = 3.14159265358979323846;

    double length = 1.20;
    double initialAngle = 25.0 * pi / 180.0;
    double theta = initialAngle;
    double omega = 0.0;
    double elapsed = 0.0;

    void reset() { theta = initialAngle; omega = 0.0; elapsed = 0.0; }
    void setLength(double value) { length = std::clamp(value, 0.50, 2.40); reset(); }
    void setInitialAngleDegrees(double degrees) {
        initialAngle = std::clamp(degrees, 5.0, 70.0) * pi / 180.0;
        reset();
    }
    double initialAngleDegrees() const { return initialAngle * 180.0 / pi; }
    double currentAngleDegrees() const { return theta * 180.0 / pi; }
    double smallAnglePeriod() const { return 2.0 * pi * std::sqrt(length / g); }
    double acceleration(double a) const { return -(g / length) * std::sin(a); }

    void step(double dt) {
        if (dt <= 0.0 || !std::isfinite(dt)) return;
        const auto dTheta = [](double velocity) { return velocity; };
        const double k1t = dTheta(omega);
        const double k1w = acceleration(theta);
        const double k2t = dTheta(omega + 0.5 * dt * k1w);
        const double k2w = acceleration(theta + 0.5 * dt * k1t);
        const double k3t = dTheta(omega + 0.5 * dt * k2w);
        const double k3w = acceleration(theta + 0.5 * dt * k2t);
        const double k4t = dTheta(omega + dt * k3w);
        const double k4w = acceleration(theta + dt * k3t);
        theta += (dt / 6.0) * (k1t + 2.0 * k2t + 2.0 * k3t + k4t);
        omega += (dt / 6.0) * (k1w + 2.0 * k2w + 2.0 * k3w + k4w);
        elapsed += dt;
    }
};

struct PeriodMeasurement {
    double measuredPeriod = 0.0;
    int samples = 0;
};

inline PeriodMeasurement measurePeriod(double length, double angleDegrees, double duration = 30.0) {
    PendulumModel model;
    model.setLength(length);
    model.setInitialAngleDegrees(angleDegrees);
    constexpr double dt = 1.0 / 240.0;
    double previousCrossing = -1.0;
    double totalPeriod = 0.0;
    int periods = 0;
    for (int i = 0; i < static_cast<int>(duration / dt); ++i) {
        const double oldAngle = model.theta;
        const double oldTime = model.elapsed;
        model.step(dt);
        if (oldAngle > 0.0 && model.theta <= 0.0 && model.omega < 0.0) {
            const double weight = oldAngle / (oldAngle - model.theta);
            const double crossing = oldTime + dt * weight;
            if (previousCrossing >= 0.0) {
                totalPeriod += crossing - previousCrossing;
                ++periods;
            }
            previousCrossing = crossing;
        }
    }
    return {periods > 0 ? totalPeriod / periods : 0.0, periods};
}
