#include "SinglePendulum.h"
#include "ODESolver.h"
#include <cmath>

SinglePendulum::SinglePendulum(double mass, double length)
    : m_mass(mass)
    , m_length(length)
    , m_initialAngle(0.0) // default: hanging down
    , m_angle(m_initialAngle)
    , m_angularVelocity(0.0)
{
}

void SinglePendulum::update(double dt, double cartAcceleration)
{
    // Use Dormand-Prince RK8 solver for high accuracy
    static ODESolver solver;

    // State vector: [angle, angular_velocity]
    std::vector<double> state = { m_angle, m_angularVelocity };

    // Derivative function
    auto derivFunc = [this, cartAcceleration](double t, const std::vector<double>& s) -> std::vector<double> {
        double angle = s[0];
        double angVel = s[1];
        double angAccel = this->computeAngularAcceleration(angle, angVel, cartAcceleration);
        return { angVel, angAccel };
        };

    // Take one step with fixed dt for consistent frame timing
    solver.stepFixed(0.0, state, derivFunc, dt);

    // Update state
    m_angle = state[0];
    m_angularVelocity = state[1];

    // Keep angle in [-pi, pi] range
    m_angle = normalizeAngle(m_angle);

    // Clamp very small residuals to exact zero to avoid tiny oscillations
    // caused by numerical integration / floating-point noise. This makes
    // the pendulum reach a true motionless state when it has effectively
    // settled.
    const double ANGLE_EPS = 1e-6; // ~0.000057 deg
    const double VEL_EPS = 1e-6;
    if (std::abs(m_angularVelocity) < VEL_EPS && std::abs(m_angle) < ANGLE_EPS) {
        m_angularVelocity = 0.0;
        m_angle = 0.0;
    }
}

void SinglePendulum::reset()
{
    // Reset to configured initial angle and clear velocity
    m_angle = m_initialAngle;
    m_angularVelocity = 0.0;
}

double SinglePendulum::getAngle(int index) const
{
    return (index == 0) ? m_angle : 0.0;
}

double SinglePendulum::getAngularVelocity(int index) const
{
    return (index == 0) ? m_angularVelocity : 0.0;
}

double SinglePendulum::computeAngularAcceleration(double angle, double angularVel, double cartAccel)
{
    // Equation of motion for inverted pendulum on cart:
    // θ̈ = (g·sin(θ) - a·cos(θ)) / L
    // where:
    //   θ  = pendulum angle (0 = up)
    //   g  = gravity
    //   a  = cart acceleration
    //   L  = pendulum length

    // Use downward-zero convention (θ = 0 is hanging down). With this
    // convention the gravity term is -g*sin(θ). Cart acceleration couples
    // with -cos(θ). Include a simple viscous damping term proportional to angular velocity.
    double numerator = -m_gravity * std::sin(angle) - cartAccel * std::cos(angle);
    double angularAccel = (numerator - m_damping * angularVel) / m_length;

    return angularAccel;
}

double SinglePendulum::normalizeAngle(double angle)
{
    // Wrap angle to [-pi, pi]
    const double PI = 3.14159265358979323846;
    while (angle > PI) angle -= 2.0 * PI;
    while (angle < -PI) angle += 2.0 * PI;
    return angle;
}