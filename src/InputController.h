#pragma once

#include <GLFW/glfw3.h>

/**
 * InputController - handles keyboard input
 */
class InputController
{
public:
    InputController(GLFWwindow* window);
    
    void update();
    
    double getCartForce() const { return m_cartForce; }
    bool shouldTogglePendulum() const { return m_togglePressed; }
    bool shouldReset() const { return m_resetPressed; }
    
private:
    GLFWwindow* m_window;
    
    double m_cartForce;
    bool m_togglePressed;
    bool m_resetPressed;
    
    bool m_spaceWasPressed;
    bool m_rWasPressed;
    
    static constexpr double MAX_FORCE = 50.0;
};