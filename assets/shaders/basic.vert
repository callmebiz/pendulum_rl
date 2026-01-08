#version 430 core

// Input: vertex position and color
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

// Output: color to fragment shader
out vec3 vertexColor;

// Transformation matrices
uniform mat4 projection;
uniform mat4 model;

void main()
{
    // Transform vertex position
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
    
    // Pass color through
    vertexColor = aColor;
}