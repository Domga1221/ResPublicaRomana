#version 330 core
out vec4 FragColor;

in vec3 v_Position;
in vec4 v_Color;

void main() {
	FragColor = v_Color;
}

