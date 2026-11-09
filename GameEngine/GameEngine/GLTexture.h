#pragma once
#include <GL/glew.h>
#include <string>

namespace GameEngine {
	class GLTexture
	{
	public:
		std::string filePath = "";
		GLuint id;
		int width;
		int height;
	};
}