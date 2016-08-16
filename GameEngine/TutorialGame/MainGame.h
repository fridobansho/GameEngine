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

	GameEngine::Window _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;
	GameEngine::GLSLProgram _colourProgram;
	GameEngine::Camera2D _camera;
	GameEngine::SpriteBatch _spriteBatch;
	GameEngine::InputManager _inputManager;
	GameEngine::FPSLimiter _fpsLimiter;

	std::vector<Bullet> _bullets;

	float _maxFPS;
	float _fps;
	float _time;
};

