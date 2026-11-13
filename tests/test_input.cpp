#include "TestHarness.h"
#include <GameEngine/InputManager.h>
#include <GameEngine/VirtualControls.h>
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

TEST_CASE(InputManager_TouchAndPinch) {
    GameEngine::InputManager input;
    const float w = 800.0f;
    const float h = 600.0f;

    CHECK(!input.isTouchActive());
    CHECK_EQ(input.getActiveFingers().size(), 0u);
    CHECK_CLOSE(input.getPinchDelta(), 0.0f, 0.0001f);

    // Single-finger down maps to mouse coords + SDL_BUTTON_LEFT
    input.onTouchDown(1, 0.25f, 0.5f, w, h);
    CHECK(input.isTouchActive());
    CHECK_EQ(input.getActiveFingers().size(), 1u);
    CHECK_CLOSE(input.getMouseCoords().x, 200.0f, 0.01f);
    CHECK_CLOSE(input.getMouseCoords().y, 300.0f, 0.01f);
    CHECK(input.isKeyDown(SDL_BUTTON_LEFT));
    CHECK(input.isKeyPressed(SDL_BUTTON_LEFT));

    // Single-finger motion updates mouse coords and finger delta
    input.onTouchMotion(1, 0.5f, 0.5f, 0.25f, 0.0f, w, h);
    CHECK_CLOSE(input.getMouseCoords().x, 400.0f, 0.01f);
    CHECK_CLOSE(input.getMouseCoords().y, 300.0f, 0.01f);
    CHECK_CLOSE(input.getActiveFingers()[0].delta.x, 200.0f, 0.01f);

    input.update();
    CHECK(input.isKeyDown(SDL_BUTTON_LEFT));
    CHECK(!input.isKeyPressed(SDL_BUTTON_LEFT));
    CHECK_CLOSE(input.getActiveFingers()[0].delta.x, 0.0f, 0.001f);

    // Two-finger pinch out -> positive pinch delta
    input.onTouchDown(2, 0.75f, 0.5f, w, h);
    CHECK_EQ(input.getActiveFingers().size(), 2u);
    input.onTouchMotion(2, 0.9f, 0.5f, 0.15f, 0.0f, w, h);
    CHECK(input.getPinchDelta() > 0.0f);

    // update() resets pinch delta
    input.update();
    CHECK_CLOSE(input.getPinchDelta(), 0.0f, 0.0001f);

    // Pinch in -> negative pinch delta
    input.onTouchMotion(2, 0.65f, 0.5f, -0.25f, 0.0f, w, h);
    CHECK(input.getPinchDelta() < 0.0f);

    // Lift fingers
    input.onTouchUp(2, 0.65f, 0.5f, w, h);
    CHECK(input.isTouchActive());
    CHECK_EQ(input.getActiveFingers().size(), 1u);

    input.onTouchUp(1, 0.5f, 0.5f, w, h);
    CHECK(!input.isTouchActive());
    CHECK_EQ(input.getActiveFingers().size(), 0u);
    CHECK(!input.isKeyDown(SDL_BUTTON_LEFT));
}

TEST_CASE(VirtualControls_DPadAndButtons) {
    const int sw = 1280;
    const int sh = 720;

    auto makeFingerEvent = [](Uint32 type, int64_t fingerId, float x, float y) {
        SDL_Event ev{};
        ev.type = type;
        ev.tfinger.fingerId = fingerId;
        ev.tfinger.x = x;
        ev.tfinger.y = y;
        return ev;
    };

    // 1. PLATFORMER layout: virtual D-pad (< / >) + Jump (W) + Punch (SPACE)
    {
        GameEngine::InputManager input;
        GameEngine::VirtualControls vc;
        vc.init(GameEngine::VirtualControlLayout::PLATFORMER);
        vc.setEnabled(true);

        const auto& buttons = vc.getButtons();
        CHECK(buttons.size() >= 4u);

        auto findCenter = [&](unsigned int keyCode) -> glm::vec2 {
            for (const auto& b : buttons) {
                if (b.keyCode == keyCode) {
                    return glm::vec2(b.normRect.x + b.normRect.z * 0.5f,
                                     b.normRect.y + b.normRect.w * 0.5f);
                }
            }
            return glm::vec2(-1.0f);
        };

        glm::vec2 leftPos = findCenter(SDLK_a);
        glm::vec2 rightPos = findCenter(SDLK_d);
        glm::vec2 jumpPos = findCenter(SDLK_w);
        glm::vec2 punchPos = findCenter(SDLK_SPACE);
        CHECK(leftPos.x >= 0.0f);
        CHECK(rightPos.x >= 0.0f);
        CHECK(jumpPos.x >= 0.0f);
        CHECK(punchPos.x >= 0.0f);

        // Press Left (<)
        SDL_Event evDown = makeFingerEvent(SDL_FINGERDOWN, 10, leftPos.x, leftPos.y);
        CHECK(vc.onSDLEvent(evDown, input, sw, sh));
        CHECK(input.isKeyDown(SDLK_a));
        CHECK(input.isKeyPressed(SDLK_a));

        // Press Jump (W) simultaneously with a second finger
        SDL_Event evJumpDown = makeFingerEvent(SDL_FINGERDOWN, 11, jumpPos.x, jumpPos.y);
        CHECK(vc.onSDLEvent(evJumpDown, input, sw, sh));
        CHECK(input.isKeyDown(SDLK_a));
        CHECK(input.isKeyDown(SDLK_w));

        // Release Left and Jump
        SDL_Event evUp = makeFingerEvent(SDL_FINGERUP, 10, leftPos.x, leftPos.y);
        CHECK(vc.onSDLEvent(evUp, input, sw, sh));
        CHECK(!input.isKeyDown(SDLK_a));
        CHECK(input.isKeyDown(SDLK_w));

        SDL_Event evJumpUp = makeFingerEvent(SDL_FINGERUP, 11, jumpPos.x, jumpPos.y);
        CHECK(vc.onSDLEvent(evJumpUp, input, sw, sh));
        CHECK(!input.isKeyDown(SDLK_w));

        // Press and release Right (>) and Punch (SPACE)
        SDL_Event evRightDown = makeFingerEvent(SDL_FINGERDOWN, 12, rightPos.x, rightPos.y);
        CHECK(vc.onSDLEvent(evRightDown, input, sw, sh));
        CHECK(input.isKeyDown(SDLK_d));
        SDL_Event evRightUp = makeFingerEvent(SDL_FINGERUP, 12, rightPos.x, rightPos.y);
        CHECK(vc.onSDLEvent(evRightUp, input, sw, sh));
        CHECK(!input.isKeyDown(SDLK_d));

        SDL_Event evPunchDown = makeFingerEvent(SDL_FINGERDOWN, 13, punchPos.x, punchPos.y);
        CHECK(vc.onSDLEvent(evPunchDown, input, sw, sh));
        CHECK(input.isKeyDown(SDLK_SPACE));
        SDL_Event evPunchUp = makeFingerEvent(SDL_FINGERUP, 13, punchPos.x, punchPos.y);
        CHECK(vc.onSDLEvent(evPunchUp, input, sw, sh));
        CHECK(!input.isKeyDown(SDLK_SPACE));
    }

    // 2. TWIN_STICK layout: movement stick injects A/D/W and aim stick triggers aiming + SDL_BUTTON_LEFT
    {
        GameEngine::InputManager input;
        GameEngine::VirtualControls vc;
        vc.init(GameEngine::VirtualControlLayout::TWIN_STICK);
        vc.setEnabled(true);

        CHECK_EQ(vc.getSticks().size(), 2u);
        const glm::vec2 moveCenter = vc.getSticks()[0].normCenter;
        const glm::vec2 aimCenter = vc.getSticks()[1].normCenter;

        // Touch move stick deflected right and up (remember top-left Y: smaller Y is up -> W)
        SDL_Event stickDown = makeFingerEvent(SDL_FINGERDOWN, 20, moveCenter.x + 0.05f, moveCenter.y - 0.05f);
        CHECK(vc.onSDLEvent(stickDown, input, sw, sh));
        CHECK(input.isKeyDown(SDLK_d));
        CHECK(input.isKeyDown(SDLK_w));
        CHECK(!input.isKeyDown(SDLK_a));

        // Move stick to the left
        SDL_Event stickMove = makeFingerEvent(SDL_FINGERMOTION, 20, moveCenter.x - 0.06f, moveCenter.y);
        CHECK(vc.onSDLEvent(stickMove, input, sw, sh));
        CHECK(input.isKeyDown(SDLK_a));
        CHECK(!input.isKeyDown(SDLK_d));
        CHECK(!input.isKeyDown(SDLK_w));

        // Release move stick
        SDL_Event stickUp = makeFingerEvent(SDL_FINGERUP, 20, moveCenter.x, moveCenter.y);
        CHECK(vc.onSDLEvent(stickUp, input, sw, sh));
        CHECK(!input.isKeyDown(SDLK_a));

        // Touch aim stick deflected right -> aiming + SDL_BUTTON_LEFT
        SDL_Event aimDown = makeFingerEvent(SDL_FINGERDOWN, 21, aimCenter.x + 0.06f, aimCenter.y);
        CHECK(vc.onSDLEvent(aimDown, input, sw, sh));
        CHECK(vc.isAiming());
        CHECK(vc.getAimDirection().x > 0.9f);
        CHECK(input.isKeyDown(SDL_BUTTON_LEFT));

        SDL_Event aimUp = makeFingerEvent(SDL_FINGERUP, 21, aimCenter.x, aimCenter.y);
        CHECK(vc.onSDLEvent(aimUp, input, sw, sh));
        CHECK(!vc.isAiming());
        CHECK(!input.isKeyDown(SDL_BUTTON_LEFT));
    }
}
