#pragma once
#include <string>
#include "TextureCache.h"

namespace GameEngine {
	class ResourceManager
	{
	public:
		static GLTexture GetTexture(const std::string&);

	private:
		static TextureCache m_textureCache;
	};

}