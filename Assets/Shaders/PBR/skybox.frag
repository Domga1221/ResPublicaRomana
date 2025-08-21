#version 330 core

out vec4 FragColor;

in vec3 localPos;

uniform samplerCube environmentMap;
uniform bool colorCorrect;

void main() {
	vec3 envColor = textureLod(environmentMap, localPos, 1.2).rgb; // specular prefilter
	
	// HDR tonemap and gamma correct
	// TODO: COMMENT OUT AND DO POSTPROCESS
	if(colorCorrect) {
		envColor = envColor / (envColor + vec3(1.0));
		envColor = pow(envColor, vec3(1.0/2.2)); 
	}

	FragColor = vec4(envColor, 1.0);
	//FragColor = vec4(vec3(1.0, 0.5, 0.5), 1.0);
}