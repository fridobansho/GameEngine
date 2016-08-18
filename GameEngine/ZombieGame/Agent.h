#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GameEngine/SpriteBatch.h>

const float AGENT_WIDTH = 60.0f;
const float AGENT_RADIUS = (float)AGENT_WIDTH / 2.0f;

class Zombie;
class Human;

class Agent
{
public:
	Agent();
	virtual ~Agent();

	virtual void update(const std::vector<std::string>& levelData,
		std::vector<Human*>& humans,
		std::vector<Zombie*>& zombies) = 0;

	bool collideWithLevel(const std::vector<std::string>& levelData);

	bool collideWithAgent(Agent* agent);

	void collideWithTile(glm::vec2 tilePosition);

	void draw(GameEngine::SpriteBatch& spriteBatch);

	glm::vec2 getPosition() const { return _position; }

	bool applyDamage(float damage);

protected:
	void checkTilePosition(const std::vector<std::string>& levelData, std::vector<glm::vec2>& collideTilePositions, float x, float y);

	glm::vec2 _position;
	float _speed;
	GameEngine::ColourRGBA8 _colour;

	float _health;
};

