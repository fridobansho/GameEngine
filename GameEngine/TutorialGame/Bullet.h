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
	float _speed;
	glm::vec2 _direction;
	glm::vec2 _position;
};

