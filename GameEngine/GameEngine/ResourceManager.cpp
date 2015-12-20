#include "ResourceManager.h"

namespace GameEngine {
	TextureCache ResourceManager::_textureCache;

	GLTexture ResourceManager::GetTexture(const std::string & texturePath)
	{
		return _textureCache.GetTexture(texturePath);
	}
}