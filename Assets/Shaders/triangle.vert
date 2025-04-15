#version 460

//layout(location = 0) in vec3 aPos;
//layout(location = 1) in vec3 aColor;

layout(location = 0) out vec3 outColor;

const vec3 positions[] = {
    vec3(0.5f,  -0.5f, 0.0f),
    vec3(0.0f, 0.5f, 0.0f),
    vec3(-0.5f, -0.5f, 0.0f)
};

const vec3 colors[] = {
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f)
};

void main() {
    gl_Position = vec4(positions[gl_VertexID], 1.0);
    //outColor = aColor;
    outColor = colors[gl_VertexID];
}