#include "DoublePendulum.h"
#include "ODESolver.h"
#include <cmath>

DoublePendulum::DoublePendulum(double mass1, double length1, double mass2, double length2)
    : m_mass1(mass1)
    , m_length1(length1)
    , m_mass2(mass2)
    , m_length2(length2)
    , m_initialAngle1(0.0)
    , m_initialAngle2(0.0)
    , m_angle1(m_initialAngle1)
    , m_angle2(m_initialAngle2)
    , m_angularVelocity1(0.0)
    , m_angularVelocity2(0.0)
{
}

void DoublePendulum::update(double dt, double cartAcceleration)
{
    // Use Dormand-Prince RK8 solver
    static ODESolver solver;

    // State vector: [angle1, angVel1, angle2, angVel2]
    std::vector<double> state = { m_angle1, m_angularVelocity1, m_angle2, m_angularVelocity2 };

    // Derivative function
    auto derivFunc = [this, cartAcceleration](double t, const std::vector<double>& s) -> std::vector<double> {
        double theta1 = s[0];
        double omega1 = s[1];
        double theta2 = s[2];
        double omega2 = s[3];

        double alpha1, alpha2;
        this->computeAngularAccelerations(theta1, theta2, omega1, omega2,
            cartAcceleration, alpha1, alpha2);

        return { omega1, alpha1, omega2, alpha2 };
        };

    // Take one step with fixed dt
    solver.stepFixed(0.0, state, derivFunc, dt);

    // Update state
    m_angle1 = state[0];
    m_angularVelocity1 = state[1];
    m_angle2 = state[2];
    m_angularVelocity2 = state[3];

    // Normalize angles
    m_angle1 = normalizeAngle(m_angle1);
    m_angle2 = normalizeAngle(m_angle2);

    // Clamp very small residuals to exact zero to avoid tiny oscillations
    // caused by numerical integration / floating-point noise. If both
    // pendulums have effectively settled, snap them to zero for a
    // clear motionless state.
    const double ANGLE_EPS = 1e-6;
    const double VEL_EPS = 1e-6;
    if (std::abs(m_angularVelocity1) < VEL_EPS && std::abs(m_angularVelocity2) < VEL_EPS &&
        std::abs(m_angle1) < ANGLE_EPS && std::abs(m_angle2) < ANGLE_EPS) {
        m_angularVelocity1 = 0.0;
        m_angularVelocity2 = 0.0;
        m_angle1 = 0.0;
        m_angle2 = 0.0;
    }
}

void DoublePendulum::reset()
{
    m_angle1 = m_initialAngle1;
    m_angle2 = m_initialAngle2;
    m_angularVelocity1 = 0.0;
    m_angularVelocity2 = 0.0;
}

double DoublePendulum::getAngle(int index) const
{
    if (index == 0) return m_angle1;
    if (index == 1) return m_angle2;
    return 0.0;
}

double DoublePendulum::getAngularVelocity(int index) const
{
    if (index == 0) return m_angularVelocity1;
    if (index == 1) return m_angularVelocity2;
    return 0.0;
}

double DoublePendulum::getMass(int index) const
{
    return (index == 0) ? m_mass1 : m_mass2;
}

double DoublePendulum::getLength(int index) const
{
    return (index == 0) ? m_length1 : m_length2;
}

void DoublePendulum::setAngle(int index, double angle)
{
    if (index == 0) m_angle1 = angle;
    else if (index == 1) m_angle2 = angle;
}

void DoublePendulum::setAngularVelocity(int index, double vel)
{
    if (index == 0) m_angularVelocity1 = vel;
    else if (index == 1) m_angularVelocity2 = vel;
}

void DoublePendulum::computeAngularAccelerations(double theta1, double theta2,
    double omega1, double omega2,
    double cartAccel,
    double& alpha1, double& alpha2)
{
    // Simplified double pendulum on cart equations
    // These are approximations - we'll refine in Phase 2 with proper Lagrangian derivation

    const double g = m_gravity;
    const double L1 = m_length1;
    const double L2 = m_length2;
    const double m1 = m_mass1;
    const double m2 = m_mass2;

    // Angular difference
    double dtheta = theta2 - theta1;

    // Compute angular accelerations (simplified coupled equations)
    // Include simple viscous damping using m_damping for each pendulum.

    // First pendulum
    double num1 = -g * std::sin(theta1) - cartAccel * std::cos(theta1);
    num1 += m2 * L2 * omega2 * omega2 * std::sin(dtheta) / (m1 + m2);
    alpha1 = (num1 - m_damping * omega1) / L1;

    // Second pendulum (relative to first)
    double num2 = g * (std::sin(theta1) - std::sin(theta2));
    num2 += cartAccel * (std::cos(theta1) - std::cos(theta2));
    num2 -= L1 * omega1 * omega1 * std::sin(dtheta);
    alpha2 = (num2 - m_damping * omega2) / L2;
}

double DoublePendulum::normalizeAngle(double angle)
{
    const double PI = 3.14159265358979323846;
    while (angle > PI) angle -= 2.0 * PI;
    while (angle < -PI) angle += 2.0 * PI;
    return angle;
}