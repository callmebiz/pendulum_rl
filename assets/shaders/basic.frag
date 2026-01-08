#version 430 core

// Input: color from vertex shader
in vec3 vertexColor;

// Output: final pixel color
out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
}