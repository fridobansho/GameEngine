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

TEST_CASE(LevelReaderWriter_RoundTripBinary) {
    if (!TestHarness::hasGLContext()) return;

    b2Vec2 gravity(0.0f, -9.8f);
    b2World world(gravity);

    Player player;
    player.init(&world, glm::vec2(4.5f, 8.25f), glm::vec2(2.0f, 2.0f), glm::vec2(1.0f, 1.8f),
                GameEngine::ColourRGBA8(120, 200, 255, 255));

    GameEngine::GLTexture brickTex = GameEngine::ResourceManager::GetTexture("Assets/bricks_top.png");
    std::vector<Box> boxes;

    Box b1;
    b1.init(&world, glm::vec2(0.0f, -2.0f), glm::vec2(20.0f, 2.0f), brickTex,
            GameEngine::ColourRGBA8(100, 150, 200, 255), false, false, 0.0f,
            glm::vec4(0.0f, 0.0f, 2.0f, 1.0f), false);
    boxes.push_back(b1);

    Box b2;
    b2.init(&world, glm::vec2(3.5f, 4.25f), glm::vec2(1.5f, 2.5f), brickTex,
            GameEngine::ColourRGBA8(255, 50, 60, 255), true, true, 0.45f,
            glm::vec4(0.25f, 0.25f, 0.5f, 0.5f), false);
    boxes.push_back(b2);

    Box goalBox;
    goalBox.init(&world, glm::vec2(12.0f, 3.0f), glm::vec2(1.2f, 2.4f), brickTex,
                 GameEngine::ColourRGBA8(255, 215, 0, 255), true, false, 0.0f,
                 glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), true);
    boxes.push_back(goalBox);

    std::vector<Light> lights;
    Light l1;
    l1.position = glm::vec2(2.0f, 5.0f);
    l1.size = 15.0f;
    l1.colour = GameEngine::ColourRGBA8(200, 220, 240, 180);
    lights.push_back(l1);

    Light l2;
    l2.position = glm::vec2(12.0f, 3.5f);
    l2.size = 9.5f;
    l2.colour = GameEngine::ColourRGBA8(255, 230, 100, 200);
    lights.push_back(l2);

    const std::string tmpPath = "/tmp/test_level_roundtrip.bin";
    bool saveOk = LevelReaderWriter::saveAsBinary(tmpPath, player, boxes, lights);
    CHECK(saveOk);

    b2World loadWorld(gravity);
    Player loadedPlayer;
    std::vector<Box> loadedBoxes;
    std::vector<Light> loadedLights;

    bool loadOk = LevelReaderWriter::loadAsBinary(tmpPath, &loadWorld, loadedPlayer, loadedBoxes, loadedLights);
    CHECK(loadOk);

    // Verify player
    CHECK_CLOSE(loadedPlayer.getPosition().x, 4.5f, 0.01f);
    CHECK_CLOSE(loadedPlayer.getPosition().y, 8.25f, 0.01f);
    CHECK_CLOSE(loadedPlayer.getDrawDims().x, 2.0f, 0.01f);
    CHECK_CLOSE(loadedPlayer.getDrawDims().y, 2.0f, 0.01f);
    CHECK_CLOSE(loadedPlayer.getCollisionDims().x, 1.0f, 0.01f);
    CHECK_CLOSE(loadedPlayer.getCollisionDims().y, 1.8f, 0.01f);
    CHECK_EQ(loadedPlayer.getColour().r, 120);
    CHECK_EQ(loadedPlayer.getColour().g, 200);
    CHECK_EQ(loadedPlayer.getColour().b, 255);
    CHECK_EQ(loadedPlayer.getColour().a, 255);

    // Verify boxes
    CHECK_EQ(loadedBoxes.size(), 3);
    if (loadedBoxes.size() == 3) {
        CHECK_CLOSE(loadedBoxes[0].getPosition().x, 0.0f, 0.01f);
        CHECK_CLOSE(loadedBoxes[0].getPosition().y, -2.0f, 0.01f);
        CHECK_CLOSE(loadedBoxes[0].getDimensions().x, 20.0f, 0.01f);
        CHECK_CLOSE(loadedBoxes[0].getDimensions().y, 2.0f, 0.01f);
        CHECK(!loadedBoxes[0].getIsDynamic());
        CHECK(!loadedBoxes[0].getFixedRotation());
        CHECK(!loadedBoxes[0].isGoal());
        CHECK(!loadedBoxes[0].getFixture()->IsSensor());
        CHECK_CLOSE(loadedBoxes[0].getUvRect().z, 2.0f, 0.01f);

        CHECK_CLOSE(loadedBoxes[1].getPosition().x, 3.5f, 0.01f);
        CHECK_CLOSE(loadedBoxes[1].getPosition().y, 4.25f, 0.01f);
        CHECK_CLOSE(loadedBoxes[1].getAngle(), 0.45f, 0.01f);
        CHECK(loadedBoxes[1].getIsDynamic());
        CHECK(loadedBoxes[1].getFixedRotation());
        CHECK(!loadedBoxes[1].isGoal());
        CHECK_CLOSE(loadedBoxes[1].getUvRect().x, 0.25f, 0.01f);
        CHECK_CLOSE(loadedBoxes[1].getUvRect().w, 0.5f, 0.01f);

        CHECK_CLOSE(loadedBoxes[2].getPosition().x, 12.0f, 0.01f);
        CHECK_CLOSE(loadedBoxes[2].getPosition().y, 3.0f, 0.01f);
        CHECK(!loadedBoxes[2].getIsDynamic());
        CHECK(loadedBoxes[2].isGoal());
        CHECK(loadedBoxes[2].getFixture()->IsSensor());
    }

    // Verify lights
    CHECK_EQ(loadedLights.size(), 2);
    if (loadedLights.size() == 2) {
        CHECK_CLOSE(loadedLights[0].position.x, 2.0f, 0.01f);
        CHECK_CLOSE(loadedLights[0].size, 15.0f, 0.01f);
        CHECK_EQ(loadedLights[0].colour.r, 200);
        CHECK_CLOSE(loadedLights[1].position.x, 12.0f, 0.01f);
        CHECK_CLOSE(loadedLights[1].size, 9.5f, 0.01f);
        CHECK_EQ(loadedLights[1].colour.g, 230);
    }

    std::remove(tmpPath.c_str());
}

TEST_CASE(LevelReaderWriter_GoalObjectRoundTrip) {
    if (!TestHarness::hasGLContext()) return;

    b2Vec2 gravity(0.0f, -9.8f);
    b2World world(gravity);

    Player player;
    player.init(&world, glm::vec2(0.0f, 2.0f), glm::vec2(2.0f, 2.0f), glm::vec2(1.0f, 1.8f),
                GameEngine::ColourRGBA8(255, 255, 255, 255));

    GameEngine::GLTexture brickTex = GameEngine::ResourceManager::GetTexture("Assets/bricks_top.png");
    std::vector<Box> boxes;

    Box floorBox;
    floorBox.init(&world, glm::vec2(0.0f, 0.0f), glm::vec2(12.0f, 1.0f), brickTex,
                  GameEngine::ColourRGBA8(255, 255, 255, 255), false, false, 0.0f,
                  glm::vec4(0, 0, 1, 1), false);
    CHECK(!floorBox.isGoal());
    CHECK(!floorBox.getFixture()->IsSensor());
    boxes.push_back(floorBox);

    Box goalBox;
    goalBox.init(&world, glm::vec2(5.0f, 1.5f), glm::vec2(1.5f, 2.0f), brickTex,
                 GameEngine::ColourRGBA8(255, 215, 0, 255), true, false, 0.0f,
                 glm::vec4(0, 0, 1, 1), true);
    CHECK(goalBox.isGoal());
    CHECK(goalBox.getFixture()->IsSensor());
    boxes.push_back(goalBox);

    std::vector<Light> lights;
    Light l;
    l.position = glm::vec2(5.0f, 2.0f);
    l.size = 10.0f;
    l.colour = GameEngine::ColourRGBA8(255, 255, 200, 255);
    lights.push_back(l);

    const std::string tmpPath = "/tmp/test_level_goal_roundtrip.txt";
    bool saveOk = LevelReaderWriter::saveAsText(tmpPath, player, boxes, lights);
    CHECK(saveOk);

    b2World loadWorld(gravity);
    Player loadedPlayer;
    std::vector<Box> loadedBoxes;
    std::vector<Light> loadedLights;
    bool loadOk = LevelReaderWriter::loadAsText(tmpPath, &loadWorld, loadedPlayer, loadedBoxes, loadedLights);
    CHECK(loadOk);
    CHECK_EQ(loadedBoxes.size(), 2);
    if (loadedBoxes.size() == 2) {
        CHECK(!loadedBoxes[0].isGoal());
        CHECK(!loadedBoxes[0].getFixture()->IsSensor());
        CHECK(loadedBoxes[1].isGoal());
        CHECK(loadedBoxes[1].getFixture()->IsSensor());
    }
    std::remove(tmpPath.c_str());

    // Verify existing DumbLevel.txt still loads with 39 boxes and 0 goals
    b2World shippedWorld(gravity);
    Player shippedPlayer;
    std::vector<Box> shippedBoxes;
    std::vector<Light> shippedLights;
    std::string levelPath = "Levels/DumbLevel.txt";
    bool shippedOk = LevelReaderWriter::loadAsText(levelPath, &shippedWorld, shippedPlayer, shippedBoxes, shippedLights);
    if (!shippedOk) {
        levelPath = "GameEngine/NinjaPlatformer/Levels/DumbLevel.txt";
        shippedOk = LevelReaderWriter::loadAsText(levelPath, &shippedWorld, shippedPlayer, shippedBoxes, shippedLights);
    }
    CHECK(shippedOk);
    CHECK_EQ(shippedBoxes.size(), 39);
    int goalCount = 0;
    for (const auto& b : shippedBoxes) {
        if (b.isGoal()) {
            goalCount++;
        }
    }
    CHECK_EQ(goalCount, 0);
}
