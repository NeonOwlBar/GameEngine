#version 140

// Inputs to vertex shader
in vec3 vertexPos3D;
in vec2 vUV;

// Output
out vec2 uv;

// MVP transformations
uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

// fog attributes
out float fog_amount;

// exponential fog
uniform float fog_density;

// Shader main code
void main() {
	// Convert input vertex to 4D 
	vec4 v = vec4(vertexPos3D.xyz, 1);
	vec4 posInWorld = transform * v;
	v = projection * view * posInWorld;

	// Pass transformed vertex to next stage
	gl_Position = v;

	// As well as uv.  Don't transform uv
	uv = vUV;

	// Fog calculation
	vec4 pos_rel_eye = view * posInWorld;
	float distance = length(pos_rel_eye.xyz);

	// Exponential fog
	fog_amount = 1.0 - exp(-pow(distance * fog_density, 2.0f));
	fog_amount = clamp(fog_amount, 0.0f, 1.0f);
}