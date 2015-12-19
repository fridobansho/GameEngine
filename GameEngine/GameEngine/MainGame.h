#pragma once

#include <SDL.h>
#include <GL/glew.h>
#include <vector>

#include "GLSLProgram.h"
#include "Sprite.h"
#include "GLTexture.h"

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

	void CalculateFPS();

	SDL_Window* _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;
	std::vector<Sprite*> _sprites;
	GLSLProgram _colourProgram;
	float _time;
	float _fps;
	float _maxFPS;
	float _frameTime;
};

