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
    // Lagrangian-derived equations for a double pendulum with a moving support (cart)
    // Coordinates: theta1, theta2 measured from downward vertical (theta=0 is hanging down)
    // Using positions:
    //  x1 = x_cart + L1*sin(theta1), y1 = -L1*cos(theta1)
    //  x2 = x1 + L2*sin(theta2), y2 = y1 - L2*cos(theta2)
    // After deriving Euler-Lagrange equations and isolating theta'' terms we obtain
    // a 2x2 linear system: A * [theta1_dd; theta2_dd] = RHS

    const double g = m_gravity;
    const double L1 = m_length1;
    const double L2 = m_length2;
    const double m1 = m_mass1;
    const double m2 = m_mass2;

    double dtheta = theta1 - theta2; // note sign used in cos/sin below
    double c = std::cos(dtheta);
    double s = std::sin(dtheta);

    // Mass-inertia matrix coefficients
    double A11 = (m1 + m2) * L1;
    double A12 = m2 * L2 * c;
    double A21 = m2 * L1 * c;
    double A22 = m2 * L2;

    // Right-hand side (move all non-acceleration terms to RHS)
    double RHS1 = - (m1 + m2) * g * std::sin(theta1)
                  - m2 * L2 * omega2 * omega2 * s
                  - (m1 + m2) * cartAccel * std::cos(theta1)
                  - m_damping * omega1;

    double RHS2 = m2 * L1 * omega1 * omega1 * s
                  - m2 * g * std::sin(theta2)
                  - m2 * cartAccel * std::cos(theta2)
                  - m_damping * omega2;

    // Solve 2x2 linear system
    double det = A11 * A22 - A12 * A21;
    if (std::abs(det) < 1e-12) {
        // Ill-conditioned; fall back to simple decoupled estimates
        alpha1 = RHS1 / (A11 > 1e-12 ? A11 : 1.0);
        alpha2 = RHS2 / (A22 > 1e-12 ? A22 : 1.0);
        return;
    }

    alpha1 = (RHS1 * A22 - A12 * RHS2) / det;
    alpha2 = (A11 * RHS2 - RHS1 * A21) / det;
}

double DoublePendulum::normalizeAngle(double angle)
{
    const double PI = 3.14159265358979323846;
    while (angle > PI) angle -= 2.0 * PI;
    while (angle < -PI) angle += 2.0 * PI;
    return angle;
}

double DoublePendulum::getKineticEnergy(double cartVelocity) const
{
    // Positions (relative to cart pivot at y=0):
    // x1 = x_cart + L1*sin(theta1)
    // y1 = -L1*cos(theta1)
    // x2 = x1 + L2*sin(theta2)
    // y2 = y1 - L2*cos(theta2)
    // Velocities computed by differentiating positions

    double v_cart = cartVelocity;
    double t1 = m_angle1;
    double t2 = m_angle2;
    double w1 = m_angularVelocity1;
    double w2 = m_angularVelocity2;

    double x1dot = v_cart + m_length1 * std::cos(t1) * w1;
    double y1dot = m_length1 * std::sin(t1) * w1;

    double x2dot = v_cart + m_length1 * std::cos(t1) * w1 + m_length2 * std::cos(t2) * w2;
    double y2dot = m_length1 * std::sin(t1) * w1 + m_length2 * std::sin(t2) * w2;

    double ke1 = 0.5 * m_mass1 * (x1dot * x1dot + y1dot * y1dot);
    double ke2 = 0.5 * m_mass2 * (x2dot * x2dot + y2dot * y2dot);

    return ke1 + ke2;
}

double DoublePendulum::getPotentialEnergy() const
{
    // PE relative to both pendulums hanging down (theta = 0):
    // m1 * g * L1 * (1 - cos(theta1))
    // m2 * g * (L1*(1 - cos(theta1)) + L2*(1 - cos(theta2)))
    double pe1 = m_mass1 * m_gravity * m_length1 * (1.0 - std::cos(m_angle1));
    double pe2 = m_mass2 * m_gravity * (m_length1 * (1.0 - std::cos(m_angle1)) + m_length2 * (1.0 - std::cos(m_angle2)));
    return pe1 + pe2;
}