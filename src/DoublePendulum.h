#pragma once

#include "Pendulum.h"

/**
 * DoublePendulum - two pendulums connected in series
 */
class DoublePendulum : public Pendulum
{
public:
    DoublePendulum(double mass1, double length1, double mass2, double length2);
    
    void update(double dt, double cartAcceleration) override;
    void reset() override;
    int getNumAngles() const override { return 2; }
    
    double getAngle(int index) const override;
    double getAngularVelocity(int index) const override;
    double getMass(int index) const;
    double getLength(int index) const;
    
    void setAngle(int index, double angle);
    void setAngularVelocity(int index, double vel);
    
private:
    double m_mass1, m_mass2;
    double m_length1, m_length2;
    double m_angle1, m_angle2;
    double m_angularVelocity1, m_angularVelocity2;
    
    void computeAngularAccelerations(double angle1, double angle2, 
                                    double angVel1, double angVel2,
                                    double cartAccel,
                                    double& angAccel1, double& angAccel2);
    
    double normalizeAngle(double angle);
};