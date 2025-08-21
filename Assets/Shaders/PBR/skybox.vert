#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 projection;
uniform mat4 view;

out vec3 localPos;

void main() {
	localPos = aPosition;

	mat4 rotView = mat4(mat3(view)); // remove translation from view matrix
	vec4 clipPos = projection * rotView * vec4(localPos, 1.0);

	gl_Position = clipPos.xyww; // depth value of the rendered cube fragments always end up at 1.0
}