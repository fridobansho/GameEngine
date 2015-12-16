#include "MainGame.h"

MainGame::MainGame() : _window(nullptr), _screenWidth(1024), _screenHeight(768)
{
}

MainGame::~MainGame()
{
}

void MainGame::Run()
{
	InitSystems();
}

void MainGame::InitSystems()
{
	//Initialise SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	_window = SDL_CreateWindow("Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, SDL_WINDOW_OPENGL);
}
