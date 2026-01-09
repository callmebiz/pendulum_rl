#pragma once

/**
 * Pendulum base class - common interface for all pendulum types
 *
 * This is an abstract base class that defines the interface
 * for both single and double pendulums.
 */
class Pendulum
{
public:
    virtual ~Pendulum() = default;

    // Pure virtual functions - must be implemented by derived classes
    virtual void update(double dt, double cartAcceleration) = 0;
    virtual void reset() = 0;
    virtual int getNumAngles() const = 0;

    // Common getters (implemented by derived classes)
    virtual double getAngle(int index) const = 0;
    virtual double getAngularVelocity(int index) const = 0;

    // Set gravity (called from main loop)
    void setGravity(double g) { m_gravity = g; }
    double getGravity() const { return m_gravity; }
    void setDamping(double d) { m_damping = d; }
    double getDamping() const { return m_damping; }

    // Energy instrumentation: kinetic / potential energy of the pendulum
    virtual double getKineticEnergy(double cartVelocity) const = 0;
    virtual double getPotentialEnergy() const = 0;

    // Convenience: total mechanical energy (pendulum only)
    double getTotalEnergy(double cartVelocity) const { return getKineticEnergy(cartVelocity) + getPotentialEnergy(); }

protected:
    // Physics constants
    double m_gravity = 9.81;  // m/s^2 - can be changed at runtime
    double m_damping = 0.1;   // generic damping term (applies as angular damping)
};