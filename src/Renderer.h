#pragma once

#include "Shader.h"
#include "Cart.h"
#include "Pendulum.h"
#include "SinglePendulum.h"
#include "DoublePendulum.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

/**
 * Renderer - handles all OpenGL rendering in 2D
 */
class Renderer
{
public:
    Renderer(int windowWidth, int windowHeight);
    ~Renderer();
    
    void initialize();
    void render(const Cart& cart, const Pendulum& pendulum, bool isSingle);
    void onWindowResize(int width, int height);
    // Adjust view width to fit the given rail length (meters). Max cap applied.
    void setViewWidthForRail(double railLength);
    
private:
    int m_windowWidth;
    int m_windowHeight;
    
    glm::mat4 m_projection;
    Shader* m_shader;
    // World view width in meters (controls zoom). Adjusted to fit rail length.
    float m_viewWidth;
    
    unsigned int m_rectangleVAO, m_rectangleVBO;
    unsigned int m_lineVAO, m_lineVBO;
    unsigned int m_circleVAO, m_circleVBO;
    
    void setupRectangle();
    void setupLine();
    void setupCircle();
    
    void drawRectangle(const glm::vec2& position, const glm::vec2& size, 
                      const glm::vec3& color);
    void drawLine(const glm::vec2& start, const glm::vec2& end, 
                 const glm::vec3& color, float thickness = 0.02f);
    void drawCircle(const glm::vec2& position, float radius, 
                   const glm::vec3& color);
    void drawWheel(const glm::vec2& center, float radius, const glm::vec3& tireColor, const glm::vec3& rimColor);
    void drawRail(const Cart& cart);
    
    void updateProjection();
    
    static constexpr float PIXELS_PER_METER = 100.0f;
    static constexpr int CIRCLE_SEGMENTS = 32;
};