#include "Cart.h"
#include "ODESolver.h"
#include <algorithm>
#include <cmath>

Cart::Cart(double mass, double railLength)
    : m_mass(mass)
    , m_railLength(railLength)
    , m_position(0.0)
    , m_velocity(0.0)
    , m_width(WIDTH)
    , m_height(HEIGHT)
{
}

double Cart::update(double dt, double appliedAcceleration, double friction, double gravity)
{
    // Note: gravity parameter is here for consistency but doesn't affect horizontal cart motion
    // Use Dormand-Prince RK8 solver for smooth cart motion
    static ODESolver solver;

    // State vector: [position, velocity]
    std::vector<double> state = { m_position, m_velocity };

    // Derivative function
    auto derivFunc = [appliedAcceleration, friction](double t, const std::vector<double>& s) -> std::vector<double> {
        double pos = s[0];
        double vel = s[1];

        // Acceleration = applied - friction
        double frictionAccel = -friction * vel;
        double totalAccel = appliedAcceleration + frictionAccel;

        return { vel, totalAccel };
        };

    // Take one step
    solver.stepFixed(0.0, state, derivFunc, dt);

    // Update state
    m_position = state[0];
    m_velocity = state[1];

    // Clamp position to rail bounds
    double halfRail = m_railLength / 2.0;
    bool blocked = false;
    if (m_wrapEnabled) {
        // Wrap position into [-halfRail, halfRail]
        while (m_position < -halfRail) m_position += m_railLength;
        while (m_position > halfRail) m_position -= m_railLength;
        // wrapping does not block
    }
    else {
        if (m_position < -halfRail) {
            m_position = -halfRail;
            m_velocity = 0.0;
            blocked = true;
        }
        if (m_position > halfRail) {
            m_position = halfRail;
            m_velocity = 0.0;
            blocked = true;
        }
    }

    // If cart is blocked and the input acceleration is trying to push it
    // further out of bounds, we should consider the effective applied
    // acceleration as zero for the purposes of the pendulum (reaction
    // force prevents cart motion). Return effective applied acceleration.
    if (blocked) {
        if ((m_position <= -halfRail && appliedAcceleration < 0.0) ||
            (m_position >= halfRail && appliedAcceleration > 0.0)) {
            return 0.0;
        }
    }

    return appliedAcceleration;
}

void Cart::reset()
{
    m_position = 0.0;
    m_velocity = 0.0;
}