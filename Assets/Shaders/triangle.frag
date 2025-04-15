#version 460

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 inColor;

void main() {
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0f);
    FragColor = vec4(inColor, 1.0f);
}