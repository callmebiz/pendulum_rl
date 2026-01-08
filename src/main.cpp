#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Renderer.h"
#include "Cart.h"
#include "SinglePendulum.h"
#include "DoublePendulum.h"
#include "InputController.h"

#include <iostream>
#include <memory>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
                                         "Pendulum ML - Phase 1", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
    
    Cart cart(1.0, 4.0);
    
    std::unique_ptr<SinglePendulum> singlePendulum = 
        std::make_unique<SinglePendulum>(0.3, 1.0);
    
    std::unique_ptr<DoublePendulum> doublePendulum = 
        std::make_unique<DoublePendulum>(1.0, 1.0, 1.0, 1.0);
    
    bool useSinglePendulum = true;
    Pendulum* currentPendulum = singlePendulum.get();
    
    Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT);
    renderer.initialize();
    
    InputController input(window);
    
    double friction = 0.1;
    double dt = 1.0 / 60.0;
    double simulationTime = 0.0;
    
    std::cout << "\n=== Controls ===\n";
    std::cout << "A/D: Move cart left/right\n";
    std::cout << "SPACE: Toggle single/double pendulum\n";
    std::cout << "R: Reset simulation\n";
    std::cout << "ESC: Quit\n";
    std::cout << "================\n\n";
    
    while (!glfwWindowShouldClose(window))
    {
        input.update();
        
        if (input.shouldTogglePendulum()) {
            useSinglePendulum = !useSinglePendulum;
            currentPendulum = useSinglePendulum ? 
                static_cast<Pendulum*>(singlePendulum.get()) : 
                static_cast<Pendulum*>(doublePendulum.get());
            
            std::cout << "Switched to " << (useSinglePendulum ? "SINGLE" : "DOUBLE") 
                      << " pendulum\n";
        }
        
        if (input.shouldReset()) {
            cart.reset();
            singlePendulum->reset();
            doublePendulum->reset();
            simulationTime = 0.0;
            std::cout << "Simulation reset\n";
        }
        
        double appliedForce = input.getCartForce();
        
        cart.update(dt, appliedForce, friction);
        
        double cartAcceleration = appliedForce / cart.getMass();
        
        currentPendulum->update(dt, cartAcceleration);
        
        simulationTime += dt;
        
        renderer.render(cart, *currentPendulum, useSinglePendulum);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Simulation Info");
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Time: %.2f s", simulationTime);
        ImGui::Separator();
        ImGui::Text("Mode: %s Pendulum", useSinglePendulum ? "SINGLE" : "DOUBLE");
        ImGui::Separator();
        ImGui::Text("Cart:");
        ImGui::Text("  Position: %.3f m", cart.getPosition());
        ImGui::Text("  Velocity: %.3f m/s", cart.getVelocity());
        ImGui::Text("  Force: %.1f N", appliedForce);
        ImGui::Separator();
        
        if (useSinglePendulum) {
            double angle = currentPendulum->getAngle(0);
            double angVel = currentPendulum->getAngularVelocity(0);
            ImGui::Text("Pendulum:");
            ImGui::Text("  Angle: %.2f deg", angle * 180.0 / 3.14159);
            ImGui::Text("  Ang Vel: %.2f deg/s", angVel * 180.0 / 3.14159);
        } else {
            double angle1 = currentPendulum->getAngle(0);
            double angle2 = currentPendulum->getAngle(1);
            ImGui::Text("Pendulum 1 (yellow):");
            ImGui::Text("  Angle: %.2f deg", angle1 * 180.0 / 3.14159);
            ImGui::Text("Pendulum 2 (blue):");
            ImGui::Text("  Angle: %.2f deg", angle2 * 180.0 / 3.14159);
        }
        
        ImGui::Separator();
        ImGui::SliderFloat("Friction", (float*)&friction, 0.0f, 1.0f);
        
        ImGui::End();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}