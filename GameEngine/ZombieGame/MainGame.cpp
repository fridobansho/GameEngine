#include "MainGame.h"

#include <GameEngine/GameEngine.h>
#include <GameEngine/Timing.h>
#include <GameEngine/GameEngineErrors.h>
#include <GameEngine/SoA/SpriteFont.h>
#include <GameEngine/ResourceManager.h>

#include <SDL.h>
#include <iostream>
#include <random>
#include <ctime>
#include <algorithm>
#include <glm/gtx/rotate_vector.hpp>

#include "Gun.h"

const float HUMAN_SPEED = 1.0f;
const float ZOMBIE_SPEED = 1.3f;
const float PLAYER_SPEED = 5.0f;

MainGame::MainGame() :
	m_screenWidth(1024),
	m_screenHeight(768),
	m_gameState(GameState::PLAY),
	m_fps(0),
	m_player(nullptr),
	m_numHumansKilled(0),
	m_numZombiesKilled(0)
{
    // Empty
}

MainGame::~MainGame()
{
	for (int i = 0; i < m_levels.size(); i++)
	{
		delete m_levels[i];
	}
	for (int i = 0; i < m_humans.size(); i++)
	{
		delete m_humans[i];
	}
	for (int i = 0; i < m_zombies.size(); i++)
	{
		delete m_zombies[i];
	}
}

void MainGame::Run()
{
	initSystems();

	initLevel();

	GameEngine::Music music = m_audioEngine.loadMusic("Audio/Music/XYZ_0.ogg");
	music.play(-1);

	gameLoop();
}

void MainGame::initSystems() {
	GameEngine::Init();

	m_audioEngine.init();

	m_window.Create("Zombie Game", m_screenWidth, m_screenHeight, 0);

	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

	initShaders();

	m_agentSpriteBatch.init();
	m_hudSpriteBatch.init();

	m_spriteFont = new GameEngine::SpriteFont("Fonts/chintzy.ttf", 64);

	m_camera.Init(m_screenWidth, m_screenHeight);
	m_hudCamera.Init(m_screenWidth, m_screenHeight);
	m_hudCamera.SetPosition(glm::vec2(m_screenWidth / 2.0f, m_screenHeight / 2.0f));

	m_bloodParticleBatch = new GameEngine::ParticleBatch2D();
	m_bloodParticleBatch->init(1000, 0.05f, GameEngine::ResourceManager::GetTexture("Textures/particle.png"));
	m_particleEngine.addParticleBatch(m_bloodParticleBatch);

}

void MainGame::initLevel()
{
	m_levels.push_back(new Level("Levels/level1.txt"));
	m_currentLevel = 0;

	m_player = new Player();
	m_player->init(PLAYER_SPEED, m_levels[m_currentLevel]->getStartPlayerPos(), &m_inputManager, &m_camera, &m_bullets);

	m_humans.push_back(m_player);

	std::mt19937 randomEngine;
	randomEngine.seed(time(nullptr));
	std::uniform_int_distribution<int> randomX(1, m_levels[m_currentLevel]->getWidth() - 2);
	std::uniform_int_distribution<int> randomY(1, m_levels[m_currentLevel]->getHeight() - 2);

	for (int i = 0; i < m_levels[m_currentLevel]->getNumHumans(); i++)
	{
		m_humans.push_back(new Human);
		glm::vec2 pos(randomX(randomEngine) * TILE_WIDTH, randomY(randomEngine) * TILE_WIDTH);
		m_humans.back()->init(HUMAN_SPEED, pos);
	}

	const std::vector<glm::vec2>& zombiePositions = m_levels[m_currentLevel]->getZombiePositions();

	for (int i = 0; i < zombiePositions.size(); i++)
	{
		m_zombies.push_back(new Zombie);
		m_zombies.back()->init(ZOMBIE_SPEED, zombiePositions[i]);
	}

	const float BULLET_SPEED = 20.0f;
	m_player->AddGun(new Gun("Magnum", 10, 1, 0.125f, 30, BULLET_SPEED, m_audioEngine.loadSoundEffect("Audio/Sounds/shots/pistol.wav")));
	m_player->AddGun(new Gun("Shotgun", 30, 12, 0.3f, 4, BULLET_SPEED, m_audioEngine.loadSoundEffect("Audio/Sounds/shots/shotgun.wav")));
	m_player->AddGun(new Gun("MP5", 2, 1, 0.3f, 20, BULLET_SPEED, m_audioEngine.loadSoundEffect("Audio/Sounds/shots/cg1.wav")));
}

void MainGame::initShaders()
{
	m_colourProgram.CompileShaders("Shaders\\colourShading.vert", "Shaders\\colourShading.frag");
	m_colourProgram.AddAttribute("vertexPosition");
	m_colourProgram.AddAttribute("vertexColour");
	m_colourProgram.AddAttribute("vertexUV");
	m_colourProgram.LinkShaders();
}

void MainGame::gameLoop()
{
	const float DESIRED_FPS = 60.0f;
	const int MAX_PHYSICS_STEPS = 6;
	GameEngine::FPSLimiter fpsLimiter;
	fpsLimiter.setMaxFPS(DESIRED_FPS);

	const float CAMERA_SCALE = 1.0f / 4.0f;
	m_camera.SetScale(CAMERA_SCALE);

	const float MS_PER_SECOND = 1000.0f;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;

	float previousTicks = SDL_GetTicks();

	while (m_gameState == GameState::PLAY)
	{
		fpsLimiter.begin();

		float newTicks = SDL_GetTicks();
		float frameTime = newTicks - previousTicks;
		previousTicks = newTicks;

		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		checkVictory();

		m_inputManager.update();

		processInput();

		int i = 0;
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS)
		{
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			updateAgents(deltaTime);

			updateBullets(deltaTime);

			m_particleEngine.update(deltaTime);

			totalDeltaTime -= deltaTime;
			i++;
		}

		m_camera.SetPosition(m_player->getPosition());

		m_camera.Update();
		m_hudCamera.Update();

		drawGame();

		m_fps = fpsLimiter.end();
	}
}

void MainGame::updateAgents(float deltaTime)
{
	for (int i = 0; i < m_humans.size(); i++)
	{
		m_humans[i]->update(m_levels[m_currentLevel]->getLevelData(), m_humans, m_zombies, deltaTime);
	}

	for (int i = 0; i < m_zombies.size(); i++)
	{
		m_zombies[i]->update(m_levels[m_currentLevel]->getLevelData(), m_humans, m_zombies, deltaTime);
	}

	for (int i = 0; i < m_zombies.size(); i++)
	{
		for (int j = i + 1; j < m_zombies.size(); j++)
		{
			m_zombies[i]->collideWithAgent(m_zombies[j]);
		}
		for (int j =1; j < m_humans.size(); j++)
		{
			if (m_zombies[i]->collideWithAgent(m_humans[j]))
			{
				m_zombies.push_back(new Zombie);
				m_zombies.back()->init(ZOMBIE_SPEED, m_humans[j]->getPosition());

				delete m_humans[j];
				m_humans[j] = m_humans.back();
				m_humans.pop_back();
			}
		}
		if (m_zombies[i]->collideWithAgent(m_player))
		{
			GameEngine::FatalError("You lose");
		}
	}

	for (int i = 0; i < m_humans.size(); i++)
	{
		for (int j = i + 1; j < m_humans.size(); j++)
		{
			m_humans[i]->collideWithAgent(m_humans[j]);
		}
	}
}

void MainGame::updateBullets(float deltaTime)
{
	for (int i = 0; i < m_bullets.size();)
	{
		if (m_bullets[i].update(m_levels[m_currentLevel]->getLevelData(), deltaTime))
		{
			m_bullets[i] = m_bullets.back();
			m_bullets.pop_back();
		}
		else
			i++;
	}

	bool wasBulletRemoved;

	for (int i = 0; i < m_bullets.size();i++)
	{
		wasBulletRemoved = false;
		for (int j = 0; j < m_zombies.size();)
		{
			if (m_bullets[i].collideWithAgent(m_zombies[j]))
			{
				addBlood(m_bullets[i].getPosition(), 5);

				if (m_zombies[j]->applyDamage(m_bullets[i].getDamage()))
				{
					delete m_zombies[j];
					m_zombies[j] = m_zombies.back();
					m_zombies.pop_back();
					m_numZombiesKilled++;
				}
				else
					j++;

				m_bullets[i] = m_bullets.back();
				m_bullets.pop_back();
				wasBulletRemoved = true;
				break;
			}
			else
				j++;
		}
		if (wasBulletRemoved == false)
		{
			for (int j = 1; j < m_humans.size();)
			{
				if (m_bullets[i].collideWithAgent(m_humans[j]))
				{
					addBlood(m_bullets[i].getPosition(), 5);

					if (m_humans[j]->applyDamage(m_bullets[i].getDamage()))
					{
						delete m_humans[j];
						m_humans[j] = m_humans.back();
						m_humans.pop_back();
					}
					else
						j++;

					m_bullets[i] = m_bullets.back();
					m_bullets.pop_back();
					wasBulletRemoved = true;
					m_numHumansKilled++;
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
	if (m_zombies.empty())
	{
		std::printf("***You win! ***\n You killed %d humans and %d zombies. There are %d/%d civilians remaining",
			m_numHumansKilled, m_numZombiesKilled, m_humans.size() - 1, m_levels[m_currentLevel]->getNumHumans());
		GameEngine::FatalError("");
	}
}

void MainGame::processInput() {
    SDL_Event evnt;
    //Will keep looping until there are no more events to process
    while (SDL_PollEvent(&evnt)) {
        switch (evnt.type) {
            case SDL_QUIT:
				m_gameState = GameState::EXIT;
                // Exit the game here!
                break;
            case SDL_MOUSEMOTION:
                m_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
                break;
            case SDL_KEYDOWN:
                m_inputManager.pressKey(evnt.key.keysym.sym);
                break;
            case SDL_KEYUP:
                m_inputManager.releaseKey(evnt.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                m_inputManager.pressKey(evnt.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                m_inputManager.releaseKey(evnt.button.button);
                break;
        }
    }
}

void MainGame::drawGame() {
    // Set the base depth to 1.0
    glClearDepth(1.0);
    // Clear the color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_colourProgram.Use();

	glActiveTexture(GL_TEXTURE0);

	GLint textureUniform = m_colourProgram.GetUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
	GLint pUniform = m_colourProgram.GetUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	m_levels[m_currentLevel]->draw();

	m_agentSpriteBatch.begin();

	const glm::vec2 agentDims(AGENT_RADIUS * 2.0f);

	for (int i = 0; i < m_humans.size(); i++)
	{
		if (m_camera.isBoxInView(m_humans[i]->getPosition(), agentDims))
			m_humans[i]->draw(m_agentSpriteBatch);
	}
	for (int i = 0; i < m_zombies.size(); i++)
	{
		if (m_camera.isBoxInView(m_zombies[i]->getPosition(), agentDims))
			m_zombies[i]->draw(m_agentSpriteBatch);
	}

	for (int i = 0; i < m_bullets.size(); i++)
	{
		m_bullets[i].draw(m_agentSpriteBatch);
	}

	m_agentSpriteBatch.end();

	m_agentSpriteBatch.renderBatch();

	m_particleEngine.draw(&m_agentSpriteBatch);

	drawHud();

	m_colourProgram.UnUse();

    // Swap our buffer and draw everything to the screen!
    m_window.Swap();
}

void MainGame::drawHud()
{
	char buffer[256];

	glm::mat4 projectionMatrix = m_hudCamera.getCameraMatrix();
	GLint pUniform = m_colourProgram.GetUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	m_hudSpriteBatch.begin();

	sprintf_s(buffer, "Num Humans %d", m_humans.size());

	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(0, 0),
		glm::vec2(0.5), 0.0f, GameEngine::ColourRGBA8(255, 255, 255, 255));

	sprintf_s(buffer, "Num Zombies %d", m_zombies.size());

	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(0, 36),
		glm::vec2(0.5), 0.0f, GameEngine::ColourRGBA8(255, 255, 255, 255));

	m_hudSpriteBatch.end();

	m_hudSpriteBatch.renderBatch();
}

void MainGame::addBlood(const glm::vec2 & position, int numParticles)
{
	static mt19937 randEngine(time(nullptr));
	static uniform_real_distribution<float> randAngle(0.0f, 360.0f);

	glm::vec2 velocity(2.0f, 0.0f);
	GameEngine::ColourRGBA8 col(255, 0, 0, 255);

	for (int i = 0; i < numParticles; i++)
	{
		m_bloodParticleBatch->addParticle(position, glm::rotate(velocity, randAngle(randEngine)), col, 30.0f);
	}
}
