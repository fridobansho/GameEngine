#pragma once

#include <GameEngine/Window.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/InputManager.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/SoA/SpriteFont.h>

#include "Level.h"
#include "Player.h"
#include "Zombie.h"
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

    /// Runs the game
    void Run();

private:
    /// Initializes the core systems
    void initSystems();

	void initLevel();

    /// Initializes the shaders
    void initShaders();

    /// Main game loop for the program
    void gameLoop();

	void updateAgents(float deltaTime);

	void updateBullets(float deltaTime);

	void checkVictory();

    /// Handles input processing
    void processInput();

    /// Renders the game
    void drawGame();

	void drawHud();

    /// Member Variables
    GameEngine::Window _window; ///< The game window


	GameEngine::GLSLProgram _colourProgram;


    GameEngine::InputManager _inputManager; ///< Handles input

    GameEngine::Camera2D _camera; ///< Main Camera
	GameEngine::Camera2D _hudCamera;
	GameEngine::SpriteBatch _agentSpriteBatch;
	GameEngine::SpriteBatch _hudSpriteBatch;
	std::vector<Level*> _levels;

	int _screenWidth;
	int _screenHeight;

	GameState _gameState;

	float _fps;

	int _currentLevel;

	std::vector<Human*> _humans;
	std::vector<Zombie*> _zombies;
	std::vector<Bullet> _bullets;

	Player* _player;

	int _numHumansKilled;
	int _numZombiesKilled;

	GameEngine::SpriteFont* _spriteFont;
};

