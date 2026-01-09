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
#include <vector>
#include <algorithm>
#include <cfloat>

// Window dimensions
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// Callback for window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
    // ============================================================
    // GLFW Initialization
    // ============================================================
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Request 4x multisample anti-aliasing (MSAA)
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
        "Pendulum ML - Phase 1", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);  // Enable vsync

    // ============================================================
    // GLAD Initialization (Load OpenGL function pointers)
    // ============================================================
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable multisampling (MSAA) if available
    glEnable(GL_MULTISAMPLE);

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // ============================================================
    // ImGui Initialization (for UI overlays)
    // ============================================================
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    // ============================================================
    // Create simulation objects
    // ============================================================

    // Cart on rail
    Cart cart(1.0, 10.0);  // 1kg mass, 10m rail

    // Single pendulum (default mass = 1.0 kg)
    std::unique_ptr<SinglePendulum> singlePendulum =
        std::make_unique<SinglePendulum>(1.0, 1.0);  // 1kg, 1m length

    // Double pendulum (default masses = 1.0 kg)
    std::unique_ptr<DoublePendulum> doublePendulum =
        std::make_unique<DoublePendulum>(1.0, 1.0, 1.0, 1.0);  // 1kg, 1m each

    // Start with single pendulum
    bool useSinglePendulum = true;
    Pendulum* currentPendulum = singlePendulum.get();

    // Renderer
    Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT);
    renderer.initialize();
    // Ensure initial view fits the cart rail
    renderer.setViewWidthForRail(cart.getRailLength());

    // Input controller
    InputController input(window);

    // ============================================================
    // Simulation parameters
    // ============================================================
    float friction = 0.1f;      // Friction / damping coefficient (UI float)
    float gravity = 9.81f;      // Gravitational acceleration (m/s^2) (UI float)
    double dt = 1.0 / 144.0;     // Time step (144 Hz)
    double simulationTime = 0.0;

    // Energy instrumentation (history buffer for plotting)
    const int ENERGY_HISTORY_SIZE = 1440; // e.g. 10s @ 144Hz
    std::vector<float> energyHistory(ENERGY_HISTORY_SIZE, 0.0f);
    int energyIndex = 0;
    int energyCount = 0;

    // ============================================================
    // Main Loop
    // ============================================================
    std::cout << "\n=== Controls ===\n";
    std::cout << "A/D: Move cart left/right\n";
    std::cout << "Left/Right arrows: Move cart left/right\n";
    std::cout << "SPACE: Toggle single/double pendulum\n";
    std::cout << "R: Reset simulation\n";
    std::cout << "ESC: Quit\n";
    std::cout << "================\n\n";

    while (!glfwWindowShouldClose(window))
    {
        // Update input
        input.update();

        // Handle toggle
        if (input.shouldTogglePendulum()) {
            useSinglePendulum = !useSinglePendulum;
            currentPendulum = useSinglePendulum ?
                static_cast<Pendulum*>(singlePendulum.get()) :
                static_cast<Pendulum*>(doublePendulum.get());

            // Reset simulation state when switching modes so no motion persists
            cart.reset();
            singlePendulum->reset();
            doublePendulum->reset();
            simulationTime = 0.0;

            std::cout << "Switched to " << (useSinglePendulum ? "SINGLE" : "DOUBLE")
                << " pendulum (state reset)\n";
        }

        // Handle reset
        if (input.shouldReset()) {
            cart.reset();
            singlePendulum->reset();
            doublePendulum->reset();
            simulationTime = 0.0;
            std::cout << "Simulation reset\n";
        }

        // Get user input acceleration
        double appliedAcceleration = input.getCartAcceleration();

        // Update physics parameters
        currentPendulum->setGravity(static_cast<double>(gravity));
        currentPendulum->setDamping(static_cast<double>(friction));

        // Update cart physics (friction passed as damping for cart velocity)
        // Cart::update now returns the effective acceleration that actually
        // occurred (zero when the cart is blocked at the rail end and the
        // user continues pressing into the wall). Use that for pendulum.
        double effectiveAcceleration = cart.update(dt, appliedAcceleration,
            static_cast<double>(friction), static_cast<double>(gravity));

        // Update pendulum physics (gravity & damping are used inside pendulum equations)
        currentPendulum->update(dt, effectiveAcceleration);

        // Update simulation time
        simulationTime += dt;

        // -----------------------------
        // Energy instrumentation (store in millijoules)
        // -----------------------------
        double cartKE = 0.5 * cart.getMass() * cart.getVelocity() * cart.getVelocity();
        double pendKE = currentPendulum->getKineticEnergy(cart.getVelocity());
        double pendPE = currentPendulum->getPotentialEnergy();
        double totalEnergy = cartKE + pendKE + pendPE;

        // Push into circular buffer in millijoules (mJ)
        energyHistory[energyIndex] = static_cast<float>(totalEnergy * 1000.0);
        energyIndex = (energyIndex + 1) % ENERGY_HISTORY_SIZE;
        if (energyCount < ENERGY_HISTORY_SIZE) ++energyCount;

        // ========================================================
        // Rendering
        // ========================================================

        // Adjust view to fit rail length (allows runtime rail changes)
        renderer.setViewWidthForRail(cart.getRailLength());

        // Render 3D scene
        renderer.render(cart, *currentPendulum, useSinglePendulum);

        // Render ImGui overlay
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Info window with tabs
        ImGui::Begin("Simulation Info");
        if (ImGui::BeginTabBar("SimTabs")) {
            if (ImGui::BeginTabItem("Info")) {
                ImGui::Text("FPS: %.1f", io.Framerate);
                ImGui::Text("Time: %.2f s", simulationTime);
                ImGui::Separator();
                ImGui::Text("Mode: %s Pendulum", useSinglePendulum ? "SINGLE" : "DOUBLE");
                ImGui::Separator();
                ImGui::Text("Cart:");
                ImGui::Text("  Position: %.6f m", cart.getPosition());
                ImGui::Text("  Velocity: %.6f m/s", cart.getVelocity());
                ImGui::Text("  Acceleration: %.4f m/s^2", appliedAcceleration);
                ImGui::Text("  Wrap: %s", cart.isWrapEnabled() ? "ON" : "OFF");
                ImGui::Separator();

                if (useSinglePendulum) {
                    double angle = currentPendulum->getAngle(0);
                    double angVel = currentPendulum->getAngularVelocity(0);
                    ImGui::Text("Pendulum:");
                    ImGui::Text("  Angle: %.9f deg", angle * 180.0 / 3.14159);
                    ImGui::Text("  Ang Vel: %.9f deg/s", angVel * 180.0 / 3.14159);
                }
                else {
                    double angle1 = currentPendulum->getAngle(0);
                    double angle2 = currentPendulum->getAngle(1);
                    ImGui::Text("Pendulum 1 (yellow):");
                    ImGui::Text("  Angle: %.9f deg", angle1 * 180.0 / 3.14159);
                    ImGui::Text("Pendulum 2 (blue):");
                    ImGui::Text("  Angle: %.9f deg", angle2 * 180.0 / 3.14159);
                }

                ImGui::Separator();
                // Energy display / plot (millijoules)
                ImGui::Text("Energy (mJ):");
                ImGui::Text("  Cart KE: %.9f", cartKE * 1000.0);
                ImGui::Text("  Pend KE: %.9f", pendKE * 1000.0);
                ImGui::Text("  Pend PE: %.9f", pendPE * 1000.0);
                ImGui::Text("  Total : %.9f", totalEnergy * 1000.0);

                // Prepare plot data (ordered oldest->newest)
                if (energyCount > 0) {
                    std::vector<float> plotData;
                    plotData.reserve(energyCount);
                    int start = (energyIndex + ENERGY_HISTORY_SIZE - energyCount) % ENERGY_HISTORY_SIZE;
                    for (int i = 0; i < energyCount; ++i) {
                        plotData.push_back(energyHistory[(start + i) % ENERGY_HISTORY_SIZE]);
                    }
                    ImGui::PlotLines("Total Energy (mJ)", plotData.data(), static_cast<int>(plotData.size()), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));
                }

                // (CSV export removed; plotting only)

                ImGui::Separator();
                ImGui::Text("Physics Parameters:");
                // Friction slider with optional manual input toggle
                static bool showFrictionInput = false;
                ImGui::SliderFloat("Friction", &friction, 0.0f, 2.0f);
                ImGui::SameLine();
                if (ImGui::Button(showFrictionInput ? "Hide" : "Edit")) showFrictionInput = !showFrictionInput;
                if (showFrictionInput) {
                    ImGui::NewLine();
                    ImGui::InputFloat("Friction value", &friction, 0.01f, 0.1f, "%.3f");
                }

                // Gravity slider with optional manual input toggle
                static bool showGravityInput = false;
                ImGui::SliderFloat("Gravity", &gravity, 0.0f, 20.0f);
                ImGui::SameLine();
                if (ImGui::Button(showGravityInput ? "Hide##g" : "Edit##g")) showGravityInput = !showGravityInput;
                if (showGravityInput) {
                    ImGui::NewLine();
                    ImGui::InputFloat("Gravity value", &gravity, 0.1f, 1.0f, "%.3f");
                }
                if (ImGui::Button("Reset to Earth Gravity")) {
                    gravity = 9.81f;
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Tuning")) {
                ImGui::Text("Live tuning controls (apply instantly)");
                ImGui::Separator();

                // Cart tuning
                ImGui::Text("Cart / Rail");
                float railLen = static_cast<float>(cart.getRailLength());
                if (ImGui::InputFloat("Rail length (m)", &railLen, 0.1f, 1.0f, "%.2f")) {
                    cart.setRailLength(static_cast<double>(railLen));
                }
                // Wrap-around toggle
                bool wrap = cart.isWrapEnabled();
                if (ImGui::Checkbox("Wrap rail (teleport across edges)", &wrap)) {
                    cart.setWrapEnabled(wrap);
                }
                // Max acceleration tuning (affects input->getCartAcceleration())
                float maxAcc = static_cast<float>(input.getMaxAcceleration());
                if (ImGui::InputFloat("Max acceleration (m/s^2)", &maxAcc, 0.1f, 1.0f, "%.2f")) {
                    input.setMaxAcceleration(static_cast<double>(maxAcc));
                }
                float cartW = static_cast<float>(cart.getWidth());
                float cartH = static_cast<float>(cart.getHeight());
                if (ImGui::InputFloat("Cart width (m)", &cartW, 0.01f, 0.1f, "%.3f")) {
                    cart.setWidth(static_cast<double>(cartW));
                }
                if (ImGui::InputFloat("Cart height (m)", &cartH, 0.01f, 0.1f, "%.3f")) {
                    cart.setHeight(static_cast<double>(cartH));
                }
                float cartM = static_cast<float>(cart.getMass());
                if (ImGui::InputFloat("Cart mass (kg)", &cartM, 0.1f, 1.0f, "%.3f")) {
                    cart.setMass(static_cast<double>(cartM));
                }

                ImGui::Separator();

                // Pendulum tuning
                ImGui::Text("Pendulums");
                if (ImGui::Button(useSinglePendulum ? "Switch to Double" : "Switch to Single")) {
                    useSinglePendulum = !useSinglePendulum;
                    currentPendulum = useSinglePendulum ?
                        static_cast<Pendulum*>(singlePendulum.get()) :
                        static_cast<Pendulum*>(doublePendulum.get());
                    // Reset state on manual UI toggle as well
                    cart.reset();
                    singlePendulum->reset();
                    doublePendulum->reset();
                    simulationTime = 0.0;
                }

                ImGui::Separator();
                // Pendulum tuning - unified controls
                ImGui::Text("Pendulum 1");
                // Use doublePendulum's params as source-of-truth for pendulum 1 so they stay
                // synchronized between single and double modes.
                float p1Mass = static_cast<float>(doublePendulum->getMass(0));
                float p1Length = static_cast<float>(doublePendulum->getLength(0));
                float p1InitDeg = static_cast<float>(doublePendulum->getInitialAngle(0) * 180.0 / 3.14159265358979323846);
                if (ImGui::InputFloat("Pendulum 1 mass (kg)", &p1Mass, 0.01f, 0.1f, "%.3f")) {
                    singlePendulum->setMass(static_cast<double>(p1Mass));
                    doublePendulum->setMass(0, static_cast<double>(p1Mass));
                }
                if (ImGui::InputFloat("Pendulum 1 length (m)", &p1Length, 0.01f, 0.1f, "%.3f")) {
                    singlePendulum->setLength(static_cast<double>(p1Length));
                    doublePendulum->setLength(0, static_cast<double>(p1Length));
                }
                if (ImGui::InputFloat("Pendulum 1 initial angle (deg)", &p1InitDeg, 0.1f, 1.0f, "%.2f")) {
                    double initRad = static_cast<double>(p1InitDeg) * 3.14159265358979323846 / 180.0;
                    singlePendulum->setInitialAngle(initRad);
                    singlePendulum->setAngle(initRad);
                    doublePendulum->setInitialAngle(0, initRad);
                    doublePendulum->setAngle(0, initRad);
                }

                // If in double mode, show Pendulum 2 controls
                if (!useSinglePendulum) {
                    ImGui::Separator();
                    ImGui::Text("Pendulum 2");
                    float p2Mass = static_cast<float>(doublePendulum->getMass(1));
                    float p2Length = static_cast<float>(doublePendulum->getLength(1));
                    float p2InitDeg = static_cast<float>(doublePendulum->getInitialAngle(1) * 180.0 / 3.14159265358979323846);
                    if (ImGui::InputFloat("Pendulum 2 mass (kg)", &p2Mass, 0.01f, 0.1f, "%.3f")) {
                        doublePendulum->setMass(1, static_cast<double>(p2Mass));
                    }
                    if (ImGui::InputFloat("Pendulum 2 length (m)", &p2Length, 0.01f, 0.1f, "%.3f")) {
                        doublePendulum->setLength(1, static_cast<double>(p2Length));
                    }
                    if (ImGui::InputFloat("Pendulum 2 initial angle (deg)", &p2InitDeg, 0.1f, 1.0f, "%.2f")) {
                        double initRad2 = static_cast<double>(p2InitDeg) * 3.14159265358979323846 / 180.0;
                        doublePendulum->setInitialAngle(1, initRad2);
                        doublePendulum->setAngle(1, initRad2);
                    }
                }

                ImGui::Separator();
                if (ImGui::Button("Reset positions")) {
                    cart.reset();
                    singlePendulum->reset();
                    doublePendulum->reset();
                    simulationTime = 0.0;
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ============================================================
    // Cleanup
    // ============================================================
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    // Note: In a full implementation, you'd notify the renderer here
}