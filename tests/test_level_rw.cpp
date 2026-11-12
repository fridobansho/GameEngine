#include "TestHarness.h"
#include <NinjaPlatformer/LevelReaderWriter.h>
#include <GameEngine/ResourceManager.h>
#include <Box2D/Box2D.h>
#include <cstdio>

TEST_CASE(LevelReaderWriter_RoundTripText) {
    if (!TestHarness::hasGLContext()) {
        // Box and Player initialization requires OpenGL texture IDs from ResourceManager
        return;
    }

    b2Vec2 gravity(0.0f, -9.8f);
    b2World world(gravity);

    Player player;
    player.init(&world, glm::vec2(5.0f, 10.0f), glm::vec2(2.0f, 2.0f), glm::vec2(1.0f, 1.8f),
                GameEngine::ColourRGBA8(255, 255, 255, 255));

    GameEngine::GLTexture brickTex = GameEngine::ResourceManager::GetTexture("Assets/bricks_top.png");
    std::vector<Box> boxes;
    Box b1;
    b1.init(&world, glm::vec2(0.0f, 0.0f), glm::vec2(10.0f, 1.0f), brickTex,
            GameEngine::ColourRGBA8(100, 150, 200, 255), false, false, 0.0f, glm::vec4(0, 0, 1, 1));
    boxes.push_back(b1);

    Box b2;
    b2.init(&world, glm::vec2(3.0f, 4.0f), glm::vec2(2.0f, 2.0f), brickTex,
            GameEngine::ColourRGBA8(255, 0, 0, 255), true, true, 0.5f, glm::vec4(0, 0, 1, 1));
    boxes.push_back(b2);

    std::vector<Light> lights;
    Light l1;
    l1.position = glm::vec2(2.0f, 5.0f);
    l1.size = 15.0f;
    l1.colour = GameEngine::ColourRGBA8(200, 220, 240, 180);
    lights.push_back(l1);

    const std::string tmpPath = "/tmp/test_level_roundtrip.txt";
    bool saveOk = LevelReaderWriter::saveAsText(tmpPath, player, boxes, lights);
    CHECK(saveOk);

    // Load back into new world
    b2World loadWorld(gravity);
    Player loadedPlayer;
    std::vector<Box> loadedBoxes;
    std::vector<Light> loadedLights;

    bool loadOk = LevelReaderWriter::loadAsText(tmpPath, &loadWorld, loadedPlayer, loadedBoxes, loadedLights);
    CHECK(loadOk);

    // Verify player
    CHECK_CLOSE(loadedPlayer.getPosition().x, 5.0f, 0.01f);
    CHECK_CLOSE(loadedPlayer.getPosition().y, 10.0f, 0.01f);
    CHECK_CLOSE(loadedPlayer.getDrawDims().x, 2.0f, 0.01f);
    CHECK_CLOSE(loadedPlayer.getDrawDims().y, 2.0f, 0.01f);

    // Verify boxes
    CHECK_EQ(loadedBoxes.size(), 2);
    if (loadedBoxes.size() == 2) {
        CHECK_CLOSE(loadedBoxes[0].getPosition().x, 0.0f, 0.01f);
        CHECK_CLOSE(loadedBoxes[0].getDimensions().x, 10.0f, 0.01f);
        CHECK(!loadedBoxes[0].getIsDynamic());

        CHECK_CLOSE(loadedBoxes[1].getPosition().x, 3.0f, 0.01f);
        CHECK_CLOSE(loadedBoxes[1].getDimensions().x, 2.0f, 0.01f);
        CHECK(loadedBoxes[1].getIsDynamic());
    }

    // Verify lights
    CHECK_EQ(loadedLights.size(), 1);
    if (loadedLights.size() == 1) {
        CHECK_CLOSE(loadedLights[0].position.x, 2.0f, 0.01f);
        CHECK_CLOSE(loadedLights[0].size, 15.0f, 0.01f);
        CHECK_EQ(loadedLights[0].colour.r, 200);
        CHECK_EQ(loadedLights[0].colour.g, 220);
        CHECK_EQ(loadedLights[0].colour.b, 240);
        CHECK_EQ(loadedLights[0].colour.a, 180);
    }

    std::remove(tmpPath.c_str());
}

TEST_CASE(LevelReaderWriter_LoadShippedLevel) {
    if (!TestHarness::hasGLContext()) return;

    b2Vec2 gravity(0.0f, -9.8f);
    b2World world(gravity);
    Player player;
    std::vector<Box> boxes;
    std::vector<Light> lights;

    std::string levelPath = "Levels/DumbLevel.txt";
    bool loadOk = LevelReaderWriter::loadAsText(levelPath, &world, player, boxes, lights);
    if (!loadOk) {
        // Try relative path from source
        levelPath = "GameEngine/NinjaPlatformer/Levels/DumbLevel.txt";
        loadOk = LevelReaderWriter::loadAsText(levelPath, &world, player, boxes, lights);
    }

    CHECK(loadOk);
    CHECK(boxes.size() > 0);
    CHECK_EQ(boxes.size(), 39);
}
