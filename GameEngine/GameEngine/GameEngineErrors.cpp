#include <iostream>
#include <string>
#include <SDL.h>
#include <cstdlib>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "GameEngineErrors.h"

namespace GameEngine {
	void FatalError(std::string errorString)
	{
		std::cout << errorString << std::endl;
#ifdef _WIN32
		system("pause");
#else
		if (isatty(fileno(stdin))) {
			std::cout << "Press Enter to quit...";
			std::cin.get();
		}
#endif
		SDL_Quit();
		exit(-1);
	}
}