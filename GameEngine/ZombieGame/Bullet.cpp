#include "Bullet.h"
#include <GameEngine/ResourceManager.h>
#include "Human.h"
#include "Zombie.h"
#include "Level.h"

Bullet::Bullet(glm::vec2 position, glm::vec2 direction, float damage, float speed) :
	_position(position),
	_direction(direction),
	_damage(damage),
	_speed(speed)
{
}

Bullet::~Bullet()
{
}

bool Bullet::update(const std::vector<std::string>& levelData)
{
	_position += _direction * _speed;
	return collideWithWorld(levelData);
}


void Bullet::draw(GameEngine::SpriteBatch& spriteBatch)
{
	glm::vec4 destRect(_position.x + BULLET_RADIUS,
		_position.y + BULLET_RADIUS,
		BULLET_RADIUS * 2,
		BULLET_RADIUS * 2);

	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	GameEngine::Colour colour;
	colour.r = 75;
	colour.g = 75;
	colour.b = 75;
	colour.a = 255;

	spriteBatch.draw(destRect, uvRect, GameEngine::ResourceManager::GetTexture("Textures/circle.png").id, 0.0f, colour);
}

bool Bullet::collideWithAgent(Agent* agent)
{
	const float MIN_DISTANCE = AGENT_RADIUS + BULLET_RADIUS;

	glm::vec2 centerPosA = _position;
	glm::vec2 centerPosB = agent->getPosition() + glm::vec2(AGENT_RADIUS);

	glm::vec2 distVec = centerPosA - centerPosB;

	float distance = glm::length(distVec);

	float collisionDepth = MIN_DISTANCE - distance;

	if (collisionDepth > 0)
	{
		return true;
	}
	return false;
}

bool Bullet::collideWithWorld(const std::vector<std::string>& levelData)
{
	glm::ivec2 gridPosition;
	gridPosition.x = floor(_position.x / TILE_WIDTH);
	gridPosition.y = floor(_position.y / TILE_WIDTH);

	if ((gridPosition.x < 0 || gridPosition.x >= levelData[0].size()) || (gridPosition.y < 0 || gridPosition.y >= levelData.size()))
		return true;

	return levelData[gridPosition.y][gridPosition.x] != '.';
}
