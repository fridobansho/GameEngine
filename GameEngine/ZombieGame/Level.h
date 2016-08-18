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

	int getWidth() const { return _levelData[0].size(); }
	int getHeight() const { return _levelData.size(); }
	int getNumHumans() const { return _numHumans; }
	const std::vector<std::string>& getLevelData() const { return _levelData; }

	glm::vec2 getStartPlayerPos() const { return _startPlayerPosition; }

	const std::vector<glm::vec2>& getZombiePositions() const { return _zombieStartPositions; }
private:
	vector<string> _levelData;
	int _numHumans;

	GameEngine::SpriteBatch _spriteBatch;

	glm::vec2 _startPlayerPosition;
	std::vector<glm::vec2> _zombieStartPositions;
};

