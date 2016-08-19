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

	void update(bool isMouseDown, const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets, float deltaTime);

private:
	void fire(const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets);

	std::string m_name;
	int m_fireRate = 0;
	int m_bulletsPerShot = 0;
	float m_spread = 0.0f;
	float m_bulletSpeed = 0.0f;
	float m_bulletDamage = 0.0f;

	float m_frameCounter = 0.0f;
};

