#pragma once

#include <GL/glew.h>
#include <string>
#include "GLTexture.h"

namespace GameEngine {
	class Sprite
	{
	public:
		Sprite();
		~Sprite();

		void Init(float x, float y, float width, float height, const std::string&);
		void Draw();

	private:
		float _x;
		float _y;
		float _width;
		float _height;

		GLuint _vboID;
		GLTexture _texture;
	};

}