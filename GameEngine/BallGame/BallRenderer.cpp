#include "BallRenderer.h"

void BallRenderer::renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                               const glm::mat4& projectionMatrix) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

    // Render all the balls
    for (auto& ball : balls) {
        const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        const glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
                                 ball.radius * 2.0f, ball.radius * 2.0f);
        spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, ball.colour);
    }

    spriteBatch.end();
    spriteBatch.renderBatch();

    m_program->UnUse();
}

void MomentumBallRenderer::renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                               const glm::mat4& projectionMatrix) {

    glClearColor(0.0f, 0.1f, 0.5f, 1.0f);

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

    // Render all the balls
    for (auto& ball : balls) {
        const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        const glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
                                 ball.radius * 2.0f, ball.radius * 2.0f);
        GameEngine::ColourRGBA8 colour;
        GLubyte colourVal = (GLubyte)(glm::clamp(glm::length(ball.velocity) * ball.mass * 12.0f, 0.0f, 255.0f));
        colour.r = colourVal;
        colour.g = colourVal;
        colour.b = colourVal;
        colour.a = colourVal;
        spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, colour);
    }

    spriteBatch.end();
    spriteBatch.renderBatch();

    m_program->UnUse();
}

VelocityBallRenderer::VelocityBallRenderer(int screenWidth, int screenHeight) :
    m_screenWidth(screenWidth),
    m_screenHeight(screenHeight) {
    // Empty
}


void VelocityBallRenderer::renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls,
                                       const glm::mat4& projectionMatrix) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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

    // Render all the balls
    for (auto& ball : balls) {
        const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        const glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
                                 ball.radius * 2.0f, ball.radius * 2.0f);
        GameEngine::ColourRGBA8 colour;

        float mult = 100.0f;
        GLubyte colourVal = (GLubyte)(glm::clamp(ball.velocity.x * mult, 0.0f, 255.0f));
        colour.r = 128;
        colour.g = (GLubyte)((ball.position.x / m_screenWidth) * 255.0f);
        colour.b = (GLubyte)((ball.position.y / m_screenHeight) * 255.0f);
        colour.a = colourVal;
        spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, colour);
    }

    spriteBatch.end();
    spriteBatch.renderBatch();

    m_program->UnUse();
}

TrippyBallRenderer::TrippyBallRenderer(int screenWidth, int screenHeight) :
    m_screenWidth(screenWidth),
    m_screenHeight(screenHeight) {
    // Empty
}

void TrippyBallRenderer::renderBalls(GameEngine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls, const glm::mat4& projectionMatrix) {
    glClearColor(0.1f, 0.0f, 0.0f, 1.0f);

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
        angle += (centerDist / m_screenWidth) * SPIRAL_INTENSITY;

        colour.r = (GLubyte)(angle * 255.0f);
        colour.g = (GLubyte)(angle * 255.0f * cos(m_time));
        colour.b = (GLubyte)(angle * 255.0f * sin(m_time));
        colour.a = (GLubyte)(glm::clamp(1.0f - (centerDist / (m_screenWidth / 2.0f)), 0.0f, 1.0f) * 255.0f);
        spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, colour);
    }

    spriteBatch.end();
    spriteBatch.renderBatch();

    m_program->UnUse();
}
