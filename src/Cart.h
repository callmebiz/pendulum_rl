#pragma once

/**
 * Cart class - represents the movable cart on a rail
 */
class Cart
{
public:
    Cart(double mass, double railLength);
    
    void update(double dt, double appliedForce, double friction);
    
    double getPosition() const { return m_position; }
    double getVelocity() const { return m_velocity; }
    double getMass() const { return m_mass; }
    double getRailLength() const { return m_railLength; }
    
    void setPosition(double pos) { m_position = pos; }
    void setVelocity(double vel) { m_velocity = vel; }
    
    void reset();
    
private:
    double m_mass;
    double m_position;
    double m_velocity;
    double m_railLength;
    
    static constexpr double WIDTH = 0.4;
    static constexpr double HEIGHT = 0.2;
    
    friend class Renderer;
};