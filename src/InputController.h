#pragma once

#include <GLFW/glfw3.h>

/**
 * InputController - handles keyboard and mouse input
 *
 * Maps keys to actions:
 * - A/D: Move cart left/right
 * - Space: Toggle between single and double pendulum
 * - R: Reset simulation
 * - ESC: Quit
 */
class InputController
{
public:
    InputController(GLFWwindow* window);

    // Update input state (call once per frame)
    void update();

    // Query current input state
    double getCartAcceleration() const { return m_cartAcceleration; }
    bool shouldTogglePendulum() const { return m_togglePressed; }
    bool shouldReset() const { return m_resetPressed; }
    // Runtime tuning for maximum acceleration applied by A/D keys
    void setMaxAcceleration(double a) { m_maxAcceleration = a; }
    double getMaxAcceleration() const { return m_maxAcceleration; }

private:
    GLFWwindow* m_window;

    // Input state
    double m_cartAcceleration;   // Acceleration to apply to cart
    bool m_togglePressed;        // Space pressed this frame?
    bool m_resetPressed;         // R pressed this frame?

    // Previous key states (for detecting "just pressed")
    bool m_spaceWasPressed;
    bool m_rWasPressed;

    // Maximum acceleration (m/s^2) used when keys are pressed
    double m_maxAcceleration = 30.0;
};