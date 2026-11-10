#include "IOManager.h"

#include <cstdio>
#include <filesystem>
#include <fstream>

#if defined(_MSC_VER) && _MSC_VER < 1914
namespace fs = std::tr2::sys;
#else
namespace fs = std::filesystem;
#endif

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

	bool IOManager::ReadFileToBuffer(const std::string &filePath, std::string& buffer)
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

	bool IOManager::getDirectoryEntries(const char* path, std::vector<DirEntry>& rvEntries)
	{
		auto dpath = fs::path(path);
		if (!fs::is_directory(dpath)) return false;

		for (auto it = fs::directory_iterator(dpath); it != fs::directory_iterator(); ++it)
		{
			rvEntries.emplace_back();
			rvEntries.back().path = it->path().string();
			if (is_directory(it->path()))
			{
				rvEntries.back().isDirectory = true;
			}
			else
			{
				rvEntries.back().isDirectory = false;
			}
		}
		return true;
	}

	bool IOManager::makeDirectory(const char* path)
	{
		return fs::create_directory(fs::path(path));
	}
}