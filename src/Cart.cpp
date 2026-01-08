#include "Cart.h"
#include <algorithm>
#include <cmath>

Cart::Cart(double mass, double railLength)
    : m_mass(mass)
    , m_railLength(railLength)
    , m_position(0.0)
    , m_velocity(0.0)
{
}

void Cart::update(double dt, double appliedForce, double friction)
{
    double frictionForce = -friction * m_velocity;
    double totalForce = appliedForce + frictionForce;
    double acceleration = totalForce / m_mass;
    
    m_velocity += acceleration * dt;
    m_position += m_velocity * dt;
    
    double halfRail = m_railLength / 2.0;
    if (m_position < -halfRail) {
        m_position = -halfRail;
        m_velocity = 0.0;
    }
    if (m_position > halfRail) {
        m_position = halfRail;
        m_velocity = 0.0;
    }
}

void Cart::reset()
{
    m_position = 0.0;
    m_velocity = 0.0;
}