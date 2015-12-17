#include <iostream>
#include <string>
#include <SDL.h>
#include <cstdlib>

#include "Errors.h"

void FatalError(std::string errorString)
{
	std::cout << errorString << std::endl;
	system("pause");
	SDL_Quit();
	exit(-1);
}