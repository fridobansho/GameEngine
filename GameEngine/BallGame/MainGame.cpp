#define _CRT_SECURE_NO_WARNINGS // To shut up the compiler about sprintf...
#include "MainGame.h"

#include <GameEngine/GameEngine.h>
#include <GameEngine/ResourceManager.h>
#include <SDL.h>
#include <random>
#include <ctime>
#include <algorithm>
#include <cmath>

// Some helpful constants.
const float DESIRED_FPS = 60.0f; // FPS the game is designed to run at
const int MAX_PHYSICS_STEPS = 6; // Max number of physics steps per frame
const float MS_PER_SECOND = 1000; // Number of milliseconds in a second
const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS; // The desired frame time per frame
const float MAX_DELTA_TIME = 1.0f; // Maximum size of deltaTime

MainGame::~MainGame()
{

}

void MainGame::run() {
    init();
    initBalls();

    // Start our previousTicks variable
    Uint32 previousTicks = SDL_GetTicks();

    // Game loop
    while (m_gameState == GameState::RUNNING) {
        m_fpsLimiter.begin();
        processInput();

        // Calculate the frameTime in milliseconds
        Uint32 newTicks = SDL_GetTicks();
        Uint32 frameTime = newTicks - previousTicks;
        previousTicks = newTicks; // Store newTicks in previousTicks so we can use it next frame
        // Get the total delta time
        float totalDeltaTime = (float)frameTime / DESIRED_FRAMETIME;

        int i = 0; // This counter makes sure we don't spiral to death!
        // Loop while we still have steps to process.
        while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS) {
            // The deltaTime should be the the smaller of the totalDeltaTime and MAX_DELTA_TIME
            float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
            // Update all physics here and pass in deltaTime

            update(deltaTime);

            // Since we just took a step that is length deltaTime, subtract from totalDeltaTime
            totalDeltaTime -= deltaTime;
            // Increment our frame counter so we can limit steps to MAX_PHYSICS_STEPS
            i++;
        }

        m_camera.Update();
        draw();
        m_fps = m_fpsLimiter.end();
    }
}

void MainGame::init() {
    GameEngine::Init();

    m_screenWidth = 1024;
    m_screenHeight = 768;

    m_window.Create("Ball Game", m_screenWidth, m_screenHeight, 0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    m_camera.Init(m_screenWidth, m_screenHeight);
    // Point the camera to the center of the screen
    m_camera.SetPosition(glm::vec2(m_screenWidth / 2.0f, m_screenHeight / 2.0f));

    m_spriteBatch.init();
    // Initialize sprite font
    m_spriteFont = std::make_unique<GameEngine::SpriteFont>("Fonts/chintzy.ttf", 40);

    // Compile our texture shader
    m_textureProgram.CompileShaders("Shaders/colourShading.vert", "Shaders/colourShading.frag");
    m_textureProgram.AddAttribute("vertexPosition");
    m_textureProgram.AddAttribute("vertexColour");
    m_textureProgram.AddAttribute("vertexUV");
    m_textureProgram.LinkShaders();

    m_fpsLimiter.setMaxFPS(60.0f);

}

struct BallSpawn {
    BallSpawn(const GameEngine::ColourRGBA8& colr,
              float rad, float m, float minSpeed,
              float maxSpeed, float prob) :
              colour(colr),
              radius(rad),
              mass(m),
              randSpeed(minSpeed, maxSpeed),
              probability(prob) {
        // Empty
    }
    GameEngine::ColourRGBA8 colour;
    float radius;
    float mass;
    float probability;
    std::uniform_real_distribution<float> randSpeed;
};
#include <iostream>
void MainGame::initBalls() {

	m_grid = std::make_unique<Grid>(m_screenWidth, m_screenHeight, CELL_SIZE);

#define ADD_BALL(p, ...) \
    totalProbability += p; \
    possibleBalls.emplace_back(__VA_ARGS__);

    // Number of balls to spawn
    const int NUM_BALLS = 10000;

    // Random engine stuff
    std::mt19937 randomEngine((unsigned int)time(nullptr));
    std::uniform_real_distribution<float> randX(0.0f, (float)m_screenWidth);
    std::uniform_real_distribution<float> randY(0.0f, (float)m_screenHeight);
    std::uniform_real_distribution<float> randDir(-1.0f, 1.0f);

    // Add all possible balls
    std::vector <BallSpawn> possibleBalls;
    float totalProbability = 0.0f;

    // Adds the balls using a macro
    ADD_BALL(20.0f, GameEngine::ColourRGBA8(255, 255, 255, 255),
             1.0f, 1.0f, 0.1f, 7.0f, totalProbability);
    ADD_BALL(10.0f, GameEngine::ColourRGBA8(0, 0, 255, 255),
             2.0f, 2.0f, 0.1f, 3.0f, totalProbability);
    ADD_BALL(1.0f, GameEngine::ColourRGBA8(255, 0, 0, 255),
             3.0f, 4.0f, 0.0f, 0.0f, totalProbability)

    // Random probability for ball spawn
    std::uniform_real_distribution<float> spawn(0.0f, totalProbability);

    // Small optimization that sets the size of the internal array to prevent
    // extra allocations.
    m_balls.reserve(NUM_BALLS);

    // Set up ball to spawn with default value
    BallSpawn* ballToSpawn = &possibleBalls[0];
    for (int i = 0; i < NUM_BALLS; i++) {
        // Get the ball spawn roll
        float spawnVal = spawn(randomEngine);
        // Figure out which ball we picked
        for (size_t j = 0; j < possibleBalls.size(); j++) {
            if (spawnVal <= possibleBalls[j].probability) {
                ballToSpawn = &possibleBalls[j];
                break;
            }
        }

        // Get random starting position
        glm::vec2 pos(randX(randomEngine), randY(randomEngine));

        // Hacky way to get a random direction
        glm::vec2 direction(randDir(randomEngine), randDir(randomEngine));
        if (direction.x != 0.0f || direction.y != 0.0f) { // The chances of direction == 0 are astronomically low
            direction = glm::normalize(direction);
        } else {
            direction = glm::vec2(1.0f, 0.0f); // default direction
        }

        // Add ball
        m_balls.emplace_back(ballToSpawn->radius, ballToSpawn->mass, pos, direction * ballToSpawn->randSpeed(randomEngine),
                             GameEngine::ResourceManager::GetTexture("Textures/circle.png").id,
                             ballToSpawn->colour);
		m_grid->addBall(&m_balls.back());
    }
}

void MainGame::update(float deltaTime) {
    m_ballController.updateBalls(m_balls, m_grid.get(), deltaTime, m_screenWidth, m_screenHeight);
}

void MainGame::draw() {
    // Set the base depth to 1.0
    glClearDepth(1.0);
    // Clear the colour and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_textureProgram.Use();

    glActiveTexture(GL_TEXTURE0);

    // Make sure the shader uses texture 0
    GLint textureUniform = m_textureProgram.GetUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);

    // Grab the camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.GetUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    m_spriteBatch.begin();

    // Draw balls
    for (auto& ball : m_balls) {
        m_ballRenderer.renderBall(m_spriteBatch, ball);
    }

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();

    drawHud();

    m_textureProgram.UnUse();

    m_window.Swap();
}

void MainGame::drawHud() {
    const GameEngine::ColourRGBA8 fontColour(255, 0, 0, 255);
    // Convert float to char *
    char buffer[64];
    sprintf(buffer, "%.1f", m_fps);

    m_spriteBatch.begin();
    m_spriteFont->draw(m_spriteBatch, buffer, glm::vec2(0.0f, m_screenHeight - 32.0f),
                       glm::vec2(1.0f), 0.0f, fontColour);
    m_spriteBatch.end();
    m_spriteBatch.renderBatch();
}

void MainGame::processInput() {
    SDL_Event evnt;
    //Will keep looping until there are no more events to process
    while (SDL_PollEvent(&evnt)) {
        switch (evnt.type) {
            case SDL_QUIT:
                m_gameState = GameState::EXIT;
                break;
            case SDL_MOUSEMOTION:
                m_ballController.onMouseMove(m_balls, (float)evnt.motion.x, (float)m_screenHeight - (float)evnt.motion.y);
                m_inputManager.setMouseCoords((float)evnt.motion.x, (float)evnt.motion.y);
                break;
            case SDL_KEYDOWN:
                m_inputManager.pressKey(evnt.key.keysym.sym);
                break;
            case SDL_KEYUP:
                m_inputManager.releaseKey(evnt.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                m_ballController.onMouseDown(m_balls, (float)evnt.button.x, (float)m_screenHeight - (float)evnt.button.y);
                m_inputManager.pressKey(evnt.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                m_ballController.onMouseUp(m_balls);
                m_inputManager.releaseKey(evnt.button.button);
                break;
        }
    }

    if (m_inputManager.isKeyPressed(SDLK_ESCAPE)) {
        m_gameState = GameState::EXIT;
    }
    // Handle gravity changes
    if (m_inputManager.isKeyPressed(SDLK_LEFT)) {
        m_ballController.setGravityDirection(GravityDirection::LEFT);
    } else if (m_inputManager.isKeyPressed(SDLK_RIGHT)) {
        m_ballController.setGravityDirection(GravityDirection::RIGHT);
    } else if (m_inputManager.isKeyPressed(SDLK_UP)) {
        m_ballController.setGravityDirection(GravityDirection::UP);
    } else if (m_inputManager.isKeyPressed(SDLK_DOWN)) {
        m_ballController.setGravityDirection(GravityDirection::DOWN);
    } else if (m_inputManager.isKeyPressed(SDLK_SPACE)) {
        m_ballController.setGravityDirection(GravityDirection::NONE);
    }
}