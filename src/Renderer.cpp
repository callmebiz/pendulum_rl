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
    , m_viewWidth(8.0f)
{
}

Renderer::~Renderer()
{
    // Clean up OpenGL resources
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
    // Load shaders
    m_shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");

    // Set up geometry buffers
    setupRectangle();
    setupLine();
    setupCircle();

    // Set up projection matrix
    updateProjection();

    // OpenGL settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);
}

void Renderer::render(const Cart& cart, const Pendulum& pendulum, bool isSingle)
{
    // Clear screen
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader->use();
    m_shader->setMat4("projection", m_projection);

    // Draw rail (horizontal line at y=0)
    // Draw stylized rail
    drawRail(cart);

    // Draw cart - position it so it sits ON the rail
    glm::vec2 cartPos(cart.getPosition(), static_cast<float>(cart.getHeight() / 2.0));  // Center cart above rail
    glm::vec2 cartSize(static_cast<float>(cart.getWidth()), static_cast<float>(cart.getHeight()));

    // Cart body
    drawRectangle(cartPos, cartSize, glm::vec3(0.22f, 0.45f, 0.7f));
    // Top panel
    drawRectangle(cartPos + glm::vec2(0.0f, static_cast<float>(cart.getHeight()*0.15)), glm::vec2(cartSize.x * 0.9f, cartSize.y * 0.4f), glm::vec3(0.18f, 0.36f, 0.55f));

    // Wheels (two wheels under the cart)
    float wheelOffset = cartSize.x * 0.33f;
    float wheelY = static_cast<float>(-cart.getHeight() / 2.0 + 0.0f); // wheels aligned slightly below cart center
    drawWheel(cartPos + glm::vec2(-wheelOffset, wheelY), 0.08f, glm::vec3(0.05f,0.05f,0.05f), glm::vec3(0.6f,0.6f,0.6f));
    drawWheel(cartPos + glm::vec2(wheelOffset, wheelY), 0.08f, glm::vec3(0.05f,0.05f,0.05f), glm::vec3(0.6f,0.6f,0.6f));

    // Draw pendulum(s)
    if (isSingle) {
        // Single pendulum
        const SinglePendulum* sp = dynamic_cast<const SinglePendulum*>(&pendulum);
        if (sp) {
            double angle = sp->getAngle(0);
            double length = sp->getLength();

            // Pendulum starts at top of cart
            // Pendulum starts at top of cart (top of cart is cartPos + half height)
            glm::vec2 pendulumStart = cartPos + glm::vec2(0.0f, static_cast<float>(cart.getHeight() / 2.0));
            // Physics uses angle where 0 = hanging down; convert to screen coords
            // (x = L*sin(theta), y = -L*cos(theta)) because +y is up in screen space.
            glm::vec2 pendulumEnd = pendulumStart + glm::vec2(
                static_cast<float>(length * std::sin(angle)),
                static_cast<float>(-length * std::cos(angle))
            );

            // Draw rod
            drawLine(pendulumStart, pendulumEnd, glm::vec3(0.85f, 0.75f, 0.25f), 0.03f);
            // Mass with rim
            drawCircle(pendulumEnd, 0.10f, glm::vec3(0.95f, 0.85f, 0.35f));
            drawCircle(pendulumEnd, 0.06f, glm::vec3(0.25f,0.18f,0.08f));
        }
    }
    else {
        // Double pendulum
        const DoublePendulum* dp = dynamic_cast<const DoublePendulum*>(&pendulum);
        if (dp) {
            double angle1 = dp->getAngle(0);
            double angle2 = dp->getAngle(1);
            double length1 = dp->getLength(0);
            double length2 = dp->getLength(1);

            // First pendulum starts at top of cart
            glm::vec2 start = cartPos + glm::vec2(0.0f, static_cast<float>(cart.getHeight() / 2.0));
            glm::vec2 joint = start + glm::vec2(
                static_cast<float>(length1 * std::sin(angle1)),
                static_cast<float>(-length1 * std::cos(angle1))
            );

            // Second pendulum starts at end of first
            glm::vec2 end = joint + glm::vec2(
                static_cast<float>(length2 * std::sin(angle2)),
                static_cast<float>(-length2 * std::cos(angle2))
            );

            // Draw first rod (yellow)
            drawLine(start, joint, glm::vec3(0.85f,0.75f,0.25f), 0.03f);
            drawCircle(joint, 0.10f, glm::vec3(0.95f,0.85f,0.35f));
            drawCircle(joint, 0.06f, glm::vec3(0.25f,0.18f,0.08f));

            drawLine(joint, end, glm::vec3(0.22f,0.5f,0.92f), 0.03f);
            drawCircle(end, 0.10f, glm::vec3(0.35f,0.68f,1.0f));
            drawCircle(end, 0.06f, glm::vec3(0.08f,0.06f,0.03f));
        }
    }
}

void Renderer::drawWheel(const glm::vec2& center, float radius, const glm::vec3& tireColor, const glm::vec3& rimColor)
{
    // Tire
    drawCircle(center, radius, tireColor);
    // Rim
    drawCircle(center, radius * 0.6f, rimColor);
    // Axle (small dark dot)
    drawCircle(center, radius * 0.18f, glm::vec3(0.02f,0.02f,0.02f));
    // Spoke: rotate based on horizontal position to simulate rolling
    float angle = center.x / radius; // simple visual linkage: position -> rotation
    glm::vec2 spokeEnd = center + glm::vec2(std::cos(angle), std::sin(angle)) * radius * 0.65f;
    drawLine(center, spokeEnd, glm::vec3(0.1f,0.1f,0.1f), 0.015f);
}

void Renderer::drawRail(const Cart& cart)
{
    float halfRail = cart.getRailLength() / 2.0f;
    // Base rail strip (dark)
    drawLine(glm::vec2(-halfRail, 0.0f), glm::vec2(halfRail, 0.0f), glm::vec3(0.12f,0.12f,0.12f), 0.12f);
    // Top shiny rail edge
    drawLine(glm::vec2(-halfRail, 0.03f), glm::vec2(halfRail, 0.03f), glm::vec3(0.6f,0.6f,0.6f), 0.02f);

    // Sleepers / ties
    float spacing = 0.5f; // meters
    for (float x = -halfRail; x <= halfRail; x += spacing) {
        glm::vec2 tiePos(x, 0.0f);
        drawRectangle(tiePos, glm::vec2(0.12f, 0.02f), glm::vec3(0.5f,0.35f,0.2f));
    }
}

void Renderer::onWindowResize(int width, int height)
{
    m_windowWidth = width;
    m_windowHeight = height;
    glViewport(0, 0, width, height);
    updateProjection();
}

void Renderer::setViewWidthForRail(double railLength)
{
    // Add a margin so the rail isn't flush to the edges
    const float marginFactor = 1.2f;
    const float maxView = 20.0f; // maximum view width in meters
    float desired = static_cast<float>(railLength) * marginFactor;
    // Ensure a sensible minimum as well
    const float minView = 4.0f;
    m_viewWidth = std::min(std::max(desired, minView), maxView);
    updateProjection();
}

void Renderer::setupRectangle()
{
    // Unit rectangle centered at origin
    float vertices[] = {
        // Positions (x,y)  // Colors (r,g,b)
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

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::setupLine()
{
    // Simple line from (0,0) to (1,0) - we'll transform it
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

    // Center point
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    vertices.push_back(1.0f);
    vertices.push_back(1.0f);

    // Circle points
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

    m_shader->setVec3("uColor", color);
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
    // Translate to the start point, rotate to match direction, then translate
    // by half-length so the rectangle (centered geometry) spans from start->end.
    model = glm::translate(model, glm::vec3(start, 0.0f));
    model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(length * 0.5f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(length, thickness, 1.0f));

    m_shader->setMat4("model", model);

    m_shader->setVec3("uColor", color);
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

    m_shader->setVec3("uColor", color);
    glBindVertexArray(m_circleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_SEGMENTS + 2);
}

void Renderer::updateProjection()
{
    // Set up orthographic projection
    float aspect = static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight);
    float viewWidth = m_viewWidth;
    float viewHeight = viewWidth / aspect;

    m_projection = glm::ortho(-viewWidth / 2.0f, viewWidth / 2.0f,
        -viewHeight / 2.0f, viewHeight / 2.0f,
        -1.0f, 1.0f);
}