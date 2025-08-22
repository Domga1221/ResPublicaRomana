#version 450 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int entityID;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

layout(location = 3) uniform sampler2D diffuse;
layout(location = 4) uniform int u_entityID;
layout(location = 5) uniform bool colorCorrect;

void main() {
	vec3 color = texture(diffuse, TexCoords).rgb;
	
	if(colorCorrect) {
		color = pow(color, vec3(2.2)); 
		color = color * (color + vec3(1.0));
	}

	FragColor = vec4(color, 1.0);

	entityID = u_entityID;
}