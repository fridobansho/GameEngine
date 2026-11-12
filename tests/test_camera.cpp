#include "TestHarness.h"
#include <GameEngine/Camera2D.h>

TEST_CASE(Camera2D_BasicInitAndMatrix) {
    GameEngine::Camera2D camera;
    camera.Init(800, 600);
    camera.SetPosition(glm::vec2(100.0f, 200.0f));
    camera.SetScale(2.0f);
    camera.Update();

    glm::mat4 matrix = camera.getCameraMatrix();
    // Deterministic check that matrix is updated and not identity
    CHECK(matrix != glm::mat4(1.0f));
    CHECK_EQ(camera.GetScale(), 2.0f);
    CHECK_EQ(camera.GetPosition().x, 100.0f);
    CHECK_EQ(camera.GetPosition().y, 200.0f);
}

TEST_CASE(Camera2D_FrustumCulling) {
    GameEngine::Camera2D camera;
    camera.Init(800, 600);
    camera.SetPosition(glm::vec2(0.0f, 0.0f));
    camera.SetScale(1.0f);
    camera.Update();

    // Box at origin should definitely be visible
    CHECK(camera.isBoxInView(glm::vec2(-50.0f, -50.0f), glm::vec2(100.0f, 100.0f)));

    // Box far out on X axis should not be visible
    CHECK(!camera.isBoxInView(glm::vec2(1000.0f, 0.0f), glm::vec2(50.0f, 50.0f)));

    // Box far out on Y axis should not be visible
    CHECK(!camera.isBoxInView(glm::vec2(0.0f, 1000.0f), glm::vec2(50.0f, 50.0f)));

    // Regression test: Box with xDepth > 0 but yDepth < 0 must be out of view
    // (Previously bugged as: xDepth > 0 && xDepth > 0)
    CHECK(!camera.isBoxInView(glm::vec2(0.0f, 800.0f), glm::vec2(50.0f, 50.0f)));

    // Regression test: Box with yDepth > 0 but xDepth < 0 must be out of view
    CHECK(!camera.isBoxInView(glm::vec2(800.0f, 0.0f), glm::vec2(50.0f, 50.0f)));

    // Box straddling the edge boundary should be visible
    CHECK(camera.isBoxInView(glm::vec2(380.0f, 0.0f), glm::vec2(50.0f, 50.0f)));
}

TEST_CASE(Camera2D_ScreenToWorld) {
    GameEngine::Camera2D camera;
    camera.Init(800, 600);
    camera.SetPosition(glm::vec2(0.0f, 0.0f));
    camera.SetScale(1.0f);
    camera.Update();

    // Center of screen (400, 300) maps to camera center (0, 0)
    glm::vec2 worldCenter = camera.convertScreenToWorld(glm::vec2(400.0f, 300.0f));
    CHECK_CLOSE(worldCenter.x, 0.0f, 0.001f);
    CHECK_CLOSE(worldCenter.y, 0.0f, 0.001f);

    // Camera translated to (100, 50)
    camera.SetPosition(glm::vec2(100.0f, 50.0f));
    camera.Update();
    glm::vec2 worldPos = camera.convertScreenToWorld(glm::vec2(400.0f, 300.0f));
    CHECK_CLOSE(worldPos.x, 100.0f, 0.001f);
    CHECK_CLOSE(worldPos.y, 50.0f, 0.001f);
}
