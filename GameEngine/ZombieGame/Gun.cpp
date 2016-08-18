#include "Gun.h"
#include <random>
#include <ctime>
#include <glm/gtx/rotate_vector.hpp>

Gun::Gun(std::string name, int fireRate, int bulletsPerShot, float spread, float bulletDamage, float bulletSpeed) :
	_name(name),
	_fireRate(fireRate),
	_bulletsPerShot(bulletsPerShot),
	_spread(spread),
	_bulletDamage(bulletDamage),
	_bulletSpeed(bulletSpeed),
	_frameCounter(0)
{
}

Gun::~Gun()
{
}

void Gun::update(bool isMouseDown, const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets, float deltaTime)
{
	_frameCounter += 1.0f * deltaTime;
	if (_frameCounter >= _fireRate && isMouseDown)
	{
		fire(position, direction, bullets);
		_frameCounter = 0;
	}
}

void Gun::fire(const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets)
{
	std::mt19937 randomEngine(time(nullptr));
	std::uniform_real_distribution<float> randomRotate(-_spread, _spread);

	for (int i = 0; i < _bulletsPerShot; i++)
	{
		bullets.emplace_back(position, glm::rotate(direction, randomRotate(randomEngine)), _bulletDamage, _bulletSpeed);
	}
}
