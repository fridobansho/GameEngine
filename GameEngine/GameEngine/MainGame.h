#pragma once

#include <SDL.h>
#include <GL/glew.h>

class MainGame
{
public:
	MainGame();
	~MainGame();

	void Run();

private:
	void InitSystems();

	SDL_Window* _window;
	int _screenWidth;
	int _screenHeight;
};

