#include "InputController.h"

InputController::InputController(GLFWwindow* window)
    : m_window(window)
    , m_cartForce(0.0)
    , m_togglePressed(false)
    , m_resetPressed(false)
    , m_spaceWasPressed(false)
    , m_rWasPressed(false)
{
}

void InputController::update()
{
    m_togglePressed = false;
    m_resetPressed = false;
    m_cartForce = 0.0;
    
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
        m_cartForce = -MAX_FORCE;
    }
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
        m_cartForce = MAX_FORCE;
    }
    
    bool spacePressed = (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS);
    if (spacePressed && !m_spaceWasPressed) {
        m_togglePressed = true;
    }
    m_spaceWasPressed = spacePressed;
    
    bool rPressed = (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS);
    if (rPressed && !m_rWasPressed) {
        m_resetPressed = true;
    }
    m_rWasPressed = rPressed;
    
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, true);
    }
}