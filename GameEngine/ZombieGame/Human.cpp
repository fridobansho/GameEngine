#include "Human.h"
#include <ctime>
#include <random>
#include <glm/gtx/rotate_vector.hpp>
#include <GameEngine/ResourceManager.h>

Human::Human() : m_frames(0)
{
}


Human::~Human()
{
}

void Human::init(float speed, glm::vec2 position)
{
	static std::mt19937 randomEngine(time(nullptr));
	static std::uniform_real_distribution<float> randomDir(-1.0f, 1.0f);

	m_speed = speed;
	m_position = position;

	m_health = 20.0f;

	m_colour.r = 255;
	m_colour.g = 255;
	m_colour.b = 255;
	m_colour.a = 255;
	m_textureID = GameEngine::ResourceManager::GetTexture("Textures/human.png").id;
	m_direction = glm::vec2(randomDir(randomEngine), randomDir(randomEngine));
	if (m_direction.length() == 0) m_direction = glm::vec2(1.0f, 0.0f);

	m_direction = glm::normalize(m_direction);
}

void Human::update(const std::vector<std::string>& levelData,
	std::vector<Human*>& humans,
	std::vector<Zombie*>& zombies,
	float deltaTime)
{
	static std::mt19937 randomEngine(time(nullptr));
	static std::uniform_real_distribution<float> randomRotate(-40.0f, 40.0f);
	m_position += m_direction * m_speed * deltaTime;

	if (m_frames == 20)
	{
		m_direction = glm::rotate(m_direction, randomRotate(randomEngine));
		m_frames = 0;
	}
	else
		m_frames++;

	if (collideWithLevel(levelData))
	{
		m_direction = glm::rotate(m_direction, randomRotate(randomEngine));
	}
}
