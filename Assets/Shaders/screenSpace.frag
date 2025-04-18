#version 460 core 

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() {
    vec3 color = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}