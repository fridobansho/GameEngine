#include "Bullet.h"

#include <GameEngine/ResourceManager.h>

Bullet::Bullet(glm::vec2 position, glm::vec2 direction, float speed, int lifeTime)
{
	_position = position;
	_direction = direction;
	_speed = speed;
	_lifeTime = lifeTime;
}

Bullet::~Bullet()
{
}

void Bullet::draw(SpriteBatch & spriteBatch)
{
	ColourRGBA8 colour(255, 255, 255, 255);

	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	static GLTexture texture = ResourceManager::GetTexture("Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");

	glm::vec4 posAndSize(_position.x, _position.y, 30, 30);

	spriteBatch.draw(posAndSize, uv, texture.id, 0.0f, colour);
}

bool Bullet::update()
{
	_position += _direction * _speed;
	_lifeTime--;

	if (_lifeTime == 0)
	{
		return true;
	}

	return false;
}
