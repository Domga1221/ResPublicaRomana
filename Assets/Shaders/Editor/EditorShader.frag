#version 460

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoords;

layout(location = 3) uniform sampler2D texture_0;

void main() {
    vec3 color = texture(texture_0, inTexCoords).rgb;
    FragColor = vec4(color.r, color.g, color.b, 1.0f); 
}