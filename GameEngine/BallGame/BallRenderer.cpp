#include "BallRenderer.h"
#include <algorithm>
#include <cmath>

void BallRenderer::beginRender(GameEngine::SpriteBatch& spriteBatch, const glm::mat4& projectionMatrix) {
    // Lazily initialize the program
    if (m_program == nullptr) {
        m_program = std::make_unique<GameEngine::GLSLProgram>();
        m_program->CompileShaders("Shaders/colourShading.vert", "Shaders/colourShading.frag");
        m_program->AddAttribute("vertexPosition");
        m_program->AddAttribute("vertexColour");
        m_program->AddAttribute("vertexUV");
        m_program->LinkShaders();
    }
    m_program->Use();

    spriteBatch.begin();

    // Make sure the shader uses texture 0
    glActiveTexture(GL_TEXTURE0);
    GLint textureUniform = m_program->GetUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);

    // Grab the camera matrix
    GLint pUniform = m_program->GetUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void BallRenderer::endRender(GameEngine::SpriteBatch& spriteBatch) {
    spriteBatch.end();
    spriteBatch.renderBatch();

    m_program->UnUse();
}

void BallRenderer::renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                               const glm::mat4& projectionMatrix) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    beginRender(spriteBatch, projectionMatrix);

    // Render all the balls
    for (auto& ball : balls) {
        const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        const glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
                                 ball.radius * 2.0f, ball.radius * 2.0f);
        spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, ball.colour);
    }

    endRender(spriteBatch);
}

void MomentumBallRenderer::renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                                       const glm::mat4& projectionMatrix) {
    glClearColor(0.0f, 0.05f, 0.2f, 1.0f);
    beginRender(spriteBatch, projectionMatrix);

    // Render all the balls colored by momentum / kinetic energy gradient
    for (auto& ball : balls) {
        const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        const glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
                                 ball.radius * 2.0f, ball.radius * 2.0f);
        const float speed = glm::length(ball.velocity);
        const float momentum = speed * ball.mass;
        const float kineticEnergy = 0.5f * ball.mass * speed * speed;
        const float t = glm::clamp(momentum * 0.06f + kineticEnergy * 0.015f, 0.0f, 1.0f);

        GameEngine::ColourRGBA8 colour;
        colour.r = (GLubyte)(glm::clamp(t * 2.0f, 0.0f, 1.0f) * 255.0f);
        colour.g = (GLubyte)(glm::clamp(std::sin(t * 3.1415926f), 0.1f, 1.0f) * 255.0f);
        colour.b = (GLubyte)(glm::clamp(1.0f - t, 0.0f, 1.0f) * 255.0f);
        colour.a = 255;
        spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, colour);
    }

    endRender(spriteBatch);
}

VelocityBallRenderer::VelocityBallRenderer(int screenWidth, int screenHeight) :
    m_screenWidth(screenWidth),
    m_screenHeight(screenHeight) {
    // Empty
}

void VelocityBallRenderer::onResize(int screenWidth, int screenHeight) {
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
}

void VelocityBallRenderer::renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                                       const glm::mat4& projectionMatrix) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    beginRender(spriteBatch, projectionMatrix);

    // Render all the balls colored by velocity direction and speed
    for (auto& ball : balls) {
        const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        const glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
                                 ball.radius * 2.0f, ball.radius * 2.0f);
        const float speed = glm::length(ball.velocity);
        const float nx = (speed > 0.0001f) ? (ball.velocity.x / speed) : 0.0f;
        const float ny = (speed > 0.0001f) ? (ball.velocity.y / speed) : 0.0f;
        const float intensity = glm::clamp(speed / 6.0f, 0.25f, 1.0f);

        GameEngine::ColourRGBA8 colour;
        colour.r = (GLubyte)(glm::clamp((nx * 0.5f + 0.5f) * intensity, 0.0f, 1.0f) * 255.0f);
        colour.g = (GLubyte)(glm::clamp((ny * 0.5f + 0.5f) * intensity, 0.0f, 1.0f) * 255.0f);
        colour.b = (GLubyte)(glm::clamp(1.0f - intensity * 0.5f, 0.0f, 1.0f) * 255.0f);
        colour.a = 255;
        spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, colour);
    }

    endRender(spriteBatch);
}

PositionBallRenderer::PositionBallRenderer(int screenWidth, int screenHeight) :
    m_screenWidth(screenWidth),
    m_screenHeight(screenHeight) {
    // Empty
}

void PositionBallRenderer::onResize(int screenWidth, int screenHeight) {
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
}

void PositionBallRenderer::renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                                       const glm::mat4& projectionMatrix) {
    glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
    beginRender(spriteBatch, projectionMatrix);

    const float invWidth = 1.0f / (float)std::max(1, m_screenWidth);
    const float invHeight = 1.0f / (float)std::max(1, m_screenHeight);

    // Render all the balls with RGB gradients mapped from position + speed
    for (auto& ball : balls) {
        const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        const glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
                                 ball.radius * 2.0f, ball.radius * 2.0f);
        const float normX = glm::clamp(ball.position.x * invWidth, 0.0f, 1.0f);
        const float normY = glm::clamp(ball.position.y * invHeight, 0.0f, 1.0f);
        const float speedBoost = glm::clamp(glm::length(ball.velocity) * 0.15f, 0.0f, 1.0f);

        GameEngine::ColourRGBA8 colour;
        colour.r = (GLubyte)(glm::clamp(normX * 0.85f + speedBoost * 0.15f, 0.0f, 1.0f) * 255.0f);
        colour.g = (GLubyte)(glm::clamp(normY * 0.85f + speedBoost * 0.15f, 0.0f, 1.0f) * 255.0f);
        colour.b = (GLubyte)(glm::clamp((1.0f - 0.5f * (normX + normY)) * 0.7f + speedBoost * 0.3f, 0.0f, 1.0f) * 255.0f);
        colour.a = 255;
        spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, colour);
    }

    endRender(spriteBatch);
}

TrippyBallRenderer::TrippyBallRenderer(int screenWidth, int screenHeight) :
    m_screenWidth(screenWidth),
    m_screenHeight(screenHeight) {
    // Empty
}

void TrippyBallRenderer::onResize(int screenWidth, int screenHeight) {
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
}

void TrippyBallRenderer::renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls, const glm::mat4& projectionMatrix) {
    glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
    beginRender(spriteBatch, projectionMatrix);

    // Change these constants to get cool stuff
    float TIME_SPEED = 0.01f;
    float DIVISOR = 4.0f; // Increase to get more arms
    float SPIRAL_INTENSITY = 10.0f; // Increase to make it spiral more

    m_time += TIME_SPEED;

    // Render all the balls
    for (auto& ball : balls) {
        const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        const glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
                                 ball.radius * 2.0f, ball.radius * 2.0f);
        GameEngine::ColourRGBA8 colour;
       
        // Get vector from center point
        glm::vec2 centerVec = ball.position - glm::vec2(m_screenWidth / 2, m_screenHeight / 2);
        float centerDist = glm::length(centerVec);

        // Get angle from the horizontal
        float angle = atan2(centerVec.x, centerVec.y) / (3.1415926f / DIVISOR);
        // Move with time
        angle -= m_time;
        // Add the spiral
        angle += (centerDist / std::max(1, m_screenWidth)) * SPIRAL_INTENSITY;

        colour.r = (GLubyte)(angle * 255.0f);
        colour.g = (GLubyte)(angle * 255.0f * cos(m_time));
        colour.b = (GLubyte)(angle * 255.0f * sin(m_time));
        colour.a = (GLubyte)(glm::clamp(1.0f - (centerDist / std::max(1.0f, m_screenWidth / 2.0f)), 0.0f, 1.0f) * 255.0f);
        spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, colour);
    }

    endRender(spriteBatch);
}
