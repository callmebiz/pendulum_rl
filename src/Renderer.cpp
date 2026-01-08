#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>

Renderer::Renderer(int windowWidth, int windowHeight)
    : m_windowWidth(windowWidth)
    , m_windowHeight(windowHeight)
    , m_shader(nullptr)
    , m_rectangleVAO(0)
    , m_lineVAO(0)
    , m_circleVAO(0)
{
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_rectangleVAO);
    glDeleteBuffers(1, &m_rectangleVBO);
    glDeleteVertexArrays(1, &m_lineVAO);
    glDeleteBuffers(1, &m_lineVBO);
    glDeleteVertexArrays(1, &m_circleVAO);
    glDeleteBuffers(1, &m_circleVBO);
    delete m_shader;
}

void Renderer::initialize()
{
    m_shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    
    setupRectangle();
    setupLine();
    setupCircle();
    
    updateProjection();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);
}

void Renderer::render(const Cart& cart, const Pendulum& pendulum, bool isSingle)
{
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    m_shader->use();
    m_shader->setMat4("projection", m_projection);
    
    float halfRail = cart.getRailLength() / 2.0f;
    drawLine(glm::vec2(-halfRail, 0.0f), glm::vec2(halfRail, 0.0f), 
             glm::vec3(0.3f, 0.3f, 0.3f), 0.03f);
    
    glm::vec2 cartPos(cart.getPosition(), 0.0f);
    glm::vec2 cartSize(Cart::WIDTH, Cart::HEIGHT);
    drawRectangle(cartPos, cartSize, glm::vec3(0.4f, 0.5f, 0.6f));
    
    if (isSingle) {
        const SinglePendulum* sp = dynamic_cast<const SinglePendulum*>(&pendulum);
        if (sp) {
            double angle = sp->getAngle(0);
            double length = sp->getLength();
            
            glm::vec2 pendulumStart = cartPos + glm::vec2(0.0f, Cart::HEIGHT / 2.0f);
            glm::vec2 pendulumEnd = pendulumStart + glm::vec2(
                static_cast<float>(length * std::sin(angle)),
                static_cast<float>(-length * std::cos(angle))
            );
            
            drawLine(pendulumStart, pendulumEnd, glm::vec3(0.8f, 0.7f, 0.2f), 0.02f);
            drawCircle(pendulumEnd, 0.08f, glm::vec3(0.9f, 0.8f, 0.3f));
        }
    } else {
        const DoublePendulum* dp = dynamic_cast<const DoublePendulum*>(&pendulum);
        if (dp) {
            double angle1 = dp->getAngle(0);
            double angle2 = dp->getAngle(1);
            double length1 = dp->getLength(0);
            double length2 = dp->getLength(1);
            
            glm::vec2 start = cartPos + glm::vec2(0.0f, Cart::HEIGHT / 2.0f);
            glm::vec2 joint = start + glm::vec2(
                static_cast<float>(length1 * std::sin(angle1)),
                static_cast<float>(-length1 * std::cos(angle1))
            );
            
            glm::vec2 end = joint + glm::vec2(
                static_cast<float>(length2 * std::sin(angle2)),
                static_cast<float>(-length2 * std::cos(angle2))
            );
            
            drawLine(start, joint, glm::vec3(0.8f, 0.7f, 0.2f), 0.02f);
            drawCircle(joint, 0.08f, glm::vec3(0.9f, 0.8f, 0.3f));
            
            drawLine(joint, end, glm::vec3(0.2f, 0.5f, 0.9f), 0.02f);
            drawCircle(end, 0.08f, glm::vec3(0.3f, 0.6f, 1.0f));
        }
    }
}

void Renderer::onWindowResize(int width, int height)
{
    m_windowWidth = width;
    m_windowHeight = height;
    glViewport(0, 0, width, height);
    updateProjection();
}

void Renderer::setupRectangle()
{
    float vertices[] = {
        -0.5f, -0.5f,       1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,       1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,       1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,       1.0f, 1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &m_rectangleVAO);
    glGenBuffers(1, &m_rectangleVBO);
    
    glBindVertexArray(m_rectangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void Renderer::setupLine()
{
    float vertices[] = {
        0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 0.0f,   1.0f, 1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &m_lineVAO);
    glGenBuffers(1, &m_lineVBO);
    
    glBindVertexArray(m_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void Renderer::setupCircle()
{
    std::vector<float> vertices;
    
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    vertices.push_back(1.0f);
    vertices.push_back(1.0f);
    
    for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
        float angle = (float)i / (float)CIRCLE_SEGMENTS * 2.0f * 3.14159f;
        vertices.push_back(std::cos(angle));
        vertices.push_back(std::sin(angle));
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
    }
    
    glGenVertexArrays(1, &m_circleVAO);
    glGenBuffers(1, &m_circleVBO);
    
    glBindVertexArray(m_circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_circleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                 vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void Renderer::drawRectangle(const glm::vec2& position, const glm::vec2& size, 
                            const glm::vec3& color)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    
    m_shader->setMat4("model", model);
    
    glBindVertexArray(m_rectangleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Renderer::drawLine(const glm::vec2& start, const glm::vec2& end, 
                       const glm::vec3& color, float thickness)
{
    glm::vec2 direction = end - start;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    float angle = std::atan2(direction.y, direction.x);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(start, 0.0f));
    model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(length, thickness, 1.0f));
    
    m_shader->setMat4("model", model);
    
    glBindVertexArray(m_rectangleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Renderer::drawCircle(const glm::vec2& position, float radius, 
                         const glm::vec3& color)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(radius, radius, 1.0f));
    
    m_shader->setMat4("model", model);
    
    glBindVertexArray(m_circleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_SEGMENTS + 2);
}

void Renderer::updateProjection()
{
    float aspect = static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight);
    float viewWidth = 8.0f;
    float viewHeight = viewWidth / aspect;
    
    m_projection = glm::ortho(-viewWidth / 2.0f, viewWidth / 2.0f,
                             -viewHeight / 2.0f, viewHeight / 2.0f,
                             -1.0f, 1.0f);
}