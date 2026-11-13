#include "Level.h"
#include <iostream>
#include <sstream>

#include <GameEngine/GameEngineErrors.h>
#include <GameEngine/IOManager.h>
#include <GameEngine/ResourceManager.h>
#include <GameEngine/Vertex.h>

Level::Level(const string & fileName)
{
	std::string content;
	if (!GameEngine::IOManager::ReadFileToBuffer(fileName, content))
	{
		GameEngine::FatalError("Failed to open " + fileName);
	}

	std::istringstream file(content);

	string tmp;
	file >> tmp >> m_numHumans;

	getline(file, tmp);

	while (getline(file, tmp))
	{
		if (!tmp.empty() && tmp.back() == '\r')
		{
			tmp.pop_back();
		}
		if (!tmp.empty())
		{
			m_levelData.push_back(tmp);
		}
	}

	m_spriteBatch.init();

	m_spriteBatch.begin();

	glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	GameEngine::ColourRGBA8 whiteColour(255, 255, 255, 255);

	for (size_t y = 0; y < m_levelData.size(); y++)
	{
		for (size_t x = 0; x < m_levelData[y].size(); x++)
		{
			char tile = m_levelData[y][x];
			glm::vec4 destRect(x * TILE_WIDTH, y * TILE_WIDTH, TILE_WIDTH, TILE_WIDTH);
			switch (tile)
			{
			case '@':
				m_levelData[y][x] = '.';
				m_startPlayerPosition.x = (float)x * TILE_WIDTH;
				m_startPlayerPosition.y = (float)y * TILE_WIDTH;
				break;
			case 'R':
			case 'B':
				m_spriteBatch.draw(destRect,
					uvRect,
					GameEngine::ResourceManager::GetTexture("Textures/red_bricks.png").id,
					0.0f,
					whiteColour);
				break;
			case 'L':
				m_spriteBatch.draw(destRect,
					uvRect,
					GameEngine::ResourceManager::GetTexture("Textures/light_bricks.png").id,
					0.0f,
					whiteColour);
				break;
			case 'G':
				m_spriteBatch.draw(destRect,
					uvRect,
					GameEngine::ResourceManager::GetTexture("Textures/glass.png").id,
					0.0f,
					whiteColour);
				break;
			case 'Z':
				m_levelData[y][x] = '.';
				m_zombieStartPositions.emplace_back(x * TILE_WIDTH, y * TILE_WIDTH);
				break;
			case '.':
				break;
			default:
				printf("Unexpected symbol %c at (%d, %d)\n", tile, (int)x, (int)y);
				break;
			}
		}
	}

	m_spriteBatch.end();
}

Level::~Level()
{
}

void Level::draw()
{
	m_spriteBatch.renderBatch();
}
