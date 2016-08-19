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
		float m_x;
		float m_y;
		float m_width;
		float m_height;

		GLuint m_vboID = 0;
		GLTexture m_texture;
	};

}