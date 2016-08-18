#pragma once

#include <GL/glew.h>

namespace GameEngine {
	struct Position
	{
		float x;
		float y;
	};

	struct ColourRGBA8
	{
		ColourRGBA8() : r(0), g(0), b(0), a(0) {}
		ColourRGBA8(GLubyte R, GLubyte G, GLubyte B, GLubyte A) :
			r(R), g(G), b(B), a(A) {}
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

		ColourRGBA8 colour;

		UV uv;

		void setColour(ColourRGBA8 colour)
		{
			setColour(colour.r, colour.g, colour.b, colour.a);
		}
		void setColour(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
		{
			colour = ColourRGBA8(r, g, b, a);
		}

		void setUV(UV uv)
		{
			setUV(uv.u, uv.v);
		}

		void setUV(float u, float v)
		{
			uv.u = u;
			uv.v = v;
		}

		void setPosition(Position position)
		{
			setPosition(position.x, position.y);
		}

		void setPosition(float x, float y)
		{
			position.x = x;
			position.y = y;
		}
	};
}