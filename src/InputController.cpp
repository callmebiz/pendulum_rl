#include "InputController.h"

InputController::InputController(GLFWwindow* window)
    : m_window(window)
    , m_cartAcceleration(0.0)
    , m_togglePressed(false)
    , m_resetPressed(false)
    , m_spaceWasPressed(false)
    , m_rWasPressed(false)
{
}

void InputController::update()
{
    // Reset single-frame events
    m_togglePressed = false;
    m_resetPressed = false;
    m_cartAcceleration = 0.0;

    // Check A/D keys for cart movement
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
        m_cartAcceleration = -m_maxAcceleration;  // Accelerate left
    }
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
        m_cartAcceleration = m_maxAcceleration;   // Accelerate right
    }
    // Also support left/right arrow keys
    if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        m_cartAcceleration = -m_maxAcceleration;  // Accelerate left
    }
    if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        m_cartAcceleration = m_maxAcceleration;   // Accelerate right
    }

    // Check spacebar for toggle (detect "just pressed")
    bool spacePressed = (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS);
    if (spacePressed && !m_spaceWasPressed) {
        m_togglePressed = true;  // Just pressed this frame
    }
    m_spaceWasPressed = spacePressed;

    // Check R key for reset (detect "just pressed")
    bool rPressed = (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS);
    if (rPressed && !m_rWasPressed) {
        m_resetPressed = true;  // Just pressed this frame
    }
    m_rWasPressed = rPressed;

    // ESC to close window
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, true);
    }
}