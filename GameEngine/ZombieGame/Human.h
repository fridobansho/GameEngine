#pragma once
#include "Agent.h"
class Human :
	public Agent
{
public:
	Human();
	virtual ~Human();

	void init(float speed, glm::vec2 position);

	virtual void update(const std::vector<std::string>& levelData,
		std::vector<Human*>& humans,
		std::vector<Zombie*>& zombies,
		float deltaTime) override;
private:
	glm::vec2 m_direction = glm::vec2(0.0f);
	int m_frames = 0;
};

