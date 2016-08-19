#include "Bullet.h"
#include <GameEngine/ResourceManager.h>
#include "Human.h"
#include "Zombie.h"
#include "Level.h"

Bullet::Bullet(glm::vec2 position, glm::vec2 direction, float damage, float speed) :
	m_position(position),
	m_direction(direction),
	m_damage(damage),
	m_speed(speed)
{
}

Bullet::~Bullet()
{
}

bool Bullet::update(const std::vector<std::string>& levelData, float deltaTime)
{
	m_position += m_direction * m_speed  * deltaTime;
	return collideWithWorld(levelData);
}


void Bullet::draw(GameEngine::SpriteBatch& spriteBatch)
{
	glm::vec4 destRect(m_position.x + BULLET_RADIUS,
		m_position.y + BULLET_RADIUS,
		BULLET_RADIUS * 2,
		BULLET_RADIUS * 2);

	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	GameEngine::ColourRGBA8 colour(75, 75, 75, 255);

	spriteBatch.draw(destRect, uvRect, GameEngine::ResourceManager::GetTexture("Textures/circle.png").id, 0.0f, colour);
}

bool Bullet::collideWithAgent(Agent* agent)
{
	const float MIN_DISTANCE = AGENT_RADIUS + BULLET_RADIUS;

	glm::vec2 centerPosA = m_position;
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
	gridPosition.x = (int)floor(m_position.x / TILE_WIDTH);
	gridPosition.y = (int)floor(m_position.y / TILE_WIDTH);

	if ((gridPosition.x < 0.0f || gridPosition.x >= (float)levelData[0].size()) || (gridPosition.y < 0.0f || gridPosition.y >= (float)levelData.size()))
		return true;

	return levelData[gridPosition.y][gridPosition.x] != '.';
}
