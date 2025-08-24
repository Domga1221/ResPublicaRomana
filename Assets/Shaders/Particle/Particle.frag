#version 450 core
layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;
in vec4 ParticleColor;

layout(location = 5) uniform sampler2D sprite;
layout(location = 6) uniform bool colorCorrect;

void main() {
	vec4 texColor = texture(sprite, TexCoords).rgba;

	if(colorCorrect) {
		// gamma correct
		texColor.rgb = pow(texColor.rgb, vec3(1.0/2.2)); 
		// HDR tonemapping
		texColor.rgb = vec3(1.0) - (vec3(1.0) / (texColor.rgb + vec3(1.0)));
	}	

	FragColor =  texColor * vec4(ParticleColor);
	//FragColor = vec4(ParticleColor, 0.5f);
	//FragColor = vec4(1.0f);
}