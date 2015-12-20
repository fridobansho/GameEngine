#pragma once
#include <vector>

namespace GameEngine {
	class IOManager
	{
	public:
		static bool ReadFileToBuffer(const std::string&, std::vector<unsigned char>&);
	};
}
