#pragma once
#include <map>
#include "GLTexture.h"

class TextureCache
{
public:
	TextureCache();
	~TextureCache();

	GLTexture GetTexture(const std::string&);

private:
	std::map<const std::string, GLTexture> _textureMap;
};

