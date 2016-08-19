#pragma once

#include <glm/glm.hpp>
#include <GameEngine/SpriteBatch.h>

using namespace GameEngine;

class Bullet
{
public:
	Bullet(glm::vec2 position, glm::vec2 direction, float speed, int lifeTime);
	~Bullet();

	void draw(SpriteBatch& spriteBatch);
	bool update();

private:
	int _lifeTime;
	float m_speed;
	glm::vec2 m_direction;
	glm::vec2 m_position;
};

