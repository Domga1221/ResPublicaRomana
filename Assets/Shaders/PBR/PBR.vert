#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;
layout(location = 3) uniform mat4 lightSpaceMatrix;

out vec3 WorldPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

void main() {
	WorldPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;
	FragPosLightSpace = lightSpaceMatrix * vec4(WorldPos, 1.0);
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}