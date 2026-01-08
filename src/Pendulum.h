#pragma once

/**
 * Pendulum base class - common interface for all pendulum types
 */
class Pendulum
{
public:
    virtual ~Pendulum() = default;
    
    virtual void update(double dt, double cartAcceleration) = 0;
    virtual void reset() = 0;
    virtual int getNumAngles() const = 0;
    
    virtual double getAngle(int index) const = 0;
    virtual double getAngularVelocity(int index) const = 0;
    
protected:
    static constexpr double GRAVITY = 9.81;
};