#version 430 core

// Input: color from vertex shader (ignored in favor of uniform color)
in vec3 vertexColor;

// Uniform color (set per-draw by CPU)
uniform vec3 uColor;

// Output: final pixel color
out vec4 FragColor;

void main()
{
    // Use uniform color for consistent coloring; vertexColor can be used
    // for subtle per-vertex tinting if desired. For now, prefer uColor.
    FragColor = vec4(uColor, 1.0);
}