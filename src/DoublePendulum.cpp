#include "DoublePendulum.h"
#include <cmath>

DoublePendulum::DoublePendulum(double mass1, double length1, double mass2, double length2)
    : m_mass1(mass1)
    , m_length1(length1)
    , m_mass2(mass2)
    , m_length2(length2)
    , m_angle1(0.1)
    , m_angle2(0.1)
    , m_angularVelocity1(0.0)
    , m_angularVelocity2(0.0)
{
}

void DoublePendulum::update(double dt, double cartAcceleration)
{
    double angAccel1, angAccel2;
    computeAngularAccelerations(m_angle1, m_angle2, 
                               m_angularVelocity1, m_angularVelocity2,
                               cartAcceleration,
                               angAccel1, angAccel2);
    
    m_angularVelocity1 += angAccel1 * dt;
    m_angularVelocity2 += angAccel2 * dt;
    
    m_angle1 += m_angularVelocity1 * dt;
    m_angle2 += m_angularVelocity2 * dt;
    
    m_angle1 = normalizeAngle(m_angle1);
    m_angle2 = normalizeAngle(m_angle2);
}

void DoublePendulum::reset()
{
    m_angle1 = 0.1;
    m_angle2 = 0.1;
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
    const double g = GRAVITY;
    const double L1 = m_length1;
    const double L2 = m_length2;
    const double m1 = m_mass1;
    const double m2 = m_mass2;
    
    double dtheta = theta2 - theta1;
    
    double num1 = -g * std::sin(theta1) - cartAccel * std::cos(theta1);
    num1 += m2 * L2 * omega2 * omega2 * std::sin(dtheta) / (m1 + m2);
    alpha1 = num1 / L1;
    
    double num2 = g * (std::sin(theta1) - std::sin(theta2));
    num2 += cartAccel * (std::cos(theta1) - std::cos(theta2));
    num2 -= L1 * omega1 * omega1 * std::sin(dtheta);
    alpha2 = num2 / L2;
}

double DoublePendulum::normalizeAngle(double angle)
{
    const double PI = 3.14159265358979323846;
    while (angle > PI) angle -= 2.0 * PI;
    while (angle < -PI) angle += 2.0 * PI;
    return angle;
}