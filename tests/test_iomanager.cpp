#include "TestHarness.h"
#include <GameEngine/IOManager.h>
#include <vector>

TEST_CASE(IOManager_ReadFileToBuffer) {
    std::vector<unsigned char> buffer;
    bool success = GameEngine::IOManager::ReadFileToBuffer("Fonts/chintzy.ttf", buffer);
    if (!success) {
        success = GameEngine::IOManager::ReadFileToBuffer("GameEngine/NinjaPlatformer/Fonts/chintzy.ttf", buffer);
    }
    CHECK(success);
    CHECK(buffer.size() > 0);

    // Non-existent file must fail gracefully without crash
    std::vector<unsigned char> failBuffer;
    bool failSuccess = GameEngine::IOManager::ReadFileToBuffer("__non_existent_file_xyz_123__.txt", failBuffer);
    CHECK(!failSuccess);
    CHECK_EQ(failBuffer.size(), 0);
}

TEST_CASE(IOManager_ReadFileToBufferString) {
    std::string path = "Shaders/textureShading.vert";
    std::string strBuffer;
    bool success = GameEngine::IOManager::ReadFileToBuffer(path, strBuffer);
    if (!success) {
        path = "GameEngine/NinjaPlatformer/Shaders/textureShading.vert";
        success = GameEngine::IOManager::ReadFileToBuffer(path, strBuffer);
    }
    CHECK(success);
    CHECK(strBuffer.size() > 0);
    CHECK(strBuffer.find("#version") != std::string::npos);

    std::vector<unsigned char> vecBuffer;
    bool vecSuccess = GameEngine::IOManager::ReadFileToBuffer(path, vecBuffer);
    CHECK(vecSuccess);
    CHECK_EQ(strBuffer.size(), vecBuffer.size());
    if (strBuffer.size() == vecBuffer.size()) {
        for (size_t i = 0; i < strBuffer.size(); ++i) {
            if (static_cast<unsigned char>(strBuffer[i]) != vecBuffer[i]) {
                CHECK(false);
                break;
            }
        }
    }

    // Non-existent file must fail gracefully without crash
    std::string failBuffer = "initial";
    bool failSuccess = GameEngine::IOManager::ReadFileToBuffer("__non_existent_file_xyz_123__.txt", failBuffer);
    CHECK(!failSuccess);
}
