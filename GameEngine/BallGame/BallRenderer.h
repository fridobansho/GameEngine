#pragma once

#include <GameEngine/SpriteBatch.h>
#include "Ball.h"

class BallRenderer {
public:
    void renderBall(GameEngine::SpriteBatch& spriteBatch, Ball& ball);
};
