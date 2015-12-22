#version 130

in vec2 vertexPosition;
in vec4 vertexColour;
in vec2 vertexUV;

out vec4 fragmentColour;
out vec2 fragmentPosition;
out vec2 fragmentUV;

uniform mat4 P;

void main()
{
	gl_Position.xy = (P * vec4(vertexPosition, 0.0, 1.0)).xy;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;

	fragmentPosition = vertexPosition;

	fragmentColour = vertexColour;

	fragmentUV = vec2(vertexUV.x, 1.0 - vertexUV.y);
}