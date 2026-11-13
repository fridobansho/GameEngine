#include "Zombie.h"
#include "Human.h"

#include <GameEngine/ResourceManager.h>

Zombie::Zombie()
{
}


Zombie::~Zombie()
{
}

void Zombie::init(float speed, glm::vec2 position)
{
	m_speed = speed;
	m_position = position;

	m_health = 150.0f;

	m_colour = GameEngine::ColourRGBA8(255, 255, 255, 255);
	m_textureID = GameEngine::ResourceManager::GetTexture("Textures/zombie.png").id;
}

void Zombie::update(const std::vector<std::string>& levelData,
	std::vector<std::unique_ptr<Human>>& humans,
	std::vector<std::unique_ptr<Zombie>>& zombies,
	float deltaTime)
{
	Human* closestHuman = getNearestHuman(humans);

	if (closestHuman != nullptr)
	{
		glm::vec2 diff = closestHuman->getPosition() - m_position;
		if (glm::length(diff) > 0.0001f)
		{
			m_direction = glm::normalize(diff);
		}
		m_position += m_direction * m_speed * deltaTime;
	}

	collideWithLevel(levelData);
}

Human * Zombie::getNearestHuman(std::vector<std::unique_ptr<Human>>& humans)
{
	Human* closestHuman = nullptr;
	float smallestDistance = 9999999.0f;

	for (size_t i = 0; i < humans.size(); i++)
	{
		glm::vec2 distVec = humans[i]->getPosition() - m_position;
		float distance = glm::length(distVec);

		if (distance < smallestDistance)
		{
			smallestDistance = distance;
			closestHuman = humans[i].get();
		}
	}

	return closestHuman;
}
