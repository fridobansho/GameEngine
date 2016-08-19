#include <iostream>
#include <string>
#include <algorithm>

#include "MainGame.h"
#include <GameEngine/Sprite.h>
#include <GameEngine/GameEngineErrors.h>
#include <GameEngine/GameEngine.h>
#include <GameEngine/ResourceManager.h>
#include <GameEngine/Camera2D.h>

using namespace GameEngine;

MainGame::MainGame() :
	m_screenWidth(1024),
	m_screenHeight(768),
	m_gameState(GameState::PLAY),
	_time(0.0f),
	m_maxFPS(60.0f)
{
	m_camera.Init(m_screenWidth, m_screenHeight);
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

	m_window.Create("Game Engine", m_screenWidth, m_screenHeight, 0);

	InitShaders();

	m_spriteBatch.init();

	m_fpsLimiter.init(m_maxFPS);
}

void MainGame::InitShaders()
{
	m_colourProgram.CompileShaders("Shaders\\colourShading.vert", "Shaders\\colourShading.frag");
	m_colourProgram.AddAttribute("vertexPosition");
	m_colourProgram.AddAttribute("vertexColour");
	m_colourProgram.AddAttribute("vertexUV");
	m_colourProgram.LinkShaders();
}

void MainGame::GameLoop()
{
	while (m_gameState != GameState::EXIT)
	{
		m_fpsLimiter.begin();
		ProcessInput();
		_time += 0.01f;

		m_camera.Update();

		for (int i = 0; i < m_bullets.size();)
		{
			if (m_bullets[i].update())
			{
				m_bullets[i] = m_bullets.back();
				m_bullets.pop_back();
			}
			else
				i++;
		}

		DrawGame();

		m_fps = m_fpsLimiter.end();

		static int frameCounter = 0;
		frameCounter++;

		if (frameCounter == 10000)
		{
			frameCounter = 0;
			std::cout << m_fps << std::endl;
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
			m_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			m_inputManager.setMouseCoords(event.motion.x, event.motion.y);
			break;
		case SDL_KEYDOWN:
			m_inputManager.pressKey(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			m_inputManager.releaseKey(event.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			m_inputManager.pressKey(event.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			m_inputManager.releaseKey(event.button.button);
			break;
		}
	}

	if(m_inputManager.isKeyDown(SDLK_w))
		m_camera.SetPosition(m_camera.GetPosition() + glm::vec2(0.0f, CAMERA_SPEED));

	if (m_inputManager.isKeyDown(SDLK_s))
		m_camera.SetPosition(m_camera.GetPosition() + glm::vec2(0.0f, -CAMERA_SPEED));

	if (m_inputManager.isKeyDown(SDLK_a))
		m_camera.SetPosition(m_camera.GetPosition() + glm::vec2(-CAMERA_SPEED, 0.0f));

	if (m_inputManager.isKeyDown(SDLK_d))
		m_camera.SetPosition(m_camera.GetPosition() + glm::vec2(CAMERA_SPEED, 0.0f));

	if (m_inputManager.isKeyDown(SDLK_q))
		m_camera.SetScale(m_camera.GetScale() + SCALE_SPEED);

	if (m_inputManager.isKeyDown(SDLK_e))
		m_camera.SetScale(m_camera.GetScale() - SCALE_SPEED);

	if (m_inputManager.isKeyDown(SDL_BUTTON_LEFT))
	{
		glm::vec2 mouseCoords = m_inputManager.getMouseCoords();
		mouseCoords = m_camera.convertScreenToWorld(mouseCoords);

		glm::vec2 playerPosition(0.0f);

		glm::vec2 direction = mouseCoords - playerPosition;

		direction = glm::normalize(direction);

		m_bullets.emplace_back(playerPosition, direction, 5.00f, 1000);
	}
}

void MainGame::DrawGame()
{
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_colourProgram.Use();

	glActiveTexture(GL_TEXTURE0);
	GLint textureLocation = m_colourProgram.GetUniformLocation("mySampler");
	glUniform1i(textureLocation, 0);

	GLint pLocation = m_colourProgram.GetUniformLocation("P");

	glm::mat4 cameraMatrix = m_camera.getCameraMatrix();

	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	m_spriteBatch.begin();

	glm::vec4 position(0.0f, 0.0f, 50.0f, 50.0f);
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);

	static GLTexture texture = ResourceManager::GetTexture("Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");
	ColourRGBA8 colour(255, 255, 255, 255);

	m_spriteBatch.draw(position, uv, texture.id, 0.0f, colour);

	for (int i = 0; i < m_bullets.size(); i++)
	{
		m_bullets[i].draw(m_spriteBatch);
	}

	m_spriteBatch.end();

	m_spriteBatch.renderBatch();

	glBindTexture(GL_TEXTURE_2D, 0);

	m_colourProgram.UnUse();

	m_window.Swap();
}
