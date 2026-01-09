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
    void setInitialAngle(int index, double angle) {
        if (index == 0) m_initialAngle1 = angle; else m_initialAngle2 = angle;
    }
    double getInitialAngle(int index) const { return (index == 0) ? m_initialAngle1 : m_initialAngle2; }
    void setMass(int index, double m) {
        if (index == 0) m_mass1 = m; else m_mass2 = m;
    }
    void setLength(int index, double l) {
        if (index == 0) m_length1 = l; else m_length2 = l;
    }
    double getKineticEnergy(double cartVelocity) const;
    double getPotentialEnergy() const;
    
private:
    double m_mass1, m_mass2;
    double m_length1, m_length2;
    double m_angle1, m_angle2;
    double m_angularVelocity1, m_angularVelocity2;
    double m_initialAngle1, m_initialAngle2;
    
    void computeAngularAccelerations(double angle1, double angle2, 
                                    double angVel1, double angVel2,
                                    double cartAccel,
                                    double& angAccel1, double& angAccel2);
    
    double normalizeAngle(double angle);
};