#pragma once

#include "Ball.h"
#include <vector>

struct Cell
{
	std::vector<Ball*> balls;
};

class Grid
{
	friend class BallController;
public:
	Grid(int width, int height, int cellSize);
	~Grid();

	void addBall(Ball* ball);
	void addBall(Ball* ball, Cell* cell);
	Cell* getCell(int x, int y);
	Cell* getCell(const glm::vec2& pos);

	void removeBallFromCell(Ball* ball);
private:
	std::vector<Cell*> m_cells;
	int m_cellSize;
	int m_width;
	int m_height;
	int m_numXCells;
	int m_numYCells;
};

