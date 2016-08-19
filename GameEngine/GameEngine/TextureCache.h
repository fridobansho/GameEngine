#pragma once
#include <map>
#include "GLTexture.h"

namespace GameEngine {
	class TextureCache
	{
	public:
		TextureCache();
		~TextureCache();

		GLTexture GetTexture(const std::string&);

	private:
		std::map<const std::string, GLTexture> m_textureMap;
	};
}
