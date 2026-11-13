#pragma once

#include <memory>

#include <GameEngine/Window.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/InputManager.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/SoA/SpriteFont.h>
#include <GameEngine/AudioEngine.h>
#include <GameEngine/ParticleEngine2D.h>
#include <GameEngine/ParticleBatch2D.h>
#include <GameEngine/VirtualControls.h>

#include "Level.h"
#include "Player.h"
#include "Zombie.h"
#include "Bullet.h"

enum class GameState
{
	PLAY,
	WON,
	LOST,
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

	void initBloodParticles();

	void initLevel();

	void restartLevel();

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

	void addBlood(const glm::vec2& position, int numParticles);

    /// Member Variables
    GameEngine::Window m_window; ///< The game window


	GameEngine::GLSLProgram m_colourProgram;


    GameEngine::InputManager m_inputManager; ///< Handles input

    GameEngine::Camera2D m_camera; ///< Main Camera
	GameEngine::Camera2D m_hudCamera;
	GameEngine::SpriteBatch m_agentSpriteBatch;
	GameEngine::SpriteBatch m_hudSpriteBatch;
	std::vector<std::unique_ptr<Level>> m_levels;
	GameEngine::ParticleEngine2D m_particleEngine;
	std::unique_ptr<GameEngine::ParticleBatch2D> m_bloodParticleBatch;

	int m_screenWidth;
	int m_screenHeight;

	GameState m_gameState;

	float m_fps;

	int m_currentLevel;

	std::vector<std::unique_ptr<Human>> m_humans;
	std::vector<std::unique_ptr<Zombie>> m_zombies;
	std::vector<Bullet> m_bullets;

	Player* m_player;

	int m_numHumansKilled;
	int m_numZombiesKilled;

	std::unique_ptr<GameEngine::SpriteFont> m_spriteFont;
	GameEngine::VirtualControls m_virtualControls;

	GameEngine::AudioEngine m_audioEngine;
};

