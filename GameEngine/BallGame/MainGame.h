#pragma once

#include <GameEngine/Camera2D.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/InputManager.h>
#include <GameEngine/Window.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Timing.h>
#include <GameEngine/SoA/SpriteFont.h>
#include <memory>

#include "BallController.h"
#include "BallRenderer.h"
#include "Grid.h"

enum class GameState { RUNNING, EXIT };

const int CELL_SIZE = 12;

class MainGame {
public:
	~MainGame();
    void run();

private:
    void init();
    void initBalls();
    void update(float deltaTime);
    void draw();
    void drawHud();
    void processInput();

    int m_screenWidth = 0;
    int m_screenHeight = 0;

    std::vector<Ball> m_balls; ///< All the balls
	std::unique_ptr<Grid> m_grid;

    BallController m_ballController; ///< Controls balls
    BallRenderer m_ballRenderer; ///< Renders balls

    GameEngine::Window m_window; ///< The main window
    GameEngine::SpriteBatch m_spriteBatch; ///< Renders all the balls
    std::unique_ptr<GameEngine::SpriteFont> m_spriteFont; ///< For font rendering
    GameEngine::Camera2D m_camera; ///< Renders the scene
    GameEngine::InputManager m_inputManager; ///< Handles input
    GameEngine::GLSLProgram m_textureProgram; ///< Shader for textures]

    GameEngine::FPSLimiter m_fpsLimiter; ///< Limits and calculates fps
    float m_fps = 0.0f;

    GameState m_gameState = GameState::RUNNING; ///< The state of the game
};

