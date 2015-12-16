#include <iostream>
#include <string>

#include "MainGame.h"

void FatalError(std::string errorString)
{
	std::cout << errorString << std::endl;
	system("pause");
	SDL_Quit();
}

MainGame::MainGame() : _window(nullptr), _screenWidth(1024), _screenHeight(768), _gameState(GameState::PLAY)
{
}

MainGame::~MainGame()
{
}

void MainGame::Run()
{
	InitSystems();

	GameLoop();
}

void MainGame::InitSystems()
{
	//Initialise SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	_window = SDL_CreateWindow("Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, SDL_WINDOW_OPENGL);

	if (_window == nullptr)
	{
		FatalError("SDL Window could not be created.");
	}
}

void MainGame::GameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		ProcessInput();
	}
}

void MainGame::ProcessInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			std::cout << event.motion.x << " " << event.motion.y << std::endl;
			break;
		}
	}
}
