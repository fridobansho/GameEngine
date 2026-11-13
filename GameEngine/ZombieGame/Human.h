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
		std::vector<std::unique_ptr<Human>>& humans,
		std::vector<std::unique_ptr<Zombie>>& zombies,
		float deltaTime) override;
private:
	int m_frames = 0;
};

