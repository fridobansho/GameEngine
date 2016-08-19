#include "Bullet.h"

#include <GameEngine/ResourceManager.h>

Bullet::Bullet(glm::vec2 position, glm::vec2 direction, float speed, int lifeTime)
{
	m_position = position;
	m_direction = direction;
	m_speed = speed;
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

	glm::vec4 posAndSize(m_position.x, m_position.y, 30, 30);

	spriteBatch.draw(posAndSize, uv, texture.id, 0.0f, colour);
}

bool Bullet::update()
{
	m_position += m_direction * m_speed;
	_lifeTime--;

	if (_lifeTime == 0)
	{
		return true;
	}

	return false;
}
