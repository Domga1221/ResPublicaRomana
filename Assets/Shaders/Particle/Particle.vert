#version 450
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 ParticleColor;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;
layout(location = 3) uniform vec4 color;
layout(location = 4) uniform bool billboard;

void main() {
	TexCoords = aTexCoords;
	ParticleColor = color;
	vec3 Position = aPosition;

	if(billboard) {
		mat4 viewModel = view * model;

        // TODO: think about different way to do this
		// https://www.reddit.com/r/opengl/comments/bi7fc8/glsl_rotating_a_billboards_xaxis_towards_camera/?share_id=tWmoeZ_MW57mg_N6j_TXX&utm_content=1&utm_medium=android_app&utm_name=androidcss&utm_source=share&utm_term=1
		viewModel[0][0] = 1;
		viewModel[0][1] = 0;
		viewModel[0][2] = 0;

		viewModel[1][0] = 0;
		viewModel[1][1] = 1;
		viewModel[1][2] = 0;

		viewModel[2][0] = 0;
		viewModel[2][1] = 0;
		viewModel[2][2] = 1;

		mat4 scale = mat4(1.0);
		scale[0][0] = model[0][0];
		scale[1][1] = model[1][1];
		scale[2][2] = model[2][2];
		gl_Position = projection * viewModel * scale * vec4(Position, 1.0);
	} else {
		gl_Position = projection * view * model * vec4(aPosition, 1.0);
	}
	
}