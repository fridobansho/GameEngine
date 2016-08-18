#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Bullet.h"

class Gun
{
public:
	Gun(std::string name, int fireRate, int bulletsPerShot, float spread, float bulletDamage, float BulletSpeed);
	~Gun();

	void update(bool isMouseDown, const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets);

private:
	void fire(const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets);
	std::string _name;
	int _fireRate;
	int _bulletsPerShot;
	float _spread;
	float _bulletSpeed;
	float _bulletDamage;

	int _frameCounter;
};

