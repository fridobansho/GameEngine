#pragma once

#include <GL/glew.h>

struct Position
{
	float x;
	float y;
};

struct Colour
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
};

struct UV
{
	float u;
	float v;
};

struct Vertex
{
	Position position;

	Colour colour;

	UV uv;

	void setColour(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
	{
		colour.r = r;
		colour.g = g;
		colour.b = b;
		colour.a = a;
	}

	void setUV(float u, float v)
	{
		uv.u = u;
		uv.v = v;
	}

	void setPosition(float x, float y)
	{
		position.x = x;
		position.y = y;
	}
};