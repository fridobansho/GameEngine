#version 130

in vec2 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

out vec4 colour;

uniform sampler2D mySampler;

void main()
{
	vec4 textureColour = texture(mySampler, fragmentUV);

	colour = textureColour * fragmentColour;
}