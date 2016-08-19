#pragma once

#include <vector>
#include <string>

#include <GameEngine/SpriteBatch.h>

using namespace std;

const int TILE_WIDTH = 64;

class Level
{
public:
	Level(const string& fileName);
	~Level();

	void draw();

	int getWidth() const { return m_levelData[0].size(); }
	int getHeight() const { return m_levelData.size(); }
	int getNumHumans() const { return m_numHumans; }
	const std::vector<std::string>& getLevelData() const { return m_levelData; }

	glm::vec2 getStartPlayerPos() const { return m_startPlayerPosition; }

	const std::vector<glm::vec2>& getZombiePositions() const { return m_zombieStartPositions; }
private:
	vector<string> m_levelData;
	int m_numHumans = 0;

	GameEngine::SpriteBatch m_spriteBatch;

	glm::vec2 m_startPlayerPosition = glm::vec2(0.0f);
	std::vector<glm::vec2> m_zombieStartPositions;
};

