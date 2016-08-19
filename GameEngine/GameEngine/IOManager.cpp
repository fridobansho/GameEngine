#include "IOManager.h"

#include <fstream>

namespace GameEngine {
	bool IOManager::ReadFileToBuffer(const std::string &filePath, std::vector<unsigned char>& buffer)
	{
		std::ifstream file(filePath, std::ios::binary);
		if (file.fail())
		{
			perror(filePath.c_str());
			return false;
		}

		file.seekg(0, std::ios::end);

		int fileSize = (int)file.tellg();

		file.seekg(0, std::ios::beg);

		fileSize -= (int)file.tellg();

		buffer.resize(fileSize);

		file.read((char*)&(buffer[0]), fileSize);

		file.close();

		return true;
	}
}