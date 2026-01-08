#include "SinglePendulum.h"
#include <cmath>

SinglePendulum::SinglePendulum(double mass, double length)
    : m_mass(mass)
    , m_length(length)
    , m_angle(0.5)
    , m_angularVelocity(0.0)
{
}

void SinglePendulum::update(double dt, double cartAcceleration)
{
    double angularAccel = computeAngularAcceleration(m_angle, m_angularVelocity, cartAcceleration);
    
    m_angularVelocity += angularAccel * dt;
    m_angle += m_angularVelocity * dt;
    
    m_angle = normalizeAngle(m_angle);
}

void SinglePendulum::reset()
{
    m_angle = 0.1;
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
    double numerator = GRAVITY * std::sin(angle) - cartAccel * std::cos(angle);
    double angularAccel = numerator / m_length;
    return angularAccel;
}

double SinglePendulum::normalizeAngle(double angle)
{
    const double PI = 3.14159265358979323846;
    while (angle > PI) angle -= 2.0 * PI;
    while (angle < -PI) angle += 2.0 * PI;
    return angle;
}