#pragma once
#include <vector>
#include <string>

namespace GameEngine {

	struct DirEntry {
		std::string path;
		bool isDirectory;
	};

	class IOManager
	{
	public:
		static bool ReadFileToBuffer(const std::string& filePath, std::vector<unsigned char>& buffer);
		static bool ReadFileToBuffer(const std::string& filePath, std::string& buffer);
		static bool getDirectoryEntries(const char* path, std::vector<DirEntry>& rvEntries);
		static bool makeDirectory(const char* path);
	};
}
