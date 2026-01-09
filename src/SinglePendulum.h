#pragma once

#include "Pendulum.h"

/**
 * SinglePendulum - one pendulum attached to cart
 */
class SinglePendulum : public Pendulum
{
public:
    SinglePendulum(double mass, double length);
    
    void update(double dt, double cartAcceleration) override;
    void reset() override;
    int getNumAngles() const override { return 1; }
    
    double getAngle(int index) const override;
    double getAngularVelocity(int index) const override;
    double getMass() const { return m_mass; }
    double getLength() const { return m_length; }
    
    void setAngle(double angle) { m_angle = angle; }
    void setAngularVelocity(double vel) { m_angularVelocity = vel; }
    void setMass(double m) { m_mass = m; }
    void setLength(double l) { m_length = l; }
    void setInitialAngle(double angle) { m_initialAngle = angle; }
    double getInitialAngle() const { return m_initialAngle; }
    
private:
    double m_mass;
    double m_length;
    double m_initialAngle;
    double m_angle;
    double m_angularVelocity;
    
    double computeAngularAcceleration(double angle, double angularVel, double cartAccel);
    double normalizeAngle(double angle);
};