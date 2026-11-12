#include "TestHarness.h"
#include <BallGame/Grid.h>

TEST_CASE(Grid_BoundaryClamping) {
    // 1000x1000 grid with 100 cell size = 10x10 cells
    Grid grid(1000, 1000, 100);

    // Normal query inside bounds
    Cell* c00 = grid.getCell(0, 0);
    CHECK(c00 != nullptr);

    Cell* c55 = grid.getCell(5, 5);
    CHECK(c55 != nullptr);

    // Negative coordinates must clamp to 0 without crashing
    Cell* cNeg = grid.getCell(-10, -20);
    CHECK_EQ(cNeg, c00);

    // Overflow coordinates must clamp to (m_numXCells - 1, m_numYCells - 1)
    Cell* cMax = grid.getCell(9, 9);
    Cell* cOver = grid.getCell(100, 200);
    CHECK_EQ(cOver, cMax);
}

TEST_CASE(Grid_AddAndRemoveBall) {
    Grid grid(500, 500, 50);

    Ball ball1(10.0f, 1.0f, glm::vec2(25.0f, 25.0f), glm::vec2(0.0f, 0.0f), 0,
               GameEngine::ColourRGBA8(255, 255, 255, 255));

    grid.addBall(&ball1);
    CHECK(ball1.ownerCell != nullptr);
    CHECK_EQ(ball1.cellVectorIndex, 0);
    CHECK_EQ(ball1.ownerCell->balls.size(), 1);
    CHECK_EQ(ball1.ownerCell->balls[0], &ball1);

    // Remove ball
    grid.removeBallFromCell(&ball1);
    CHECK(ball1.ownerCell == nullptr);
    CHECK_EQ(ball1.cellVectorIndex, -1);

    // Query cell directly to verify it is empty
    Cell* cell = grid.getCell(glm::vec2(25.0f, 25.0f));
    CHECK(cell != nullptr);
    CHECK_EQ(cell->balls.size(), 0);
}
