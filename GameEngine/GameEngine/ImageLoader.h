#pragma once
#include <string>
#include "GLTexture.h"

namespace GameEngine {
	class ImageLoader
	{
	public:
		static GLTexture LoadPNG(const std::string&);
	};

}