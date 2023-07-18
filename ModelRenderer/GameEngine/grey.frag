#version 140

// Input attributes
in vec2 texCoords;

// Output fragment colour
out vec4 fragColour;

// Original texture to sample from
uniform sampler2D sceneTexture;

uniform bool apply_greyscale;

void main() {
	// Get colour for fragment from texture
	vec4 sceneColour = texture(sceneTexture, texCoords);

	if(apply_greyscale) {
		// Calculate greyscale value
		float greyScale = dot(sceneColour.rgb, vec3(0.3f, 0.59f, 0.11f));

		// Output fragment in greyscale
		fragColour = vec4(greyScale, greyScale, greyScale, sceneColour.a);
	} else {
	    fragColour = sceneColour;
	}
}

