#include "MainGame.h"

#include <GameEngine/GameEngine.h>
#include <GameEngine/Timing.h>
#include <GameEngine/Errors.h>

#include <SDL.h>
#include <iostream>
#include <random>
#include <ctime>
#include <algorithm>

#include "Gun.h"

const float HUMAN_SPEED = 1.0f;
const float ZOMBIE_SPEED = 1.3f;
const float PLAYER_SPEED = 5.0f;

MainGame::MainGame() :
	_screenWidth(1024),
	_screenHeight(768),
	_gameState(GameState::PLAY),
	_fps(0),
	_player(nullptr),
	_numHumansKilled(0),
	_numZombiesKilled(0)
{
    // Empty
}

MainGame::~MainGame()
{
	for (int i = 0; i < _levels.size(); i++)
	{
		delete _levels[i];
	}
	for (int i = 0; i < _humans.size(); i++)
	{
		delete _humans[i];
	}
	for (int i = 0; i < _zombies.size(); i++)
	{
		delete _zombies[i];
	}
}

void MainGame::Run()
{
	initSystems();

	initLevel();

	gameLoop();
}

void MainGame::initSystems() {
	GameEngine::Init();

	_window.Create("Zombie Game", _screenWidth, _screenHeight, 0);

	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

	initShaders();

	_agentSpriteBatch.init();

	_camera.Init(_screenWidth, _screenHeight);
}

void MainGame::initLevel()
{
	_levels.push_back(new Level("Levels/level1.txt"));
	_currentLevel = 0;

	_player = new Player();
	_player->init(PLAYER_SPEED, _levels[_currentLevel]->getStartPlayerPos(), &_inputManager, &_camera, &_bullets);

	_humans.push_back(_player);

	std::mt19937 randomEngine;
	randomEngine.seed(time(nullptr));
	std::uniform_int_distribution<int> randomX(1, _levels[_currentLevel]->getWidth() - 2);
	std::uniform_int_distribution<int> randomY(1, _levels[_currentLevel]->getHeight() - 2);

	for (int i = 0; i < _levels[_currentLevel]->getNumHumans(); i++)
	{
		_humans.push_back(new Human);
		glm::vec2 pos(randomX(randomEngine) * TILE_WIDTH, randomY(randomEngine) * TILE_WIDTH);
		_humans.back()->init(HUMAN_SPEED, pos);
	}

	const std::vector<glm::vec2>& zombiePositions = _levels[_currentLevel]->getZombiePositions();

	for (int i = 0; i < zombiePositions.size(); i++)
	{
		_zombies.push_back(new Zombie);
		_zombies.back()->init(ZOMBIE_SPEED, zombiePositions[i]);
	}

	const float BULLET_SPEED = 20.0f;
	_player->AddGun(new Gun("Magnum", 10, 1, 0.125f, 30, BULLET_SPEED));
	_player->AddGun(new Gun("Shotgun", 30, 12, 0.3f, 4, BULLET_SPEED));
	_player->AddGun(new Gun("MP5", 2, 1, 0.3f, 20, BULLET_SPEED));
}

void MainGame::initShaders()
{
	_colourProgram.CompileShaders("Shaders\\colourShading.vert", "Shaders\\colourShading.frag");
	_colourProgram.AddAttribute("vertexPosition");
	_colourProgram.AddAttribute("vertexColour");
	_colourProgram.AddAttribute("vertexUV");
	_colourProgram.LinkShaders();
}

void MainGame::gameLoop()
{
	const float DESIRED_FPS = 60.0f;
	const int MAX_PHYSICS_STEPS = 6;
	GameEngine::FPSLimiter fpsLimiter;
	fpsLimiter.setMaxFPS(DESIRED_FPS);

	const float CAMERA_SCALE = 1.0f / 4.0f;
	_camera.SetScale(CAMERA_SCALE);

	const float MS_PER_SECOND = 1000.0f;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;

	float previousTicks = SDL_GetTicks();

	while (_gameState == GameState::PLAY)
	{
		fpsLimiter.begin();

		float newTicks = SDL_GetTicks();
		float frameTime = newTicks - previousTicks;
		previousTicks = newTicks;

		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		checkVictory();

		_inputManager.update();

		processInput();

		int i = 0;
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS)
		{
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			updateAgents(deltaTime);

			updateBullets(deltaTime);

			totalDeltaTime -= deltaTime;
			i++;
		}

		_camera.SetPosition(_player->getPosition());

		_camera.Update();

		drawGame();

		_fps = fpsLimiter.end();
	}
}

void MainGame::updateAgents(float deltaTime)
{
	for (int i = 0; i < _humans.size(); i++)
	{
		_humans[i]->update(_levels[_currentLevel]->getLevelData(), _humans, _zombies, deltaTime);
	}

	for (int i = 0; i < _zombies.size(); i++)
	{
		_zombies[i]->update(_levels[_currentLevel]->getLevelData(), _humans, _zombies, deltaTime);
	}

	for (int i = 0; i < _zombies.size(); i++)
	{
		for (int j = i + 1; j < _zombies.size(); j++)
		{
			_zombies[i]->collideWithAgent(_zombies[j]);
		}
		for (int j =1; j < _humans.size(); j++)
		{
			if (_zombies[i]->collideWithAgent(_humans[j]))
			{
				_zombies.push_back(new Zombie);
				_zombies.back()->init(ZOMBIE_SPEED, _humans[j]->getPosition());

				delete _humans[j];
				_humans[j] = _humans.back();
				_humans.pop_back();
			}
		}
		if (_zombies[i]->collideWithAgent(_player))
		{
			GameEngine::FatalError("You lose");
		}
	}

	for (int i = 0; i < _humans.size(); i++)
	{
		for (int j = i + 1; j < _humans.size(); j++)
		{
			_humans[i]->collideWithAgent(_humans[j]);
		}
	}
}

void MainGame::updateBullets(float deltaTime)
{
	for (int i = 0; i < _bullets.size();)
	{
		if (_bullets[i].update(_levels[_currentLevel]->getLevelData(), deltaTime))
		{
			_bullets[i] = _bullets.back();
			_bullets.pop_back();
		}
		else
			i++;
	}

	bool wasBulletRemoved;

	for (int i = 0; i < _bullets.size();i++)
	{
		wasBulletRemoved = false;
		for (int j = 0; j < _zombies.size();)
		{
			if (_bullets[i].collideWithAgent(_zombies[j]))
			{
				if (_zombies[j]->applyDamage(_bullets[i].getDamage()))
				{
					delete _zombies[j];
					_zombies[j] = _zombies.back();
					_zombies.pop_back();
					_numZombiesKilled++;
				}
				else
					j++;

				_bullets[i] = _bullets.back();
				_bullets.pop_back();
				wasBulletRemoved = true;
				break;
			}
			else
				j++;
		}
		if (wasBulletRemoved == false)
		{
			for (int j = 1; j < _humans.size();)
			{
				if (_bullets[i].collideWithAgent(_humans[j]))
				{
					if (_humans[j]->applyDamage(_bullets[i].getDamage()))
					{
						delete _humans[j];
						_humans[j] = _humans.back();
						_humans.pop_back();
					}
					else
						j++;

					_bullets[i] = _bullets.back();
					_bullets.pop_back();
					wasBulletRemoved = true;
					_numHumansKilled++;
					break;
				}
				else
					j++;
			}
		}
	}
}

void MainGame::checkVictory()
{
	if (_zombies.empty())
	{
		std::printf("***You win! ***\n You killed %d humans and %d zombies. There are %d/%d civilians remaining",
			_numHumansKilled, _numZombiesKilled, _humans.size() - 1, _levels[_currentLevel]->getNumHumans());
		GameEngine::FatalError("");
	}
}

void MainGame::processInput() {
    SDL_Event evnt;
    //Will keep looping until there are no more events to process
    while (SDL_PollEvent(&evnt)) {
        switch (evnt.type) {
            case SDL_QUIT:
				_gameState = GameState::EXIT;
                // Exit the game here!
                break;
            case SDL_MOUSEMOTION:
                _inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
                break;
            case SDL_KEYDOWN:
                _inputManager.pressKey(evnt.key.keysym.sym);
                break;
            case SDL_KEYUP:
                _inputManager.releaseKey(evnt.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                _inputManager.pressKey(evnt.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                _inputManager.releaseKey(evnt.button.button);
                break;
        }
    }
}

void MainGame::drawGame() {
    // Set the base depth to 1.0
    glClearDepth(1.0);
    // Clear the color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_colourProgram.Use();

	glActiveTexture(GL_TEXTURE0);

	GLint textureUniform = _colourProgram.GetUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	glm::mat4 projectionMatrix = _camera.getCameraMatrix();
	GLint pUniform = _colourProgram.GetUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	_levels[_currentLevel]->draw();

	_agentSpriteBatch.begin();

	for (int i = 0; i < _humans.size(); i++)
	{
		_humans[i]->draw(_agentSpriteBatch);
	}
	for (int i = 0; i < _zombies.size(); i++)
	{
		_zombies[i]->draw(_agentSpriteBatch);
	}

	for (int i = 0; i < _bullets.size(); i++)
	{
		_bullets[i].draw(_agentSpriteBatch);
	}

	_agentSpriteBatch.end();

	_agentSpriteBatch.renderBatch();

	_colourProgram.UnUse();

    // Swap our buffer and draw everything to the screen!
    _window.Swap();
}