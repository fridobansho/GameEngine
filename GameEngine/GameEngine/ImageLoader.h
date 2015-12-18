#pragma once
#include <string>
#include "GLTexture.h"

class ImageLoader
{
public:
	static GLTexture LoadPNG(const std::string&);
};

