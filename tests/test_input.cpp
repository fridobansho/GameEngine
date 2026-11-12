#include "TestHarness.h"
#include <GameEngine/InputManager.h>
#include <SDL.h>

TEST_CASE(InputManager_KeyPressAndRelease) {
    GameEngine::InputManager input;

    CHECK(!input.isKeyDown(SDLK_SPACE));
    CHECK(!input.isKeyPressed(SDLK_SPACE));

    // Press key
    input.pressKey(SDLK_SPACE);
    CHECK(input.isKeyDown(SDLK_SPACE));
    CHECK(input.isKeyPressed(SDLK_SPACE));

    // Advance frame
    input.update();
    CHECK(input.isKeyDown(SDLK_SPACE));
    CHECK(!input.isKeyPressed(SDLK_SPACE)); // No longer just pressed

    // Release key
    input.releaseKey(SDLK_SPACE);
    CHECK(!input.isKeyDown(SDLK_SPACE));
    CHECK(!input.isKeyPressed(SDLK_SPACE));

    // Advance frame
    input.update();
    CHECK(!input.isKeyDown(SDLK_SPACE));
    CHECK(!input.isKeyPressed(SDLK_SPACE));
}

TEST_CASE(InputManager_MouseCoords) {
    GameEngine::InputManager input;

    input.setMouseCoords(320.5f, 240.25f);
    glm::vec2 coords = input.getMouseCoords();
    CHECK_CLOSE(coords.x, 320.5f, 0.001f);
    CHECK_CLOSE(coords.y, 240.25f, 0.001f);
}
