#pragma once
#include "Agent.h"
class Zombie :
	public Agent
{
public:
	Zombie();
	~Zombie();

	void init(float speed, glm::vec2 position);

	void update(const std::vector<std::string>& levelData,
		std::vector<std::unique_ptr<Human>>& humans,
		std::vector<std::unique_ptr<Zombie>>& zombies,
		float deltaTime) override;

private:
	Human* getNearestHuman(std::vector<std::unique_ptr<Human>>& humans);
};

