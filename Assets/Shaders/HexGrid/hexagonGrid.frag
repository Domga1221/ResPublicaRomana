#version 460 core 

layout(location = 0) out vec4 FragColor;

layout(location = 3) uniform vec3 hexColor;

void main() {
    FragColor = vec4(hexColor, 1.0);
}