#include <iostream>
#include <string>
#include <SDL.h>
#include <cstdlib>

#include "GameEngineErrors.h"

namespace GameEngine {
	void FatalError(std::string errorString)
	{
		std::cout << errorString << std::endl;
		system("pause");
		SDL_Quit();
		exit(-1);
	}
}