#pragma once
#include <vector>

class IOManager
{
public:
	static bool ReadFileToBuffer(const std::string&, std::vector<char>&);
};

