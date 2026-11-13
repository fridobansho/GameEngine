#pragma once
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif
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