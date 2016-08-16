#include <iostream>
#include <string>

#include "MainGame.h"
#include <GameEngine/Sprite.h>
#include <GameEngine/Errors.h>
#include <GameEngine/GameEngine.h>
#include <GameEngine/ResourceManager.h>
#include <GameEngine/Camera2D.h>

using namespace GameEngine;

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

	GameLoop();
}

void MainGame::InitSystems()
{
	GameEngine::Init();

	_window.Create("Game Engine", _screenWidth, _screenHeight, 0);

	InitShaders();

	_spriteBatch.init();

	_fpsLimiter.init(_maxFPS);
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
		_fpsLimiter.begin();
		ProcessInput();
		_time += 0.01f;

		_camera.Update();
		DrawGame();

		_fps = _fpsLimiter.end();

		static int frameCounter = 0;
		frameCounter++;

		if (frameCounter == 10)
		{
			frameCounter = 0;
			std::cout << _fps << std::endl;
		}
	}
}

void MainGame::ProcessInput()
{
	SDL_Event event;

	const float CAMERA_SPEED = 2.0f;
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
			_inputManager.pressKey(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			_inputManager.releaseKey(event.key.keysym.sym);
			break;
		}
	}

	if(_inputManager.isKeyPressed(SDLK_w))
		_camera.SetPosition(_camera.GetPosition() + glm::vec2(0.0f, CAMERA_SPEED));

	if (_inputManager.isKeyPressed(SDLK_s))
		_camera.SetPosition(_camera.GetPosition() + glm::vec2(0.0f, -CAMERA_SPEED));

	if (_inputManager.isKeyPressed(SDLK_a))
		_camera.SetPosition(_camera.GetPosition() + glm::vec2(-CAMERA_SPEED, 0.0f));

	if (_inputManager.isKeyPressed(SDLK_d))
		_camera.SetPosition(_camera.GetPosition() + glm::vec2(CAMERA_SPEED, 0.0f));

	if (_inputManager.isKeyPressed(SDLK_q))
		_camera.SetScale(_camera.GetScale() + SCALE_SPEED);

	if (_inputManager.isKeyPressed(SDLK_e))
		_camera.SetScale(_camera.GetScale() - SCALE_SPEED);
}

void MainGame::DrawGame()
{
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_colourProgram.Use();

	glActiveTexture(GL_TEXTURE0);
	GLint textureLocation = _colourProgram.GetUniformLocation("mySampler");
	glUniform1i(textureLocation, 0);

	GLint pLocation = _colourProgram.GetUniformLocation("P");

	glm::mat4 cameraMatrix = _camera.getCameraMatrix();

	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	_spriteBatch.begin();

	glm::vec4 position(0.0f, 0.0f, 50.0f, 50.0f);
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);

	static GLTexture texture = ResourceManager::GetTexture("Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");
	Colour colour;
	colour.r = 255;
	colour.g = 255;
	colour.b = 255;
	colour.a = 255;

	_spriteBatch.draw(position, uv, texture.id, 0.0f, colour);
	_spriteBatch.draw(position + glm::vec4(50, 0, 0, 0), uv, texture.id, 0.0f, colour);

	_spriteBatch.end();

	_spriteBatch.renderBatch();

	glBindTexture(GL_TEXTURE_2D, 0);

	_colourProgram.UnUse();

	_window.Swap();
}
