#pragma once

#include <SDL.h>
#include <GL/glew.h>
#include <vector>

#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Sprite.h>
#include <GameEngine/GLTexture.h>
#include <GameEngine/Window.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/InputManager.h>
#include <GameEngine/Timing.h>

#include "Bullet.h"

enum class GameState
{
	PLAY,
	EXIT
};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void Run();

private:
	void InitSystems();
	void InitShaders();
	void ProcessInput();
	void DrawGame();
	void GameLoop();

	GameEngine::Window m_window;
	int m_screenWidth;
	int m_screenHeight;
	GameState m_gameState;
	GameEngine::GLSLProgram m_colourProgram;
	GameEngine::Camera2D m_camera;
	GameEngine::SpriteBatch m_spriteBatch;
	GameEngine::InputManager m_inputManager;
	GameEngine::FPSLimiter m_fpsLimiter;

	std::vector<Bullet> m_bullets;

	float m_maxFPS;
	float m_fps;
	float _time;
};

