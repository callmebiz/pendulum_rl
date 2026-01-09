cmake_minimum_required(VERSION 3.15)
project(PendulumML VERSION 1.0.0 LANGUAGES C CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add external libraries
add_subdirectory(external/glfw)
add_subdirectory(external/glm)

# Eigen is header-only
set(EIGEN3_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/eigen)

# Build GLAD as a library
add_library(glad STATIC
    external/glad/src/glad.c
    external/glad/include/glad/glad.h
)
target_include_directories(glad PUBLIC external/glad/include)

# Build ImGui as a library
file(GLOB IMGUI_SOURCES 
    external/imgui/*.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC 
    external/imgui
    external/imgui/backends
    external/glfw/include
)
target_link_libraries(imgui PRIVATE glfw)

# Main executable
add_executable(PendulumML
    src/main.cpp
    src/Shader.cpp
    src/Renderer.cpp
    src/Cart.cpp
    src/SinglePendulum.cpp
    src/DoublePendulum.cpp
    src/InputController.cpp
    src/ODESolver.cpp
)

# Link libraries
target_link_libraries(PendulumML PRIVATE
    glfw
    glad
    imgui
)

# Include directories
target_include_directories(PendulumML PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${EIGEN3_INCLUDE_DIR}
    external/glm
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(PendulumML PRIVATE opengl32)
endif()

# Copy shaders to build directory
add_custom_command(TARGET PendulumML POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets/shaders
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/shaders
)