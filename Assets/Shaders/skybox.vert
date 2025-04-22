#version 460 core

layout(location = 0) in vec3 aPosition;

layout(location = 0) out vec3 TexCoords;

layout(location = 0) uniform mat4 projection;
layout(location = 1) uniform mat4 view;

void main() {
    TexCoords = aPosition;
    vec4 position = projection * view * vec4(aPosition, 1.0);
    gl_Position = position.xyww;
}