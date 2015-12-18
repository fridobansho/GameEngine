#version 130

in vec2 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

out vec4 colour;

uniform float time;
uniform sampler2D mySampler;

void main()
{
	vec4 textureColour = texture(mySampler, fragmentUV);

	colour = textureColour * vec4(fragmentColour.r * (cos(fragmentPosition.x * 4.0 + time) + 1.0) * 0.5,
				  fragmentColour.g * (cos(fragmentPosition.y * 8.0 + time) + 1.0) * 0.5,
				  fragmentColour.b * (cos(fragmentPosition.x * 2.0 + time) + 1.0) * 0.5, fragmentColour.a);
}