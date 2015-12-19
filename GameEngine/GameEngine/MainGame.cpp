#include <iostream>
#include <string>

#include "MainGame.h"
#include "Sprite.h"
#include "Errors.h"


MainGame::MainGame() :
	_window(nullptr),
	_screenWidth(1024),
	_screenHeight(768),
	_gameState(GameState::PLAY),
	_time(0.0f)
{
}

MainGame::~MainGame()
{
}

void MainGame::Run()
{
	InitSystems();

	_sprites.push_back(new  Sprite());
	_sprites.back()->Init(-1.0f, -1.0f, 1.0f, 1.0f, "Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");

	_sprites.push_back(new Sprite());
	_sprites.back()->Init(0.0f, -1.0f, 1.0f, 1.0f, "Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");

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

	SDL_GLContext glContext = SDL_GL_CreateContext(_window);

	if (glContext == nullptr)
	{
		FatalError("SDL GL Context could not be created.");
	}

	GLenum error = glewInit();

	if (error != GLEW_OK)
	{
		FatalError("Could not initialise glew.");
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	InitShaders();
}

void MainGame::InitShaders()
{
	_colourProgram.CompileShaders("Shaders\\colourShading.vert", "Shaders\\colourShading.frag");
	_colourProgram.AddAttribute("vertexPosition");
	_colourProgram.AddAttribute("vertexColour");
	_colourProgram.AddAttribute("vertexUV");
	_colourProgram.LinkShaders();
}

void MainGame::GameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		ProcessInput();
		_time += 0.01f;
		DrawGame();
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
			//std::cout << event.motion.x << " " << event.motion.y << std::endl;
			break;
		}
	}
}

void MainGame::DrawGame()
{
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_colourProgram.Use();

	glActiveTexture(GL_TEXTURE0);
	GLint textureLocation = _colourProgram.GetUniformLocation("mySampler");
	glUniform1i(textureLocation, 0);

	GLint timeLocation = _colourProgram.GetUniformLocation("time");

	glUniform1f(timeLocation, _time);

	for (int i = 0; i < _sprites.size(); i++)
	{
		_sprites[i]->Draw();
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	_colourProgram.UnUse();

	SDL_GL_SwapWindow(_window);
}
