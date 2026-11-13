#pragma once
#include "Human.h"

#include <memory>
#include <GameEngine/InputManager.h>
#include <GameEngine/Camera2D.h>

class Gun;
class Bullet;

class Player :
	public Human
{
public:
	Player();
	~Player();

	void init(float speed, glm::vec2 position, GameEngine::InputManager* inputManager, GameEngine::Camera2D* camera, std::vector<Bullet>* bullets);

	void AddGun(std::unique_ptr<Gun> gun);

	void update(const std::vector<std::string>& levelData,
		std::vector<std::unique_ptr<Human>>& humans,
		std::vector<std::unique_ptr<Zombie>>& zombies,
		float deltaTime) override;
private:
	GameEngine::InputManager* m_inputManager;

	std::vector<std::unique_ptr<Gun>> m_guns;
	int m_currentGunIndex = -1;

	GameEngine::Camera2D* m_camera;
	std::vector<Bullet>* m_bullets;
};

