#include "IOManager.h"

#include <SDL.h>
#include <cstdio>
#include <filesystem>

#if defined(_MSC_VER) && _MSC_VER < 1914
namespace fs = std::tr2::sys;
#else
namespace fs = std::filesystem;
#endif

namespace GameEngine {
	namespace {
		template <typename BufferT>
		bool readFileWithSDL(const std::string& filePath, BufferT& buffer)
		{
			SDL_RWops* rw = SDL_RWFromFile(filePath.c_str(), "rb");
			if (rw == nullptr)
			{
				return false;
			}

			Sint64 fileSize = SDL_RWsize(rw);
			if (fileSize == 0)
			{
				buffer.clear();
				SDL_RWclose(rw);
				return true;
			}

			if (fileSize < 0)
			{
				// Fallback for streams whose size cannot be queried upfront
				buffer.clear();
				char chunk[4096];
				size_t n = 0;
				while ((n = SDL_RWread(rw, chunk, 1, sizeof(chunk))) > 0)
				{
					buffer.insert(buffer.end(), chunk, chunk + n);
				}
				SDL_RWclose(rw);
				return true;
			}

			buffer.resize(static_cast<size_t>(fileSize));
			size_t totalRead = 0;
			while (totalRead < static_cast<size_t>(fileSize))
			{
				size_t bytesRead = SDL_RWread(rw, buffer.data() + totalRead, 1, static_cast<size_t>(fileSize) - totalRead);
				if (bytesRead == 0)
				{
					break;
				}
				totalRead += bytesRead;
			}
			buffer.resize(totalRead);
			SDL_RWclose(rw);
			return true;
		}
	}

	bool IOManager::ReadFileToBuffer(const std::string &filePath, std::vector<unsigned char>& buffer)
	{
		return readFileWithSDL(filePath, buffer);
	}

	bool IOManager::ReadFileToBuffer(const std::string &filePath, std::string& buffer)
	{
		return readFileWithSDL(filePath, buffer);
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