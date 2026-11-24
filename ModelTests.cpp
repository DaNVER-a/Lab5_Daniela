#include "PhysicsModel.h"
#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    PendulumModel m;
    m.setLength(1.0);
    m.setInitialAngleDegrees(15.0);
    const double before = m.theta;
    m.step(1.0 / 240.0);
    assert(m.theta < before);
    assert(m.omega < 0.0);
    m.setLength(20.0);
    assert(std::abs(m.length - 2.4) < 0.001);
    m.setInitialAngleDegrees(-100.0);
    assert(std::abs(m.initialAngleDegrees() - 5.0) < 0.001);
    m.setLength(1.0);
    m.setInitialAngleDegrees(15.0);
    const auto result = measurePeriod(1.0, 15.0);
    assert(result.samples >= 5);
    assert(std::abs(result.measuredPeriod - m.smallAnglePeriod()) < 0.025);
    m.reset();
    assert(std::abs(m.omega) < 0.000001);
    assert(std::abs(m.elapsed) < 0.000001);
    std::cout << "Pendulum model tests: PASS\n";
}
