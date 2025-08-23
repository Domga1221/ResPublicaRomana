#version 330 core

out vec4 FragColor;

uniform sampler2D u_texture;

in vec3 v_Position;
in vec2 v_TexCoords;

void main() {
	FragColor = vec4(texture(u_texture, v_TexCoords).rgb, 1.0);
	//FragColor = vec4(vec3(texture(u_texture, v_TexCoords).r), 1.0); // NOTE: for SSAO debug 
}