#pragma once

#include <GameEngine/SpriteBatch.h>
#include <GameEngine/GLSLProgram.h>
#include <vector>
#include <memory>
#include "Ball.h"

// Ball renderer interface
class BallRenderer {
public:
    virtual ~BallRenderer() = default;
    virtual void onResize(int /*screenWidth*/, int /*screenHeight*/) {}
    virtual void renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                             const glm::mat4& projectionMatrix);
protected:
    void beginRender(GameEngine::SpriteBatch& spriteBatch, const glm::mat4& projectionMatrix);
    void endRender(GameEngine::SpriteBatch& spriteBatch);

    std::unique_ptr<GameEngine::GLSLProgram> m_program = nullptr;
};

// Visualizes kinetic energy and momentum
class MomentumBallRenderer : public BallRenderer {
public:
    virtual void renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                             const glm::mat4& projectionMatrix) override;
};

// Visualizes velocity direction and speed
class VelocityBallRenderer : public BallRenderer {
public:
    VelocityBallRenderer(int screenWidth, int screenHeight);

    virtual void onResize(int screenWidth, int screenHeight) override;
    virtual void renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                             const glm::mat4& projectionMatrix) override;
private:
    int m_screenWidth;
    int m_screenHeight;
};

// Visualizes position and speed with RGB gradients
class PositionBallRenderer : public BallRenderer {
public:
    PositionBallRenderer(int screenWidth, int screenHeight);

    virtual void onResize(int screenWidth, int screenHeight) override;
    virtual void renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                             const glm::mat4& projectionMatrix) override;
private:
    int m_screenWidth;
    int m_screenHeight;
};

// Trippy renderer!
class TrippyBallRenderer : public BallRenderer {
public:
    TrippyBallRenderer(int screenWidth, int screenHeight);

    virtual void onResize(int screenWidth, int screenHeight) override;
    virtual void renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                             const glm::mat4& projectionMatrix) override;
private:
    int m_screenWidth;
    int m_screenHeight;
    float m_time = 0.0f;
};
