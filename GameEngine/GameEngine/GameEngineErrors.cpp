#include <iostream>
#include <string>
#include <SDL.h>
#include <cstdlib>

#include "GameEngineErrors.h"

namespace GameEngine {
	void FatalError(std::string errorString)
	{
		std::cout << errorString << std::endl;
#ifdef _WIN32
		system("pause");
#else
		std::cout << "Press Enter to quit...";
		std::cin.get();
#endif
		SDL_Quit();
		exit(-1);
	}
}