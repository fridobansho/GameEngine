#include "ResourceManager.h"

TextureCache ResourceManager::_textureCache;

GLTexture ResourceManager::GetTexture(const std::string & texturePath)
{
	return _textureCache.GetTexture(texturePath);
}
