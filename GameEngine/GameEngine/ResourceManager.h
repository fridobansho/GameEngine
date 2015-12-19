#pragma once
#include <string>
#include "TextureCache.h"

class ResourceManager
{
public:
	static GLTexture GetTexture(const std::string&);

private:
	static TextureCache _textureCache;
};

