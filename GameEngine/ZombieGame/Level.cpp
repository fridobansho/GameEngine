#include "Level.h"
#include <fstream>
#include <iostream>

#include <GameEngine/Errors.h>
#include <GameEngine/ResourceManager.h>
#include <GameEngine/Vertex.h>

Level::Level(const string & fileName)
{
	ifstream file;
	file.open(fileName);

	if (file.fail())
	{
		GameEngine::FatalError("Failed to open " + fileName);
	}

	string tmp;
	file >> tmp >> _numHumans;

	getline(file, tmp);

	while (getline(file, tmp))
	{
		_levelData.push_back(tmp);
	}

	_spriteBatch.init();

	_spriteBatch.begin();

	glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	GameEngine::Colour whiteColour;
	whiteColour.r = 255;
	whiteColour.g = 255;
	whiteColour.b = 255;
	whiteColour.a = 255;

	for (int y = 0; y < _levelData.size(); y++)
	{
		for (int x = 0; x < _levelData[y].size(); x++)
		{
			char tile = _levelData[y][x];
			glm::vec4 destRect(x * TILE_WIDTH, y * TILE_WIDTH, TILE_WIDTH, TILE_WIDTH);
			switch (tile)
			{
			case '@':
				_levelData[y][x] = '.';
				_startPlayerPosition.x = x * TILE_WIDTH;
				_startPlayerPosition.y = y * TILE_WIDTH;
				break;
			case 'R':
			case 'B':
				_spriteBatch.draw(destRect,
					uvRect,
					GameEngine::ResourceManager::GetTexture("Textures/red_bricks.png").id,
					0.0f,
					whiteColour);
				break;
			case 'L':
				_spriteBatch.draw(destRect,
					uvRect,
					GameEngine::ResourceManager::GetTexture("Textures/light_bricks.png").id,
					0.0f,
					whiteColour);
				break;
			case 'G':
				_spriteBatch.draw(destRect,
					uvRect,
					GameEngine::ResourceManager::GetTexture("Textures/glass.png").id,
					0.0f,
					whiteColour);
				break;
			case 'Z':
				_levelData[y][x] = '.';
				_zombieStartPositions.emplace_back(x * TILE_WIDTH, y * TILE_WIDTH);
				break;
			case '.':
				break;
			default:
				printf("Unexpected symbol %c at (%d, %d)", tile, x, y);
				break;
			}
		}
	}

	_spriteBatch.end();
}

Level::~Level()
{
}

void Level::draw()
{
	_spriteBatch.renderBatch();
}
