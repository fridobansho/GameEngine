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
		auto mit = _textureMap.find(texturePath);

		if (mit == _textureMap.end())
		{
			GLTexture newTexture = ImageLoader::LoadPNG(texturePath);
			_textureMap.insert(make_pair(texturePath, newTexture));
			std::cout << "Loaded Texture" << std::endl;
			return newTexture;
		}

		std::cout << "Used Cached Texture" << std::endl;
		return mit->second;
	}
}