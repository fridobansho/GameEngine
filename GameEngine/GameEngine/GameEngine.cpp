#include <SDL.h>
#include <GL/glew.h>
#include "GameEngine.h"

namespace GameEngine {
	int Init()
	{
		//Initialise SDL
		SDL_Init(SDL_INIT_EVERYTHING);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		return 0;
	}
}