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
