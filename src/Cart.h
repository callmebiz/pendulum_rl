#pragma once

/**
 * Cart class - represents the movable cart on a rail
 *
 * The cart slides left/right along a fixed-length rail.
 * It has mass, position, velocity, and can have forces applied to it.
 */
class Cart
{
public:
    // Constructor: set up cart with initial parameters
    Cart(double mass, double railLength);

    // Physics update
    // Returns the effective horizontal acceleration that was actually applied
    // to the cart (may be zeroed if the cart is blocked at the rail end).
    double update(double dt, double appliedAcceleration, double friction, double gravity);

    // Getters
    double getPosition() const { return m_position; }
    double getVelocity() const { return m_velocity; }
    double getMass() const { return m_mass; }
    double getRailLength() const { return m_railLength; }
    double getWidth() const { return m_width; }
    double getHeight() const { return m_height; }

    // Setters for runtime tuning
    void setRailLength(double len) { m_railLength = len; }
    void setWidth(double w) { m_width = w; }
    void setHeight(double h) { m_height = h; }
    void setMass(double m) { m_mass = m; }

    // Setters
    void setPosition(double pos) { m_position = pos; }
    void setVelocity(double vel) { m_velocity = vel; }

    // Reset to center
    void reset();

    // Wrap-around behavior for rail: when enabled, cart wraps from one end to the other
    void setWrapEnabled(bool enabled) { m_wrapEnabled = enabled; }
    bool isWrapEnabled() const { return m_wrapEnabled; }

private:
    double m_mass;        // Cart mass (kg)
    double m_position;    // Position along rail (m)
    double m_velocity;    // Velocity (m/s)
    double m_railLength;  // Total rail length (m)
    double m_width = WIDTH;   // visual width (m)
    double m_height = HEIGHT; // visual height (m)
    bool m_wrapEnabled = false;

    // Constants (defaults)
    static constexpr double WIDTH = 0.4;   // Default cart width (m) for rendering
    static constexpr double HEIGHT = 0.2;  // Default cart height (m) for rendering
};