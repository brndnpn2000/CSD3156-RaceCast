#version 310 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aTexIndex; // Passed as float from VBO

out vec4 vColor;
out vec2 vTexCoords;
out float vTexIndex;

void main() {
    vColor = aColor;
    vTexCoords = aTexCoords;
    vTexIndex = aTexIndex;
    
    // Standard NDC transformation (you can multiply by a projection matrix here later)
    gl_Position = vec4(aPos, 1.0);
}