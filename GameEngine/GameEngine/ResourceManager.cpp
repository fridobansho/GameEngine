#include "ResourceManager.h"

namespace GameEngine {
	TextureCache ResourceManager::m_textureCache;

	GLTexture ResourceManager::GetTexture(const std::string & texturePath)
	{
		return m_textureCache.GetTexture(texturePath);
	}
}