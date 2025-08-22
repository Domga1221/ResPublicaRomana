#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_texture;

void main() {
	vec3 color = texture(u_texture, TexCoords).rgb;

    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 
	// HDR tonemapping
    color = vec3(1.0) - (vec3(1.0) / (color + vec3(1.0)));

	FragColor = vec4(color, 1.0);
}