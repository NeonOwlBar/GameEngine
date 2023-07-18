#version 140

// Input to fragment shader
in vec2 uv;

// Fog  fragment shader attribute
in float fog_amount;

// Fog fragment shader uniform
uniform vec3 fog_colour;

// Texture
uniform sampler2D sampler;

// Final fragment colour
out vec4 fragmentColour;

// Shader body
void main()
{
	// Determine fragment colour from texture based on uv co-oords
	vec4 colour = texture(sampler, uv);
	if (colour.a < 0.9)
	{
		discard;
	}
	fragmentColour = mix(colour, vec4(fog_colour, 1.0f), fog_amount);
}

