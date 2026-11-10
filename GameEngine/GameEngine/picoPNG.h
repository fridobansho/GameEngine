#pragma once
#include <vector>
#include <cstddef>

namespace GameEngine {
	using std::size_t;
	extern int decodePNG(std::vector<unsigned char>& out_image, unsigned long& imagem_width, unsigned long& imagem_height, const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);
}