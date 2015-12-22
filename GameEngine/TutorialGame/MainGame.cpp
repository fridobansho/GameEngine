#include <iostream>
#include <string>

#include "MainGame.h"
#include <GameEngine/Sprite.h>
#include <GameEngine/Errors.h>
#include <GameEngine/GameEngine.h>

#include <GameEngine/Camera2D.h>


MainGame::MainGame() :
	_screenWidth(1024),
	_screenHeight(768),
	_gameState(GameState::PLAY),
	_time(0.0f),
	_maxFPS(60.0f)
{
	_camera.Init(_screenWidth, _screenHeight);
}

MainGame::~MainGame()
{
}

void MainGame::Run()
{
	InitSystems();

	_sprites.push_back(new  GameEngine::Sprite());
	_sprites.back()->Init(0.0f, 0.0f, _screenWidth / 2, _screenWidth / 2, "Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");

	_sprites.push_back(new GameEngine::Sprite());
	_sprites.back()->Init(_screenWidth / 2, 0.0f, _screenWidth / 2, _screenWidth / 2, "Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");

	GameLoop();
}

void MainGame::InitSystems()
{
	GameEngine::Init();

	_window.Create("Game Engine", _screenWidth, _screenHeight, 0);

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
		float startTicks = SDL_GetTicks();
		ProcessInput();
		_time += 0.01f;

		_camera.Update();
		DrawGame();
		CalculateFPS();

		static int frameCounter = 0;
		frameCounter++;

		if (frameCounter == 10)
		{
			frameCounter = 0;
			std::cout << _fps << std::endl;
		}

		float frameTicks = SDL_GetTicks() - startTicks;

		if ((1000.0f / _maxFPS) > frameTicks)
		{
			SDL_Delay((1000.0f / _maxFPS) - frameTicks);
		}
	}
}

void MainGame::CalculateFPS()
{
	static const int NUM_SAMPLES = 10;
	static float frameTimes[NUM_SAMPLES];
	static int currentFrame = 0;

	static float prevTicks = SDL_GetTicks();
	float currentTicks = SDL_GetTicks();

	_frameTime = currentTicks - prevTicks;
	frameTimes[currentFrame % NUM_SAMPLES] = _frameTime;

	prevTicks = currentTicks;

	int count;

	currentFrame++;

	if (currentFrame < NUM_SAMPLES)
	{
		count = currentFrame;
	}
	else
	{
		count = NUM_SAMPLES;
	}

	float frameTimeAverage = 0.0f;

	for (int i = 0; i < count; i++)
	{
		frameTimeAverage += frameTimes[i];
	}
	frameTimeAverage /= count;

	if (frameTimeAverage > 0)
	{
		_fps = 1000.0f / frameTimeAverage;
	}
	else
	{
		_fps = 60.0f;
	}

}

void MainGame::ProcessInput()
{
	SDL_Event event;

	const float CAMERA_SPEED = 20.0f;
	const float SCALE_SPEED = 0.1f;

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
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				_camera.SetPosition(_camera.GetPosition() + glm::vec2(0.0f, CAMERA_SPEED));
				break;
			case SDLK_s:
				_camera.SetPosition(_camera.GetPosition() + glm::vec2(0.0f, -CAMERA_SPEED));
				break;
			case SDLK_a:
				_camera.SetPosition(_camera.GetPosition() + glm::vec2(-CAMERA_SPEED, 0.0f));
				break;
			case SDLK_d:
				_camera.SetPosition(_camera.GetPosition() + glm::vec2(CAMERA_SPEED, 0.0f));
				break;
			case SDLK_q:
				_camera.SetScale(_camera.GetScale() + SCALE_SPEED);
				break;
			case SDLK_e:
				_camera.SetScale(_camera.GetScale() - SCALE_SPEED);
				break;
			}
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

	GLint pLocation = _colourProgram.GetUniformLocation("P");

	glm::mat4 cameraMatrix = _camera.getCameraMatrix();

	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	for (int i = 0; i < _sprites.size(); i++)
	{
		_sprites[i]->Draw();
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	_colourProgram.UnUse();

	_window.Swap();
}
