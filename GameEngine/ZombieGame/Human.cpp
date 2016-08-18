#include "Human.h"
#include <ctime>
#include <random>
#include <glm/gtx/rotate_vector.hpp>

Human::Human() : _frames(0)
{
}


Human::~Human()
{
}

void Human::init(float speed, glm::vec2 position)
{
	static std::mt19937 randomEngine(time(nullptr));
	static std::uniform_real_distribution<float> randomDir(-1.0f, 1.0f);

	_speed = speed;
	_position = position;

	_health = 20.0f;

	_colour.r = 200;
	_colour.g = 0;
	_colour.b = 200;
	_colour.a = 255;
	_direction = glm::vec2(randomDir(randomEngine), randomDir(randomEngine));
	if (_direction.length() == 0) _direction = glm::vec2(1.0f, 0.0f);

	_direction = glm::normalize(_direction);
}

void Human::update(const std::vector<std::string>& levelData,
	std::vector<Human*>& humans,
	std::vector<Zombie*>& zombies,
	float deltaTime)
{
	static std::mt19937 randomEngine(time(nullptr));
	static std::uniform_real_distribution<float> randomRotate(-40.0f, 40.0f);
	_position += _direction * _speed * deltaTime;

	if (_frames == 20)
	{
		_direction = glm::rotate(_direction, randomRotate(randomEngine));
		_frames = 0;
	}
	else
		_frames++;

	if (collideWithLevel(levelData))
	{
		_direction = glm::rotate(_direction, randomRotate(randomEngine));
	}
}
