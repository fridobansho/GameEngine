#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <GameEngine/SpriteBatch.h>

class Agent;
class Human;
class Zombie;

const int BULLET_RADIUS = 5;

class Bullet
{
public:
	Bullet(glm::vec2 position, glm::vec2 direction, float damage, float speed);
	~Bullet();

	bool update(const std::vector<std::string>& levelData, float deltaTime);

	void draw(GameEngine::SpriteBatch& spriteBatch);

	bool collideWithAgent(Agent* agent);

	float getDamage() const { return m_damage; }

private:
	bool collideWithWorld(const std::vector<std::string>& levelData);

	glm::vec2 m_position = glm::vec2(0.0f);
	glm::vec2 m_direction = glm::vec2(0.0f);
	float m_damage = 0.0f;
	float m_speed = 0.0f;
};

