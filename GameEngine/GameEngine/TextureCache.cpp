#include <iostream>
#include "TextureCache.h"
#include "ImageLoader.h"

namespace GameEngine {

	TextureCache::TextureCache()
	{
	}


	TextureCache::~TextureCache()
	{
	}

	GLTexture TextureCache::GetTexture(const std::string & texturePath)
	{
		auto mit = m_textureMap.find(texturePath);

		if (mit == m_textureMap.end())
		{
			GLTexture newTexture = ImageLoader::LoadPNG(texturePath);
			m_textureMap.insert(make_pair(texturePath, newTexture));
			return newTexture;
		}

		return mit->second;
	}
}