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
#include <cstdio>
#include <algorithm>
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
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
	m_currentLevel(0),
	m_player(nullptr),
	m_numHumansKilled(0),
	m_numZombiesKilled(0)
{
    // Empty
}

MainGame::~MainGame()
{
	m_virtualControls.dispose();
	if (m_spriteFont)
	{
		m_spriteFont->dispose();
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

	m_window.Create("Zombie Game", m_screenWidth, m_screenHeight, 0U);
	m_screenWidth = m_window.GetScreenWidth();
	m_screenHeight = m_window.GetScreenHeight();

	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

	initShaders();

	m_agentSpriteBatch.init();
	m_hudSpriteBatch.init();

	m_spriteFont = std::make_unique<GameEngine::SpriteFont>("Fonts/chintzy.ttf", 64);

	m_camera.Init(m_screenWidth, m_screenHeight);
	m_hudCamera.Init(m_screenWidth, m_screenHeight);
	m_hudCamera.SetPosition(glm::vec2(m_screenWidth / 2.0f, m_screenHeight / 2.0f));

	initBloodParticles();

	m_virtualControls.init(GameEngine::VirtualControlLayout::TWIN_STICK);
}

void MainGame::initBloodParticles()
{
	m_particleEngine.clear();
	m_bloodParticleBatch = std::make_unique<GameEngine::ParticleBatch2D>();
	m_bloodParticleBatch->init(1000, 0.05f,
		GameEngine::ResourceManager::GetTexture("Textures/particle.png"),
		[](GameEngine::Particle2D& particle, float deltaTime) {
		particle.position += particle.velocity * deltaTime;
		particle.colour.a = (GLubyte)(particle.life * 255.0f);
	});
	m_particleEngine.addParticleBatch(m_bloodParticleBatch.get());
}

void MainGame::initLevel()
{
	m_levels.push_back(std::make_unique<Level>("Levels/level1.txt"));
	m_currentLevel = 0;

	auto player = std::make_unique<Player>();
	m_player = player.get();
	m_player->init(PLAYER_SPEED, m_levels[m_currentLevel]->getStartPlayerPos(), &m_inputManager, &m_camera, &m_bullets);

	m_humans.push_back(std::move(player));

	std::mt19937 randomEngine;
	randomEngine.seed((unsigned int)time(nullptr));
	std::uniform_int_distribution<int> randomX(1, m_levels[m_currentLevel]->getWidth() - 2);
	std::uniform_int_distribution<int> randomY(1, m_levels[m_currentLevel]->getHeight() - 2);

	for (int i = 0; i < m_levels[m_currentLevel]->getNumHumans(); i++)
	{
		auto human = std::make_unique<Human>();
		glm::vec2 pos(randomX(randomEngine) * TILE_WIDTH, randomY(randomEngine) * TILE_WIDTH);
		human->init(HUMAN_SPEED, pos);
		m_humans.push_back(std::move(human));
	}

	const std::vector<glm::vec2>& zombiePositions = m_levels[m_currentLevel]->getZombiePositions();

	for (size_t i = 0; i < zombiePositions.size(); i++)
	{
		auto zombie = std::make_unique<Zombie>();
		zombie->init(ZOMBIE_SPEED, zombiePositions[i]);
		m_zombies.push_back(std::move(zombie));
	}

	const float BULLET_SPEED = 20.0f;
	m_player->AddGun(std::make_unique<Gun>("Magnum", 10, 1, 0.125f, 30, BULLET_SPEED, m_audioEngine.loadSoundEffect("Audio/Sounds/shots/pistol.wav")));
	m_player->AddGun(std::make_unique<Gun>("Shotgun", 30, 12, 0.3f, 4, BULLET_SPEED, m_audioEngine.loadSoundEffect("Audio/Sounds/shots/shotgun.wav")));
	m_player->AddGun(std::make_unique<Gun>("MP5", 2, 1, 0.3f, 20, BULLET_SPEED, m_audioEngine.loadSoundEffect("Audio/Sounds/shots/cg1.wav")));
}

void MainGame::restartLevel()
{
	m_bullets.clear();
	m_zombies.clear();
	m_humans.clear();
	m_player = nullptr;
	m_levels.clear();
	m_numHumansKilled = 0;
	m_numZombiesKilled = 0;

	initBloodParticles();
	initLevel();
	m_gameState = GameState::PLAY;
	if (m_player != nullptr)
	{
		m_camera.SetPosition(m_player->getPosition());
	}
}

void MainGame::initShaders()
{
	m_colourProgram.CompileShaders("Shaders/colourShading.vert", "Shaders/colourShading.frag");
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

	Uint32 previousTicks = SDL_GetTicks();

	while (m_gameState != GameState::EXIT)
	{
		fpsLimiter.begin();

		Uint32 newTicks = SDL_GetTicks();
		Uint32 frameTime = newTicks - previousTicks;
		previousTicks = newTicks;

		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		m_inputManager.update();

		processInput();

		if (m_gameState == GameState::PLAY)
		{
			checkVictory();

			int i = 0;
			while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS && m_gameState == GameState::PLAY)
			{
				float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
				updateAgents(deltaTime);

				if (m_gameState == GameState::PLAY)
				{
					updateBullets(deltaTime);
					checkVictory();
				}

				m_particleEngine.update(deltaTime);

				totalDeltaTime -= deltaTime;
				i++;
			}

			if (m_player != nullptr)
			{
				m_camera.SetPosition(m_player->getPosition());
			}
		}

		m_camera.Update();
		m_hudCamera.Update();

		drawGame();

		m_fps = fpsLimiter.end();
	}
}

void MainGame::updateAgents(float deltaTime)
{
	for (auto& h : m_humans)
	{
		h->update(m_levels[m_currentLevel]->getLevelData(), m_humans, m_zombies, deltaTime);
	}

	for (auto& z : m_zombies)
	{
		z->update(m_levels[m_currentLevel]->getLevelData(), m_humans, m_zombies, deltaTime);
	}

	for (size_t i = 0; i < m_zombies.size(); i++)
	{
		for (size_t j = i + 1; j < m_zombies.size(); j++)
		{
			m_zombies[i]->collideWithAgent(m_zombies[j].get());
		}
		for (size_t j = 1; j < m_humans.size();)
		{
			if (m_zombies[i]->collideWithAgent(m_humans[j].get()))
			{
				auto zombie = std::make_unique<Zombie>();
				zombie->init(ZOMBIE_SPEED, m_humans[j]->getPosition());
				m_zombies.push_back(std::move(zombie));

				m_humans[j] = std::move(m_humans.back());
				m_humans.pop_back();
			}
			else
			{
				j++;
			}
		}
		if (m_player != nullptr && m_zombies[i]->collideWithAgent(m_player))
		{
			m_gameState = GameState::LOST;
			return;
		}
	}

	for (size_t i = 0; i < m_humans.size(); i++)
	{
		for (size_t j = i + 1; j < m_humans.size(); j++)
		{
			m_humans[i]->collideWithAgent(m_humans[j].get());
		}
	}
}

void MainGame::updateBullets(float deltaTime)
{
	for (size_t i = 0; i < m_bullets.size();)
	{
		if (m_bullets[i].update(m_levels[m_currentLevel]->getLevelData(), deltaTime))
		{
			m_bullets[i] = m_bullets.back();
			m_bullets.pop_back();
		}
		else
			i++;
	}

	for (size_t i = 0; i < m_bullets.size();)
	{
		bool wasBulletRemoved = false;
		for (size_t j = 0; j < m_zombies.size();)
		{
			if (m_bullets[i].collideWithAgent(m_zombies[j].get()))
			{
				addBlood(m_bullets[i].getPosition(), 5);

				if (m_zombies[j]->applyDamage(m_bullets[i].getDamage()))
				{
					m_zombies[j] = std::move(m_zombies.back());
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
		if (!wasBulletRemoved)
		{
			for (size_t j = 1; j < m_humans.size();)
			{
				if (m_bullets[i].collideWithAgent(m_humans[j].get()))
				{
					addBlood(m_bullets[i].getPosition(), 5);

					if (m_humans[j]->applyDamage(m_bullets[i].getDamage()))
					{
						m_humans[j] = std::move(m_humans.back());
						m_humans.pop_back();
						m_numHumansKilled++;
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
		}
		if (!wasBulletRemoved)
		{
			i++;
		}
	}
}

void MainGame::checkVictory()
{
	if (m_zombies.empty() && m_gameState == GameState::PLAY)
	{
		std::printf("***You win! ***\n You killed %d humans and %d zombies. There are %d/%d civilians remaining\n",
			m_numHumansKilled, m_numZombiesKilled, std::max(0, static_cast<int>(m_humans.size()) - 1), m_levels[m_currentLevel]->getNumHumans());
		m_gameState = GameState::WON;
	}
}

void MainGame::processInput() {
    SDL_Event evnt;
    //Will keep looping until there are no more events to process
    while (SDL_PollEvent(&evnt)) {
        if (m_virtualControls.onSDLEvent(evnt, m_inputManager, m_screenWidth, m_screenHeight)) {
            continue;
        }
        switch (evnt.type) {
            case SDL_QUIT:
				m_gameState = GameState::EXIT;
                // Exit the game here!
                break;
            case SDL_WINDOWEVENT:
                if (evnt.window.event == SDL_WINDOWEVENT_RESIZED || evnt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    m_window.OnResize(evnt.window.data1, evnt.window.data2);
                    m_screenWidth = m_window.GetScreenWidth();
                    m_screenHeight = m_window.GetScreenHeight();
                    m_camera.Resize(m_screenWidth, m_screenHeight);
                    m_hudCamera.Resize(m_screenWidth, m_screenHeight);
                    m_hudCamera.SetPosition(glm::vec2(m_screenWidth / 2.0f, m_screenHeight / 2.0f));
                }
                break;
            case SDL_MOUSEMOTION:
                m_inputManager.setMouseCoords((float)evnt.motion.x, (float)evnt.motion.y);
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
            case SDL_FINGERDOWN:
                if (m_gameState == GameState::WON || m_gameState == GameState::LOST) {
                    restartLevel();
                    break;
                }
                m_inputManager.onTouchDown(evnt.tfinger.fingerId, evnt.tfinger.x, evnt.tfinger.y,
                                           (float)m_screenWidth, (float)m_screenHeight);
                break;
            case SDL_FINGERUP:
                m_inputManager.onTouchUp(evnt.tfinger.fingerId, evnt.tfinger.x, evnt.tfinger.y,
                                         (float)m_screenWidth, (float)m_screenHeight);
                break;
            case SDL_FINGERMOTION:
                m_inputManager.onTouchMotion(evnt.tfinger.fingerId, evnt.tfinger.x, evnt.tfinger.y,
                                             evnt.tfinger.dx, evnt.tfinger.dy,
                                             (float)m_screenWidth, (float)m_screenHeight);
                break;
        }
    }

    m_virtualControls.update(m_inputManager, m_screenWidth, m_screenHeight);

    if (m_inputManager.isKeyPressed(SDLK_ESCAPE)) {
        m_gameState = GameState::EXIT;
    }

    if ((m_gameState == GameState::WON || m_gameState == GameState::LOST) &&
        m_inputManager.isKeyPressed(SDLK_r)) {
        restartLevel();
    }
}

void MainGame::drawGame() {
    // Set the base depth to 1.0
#ifdef __ANDROID__
    glClearDepthf(1.0f);
#else
    glClearDepth(1.0);
#endif
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

	for (auto& h : m_humans)
	{
		if (m_camera.isBoxInView(h->getPosition(), agentDims))
			h->draw(m_agentSpriteBatch);
	}
	for (auto& z : m_zombies)
	{
		if (m_camera.isBoxInView(z->getPosition(), agentDims))
			z->draw(m_agentSpriteBatch);
	}

	for (auto& b : m_bullets)
	{
		b.draw(m_agentSpriteBatch);
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

	snprintf(buffer, sizeof(buffer), "Num Humans %zu", m_humans.size());

	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(0, 0),
		glm::vec2(0.5), 0.0f, GameEngine::ColourRGBA8(255, 255, 255, 255));

	snprintf(buffer, sizeof(buffer), "Num Zombies %zu", m_zombies.size());

	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(0, 36),
		glm::vec2(0.5), 0.0f, GameEngine::ColourRGBA8(255, 255, 255, 255));

	if (m_gameState == GameState::WON || m_gameState == GameState::LOST)
	{
		auto drawCenteredShadowText = [&](const char* text, float y, float scale, GameEngine::ColourRGBA8 colour) {
			m_spriteFont->draw(m_hudSpriteBatch, text, glm::vec2(m_screenWidth / 2.0f + 2.0f, y - 2.0f),
				glm::vec2(scale), 0.0f, GameEngine::ColourRGBA8(0, 0, 0, 220), GameEngine::Justification::MIDDLE);
			m_spriteFont->draw(m_hudSpriteBatch, text, glm::vec2(m_screenWidth / 2.0f, y),
				glm::vec2(scale), 0.0f, colour, GameEngine::Justification::MIDDLE);
		};

		const float centerY = m_screenHeight / 2.0f;
		if (m_gameState == GameState::WON)
		{
			drawCenteredShadowText("YOU WIN!", centerY + 70.0f, 1.0f, GameEngine::ColourRGBA8(50, 255, 50, 255));
		}
		else
		{
			drawCenteredShadowText("YOU LOSE!", centerY + 70.0f, 1.0f, GameEngine::ColourRGBA8(255, 60, 60, 255));
		}

		const int civiliansRemaining = std::max(0, static_cast<int>(m_humans.size()) - 1);
		const int totalCivilians = m_levels.empty() ? 0 : m_levels[m_currentLevel]->getNumHumans();
		snprintf(buffer, sizeof(buffer), "Humans Killed: %d   Zombies Killed: %d   Civilians Remaining: %d/%d",
			m_numHumansKilled, m_numZombiesKilled, civiliansRemaining, totalCivilians);
		drawCenteredShadowText(buffer, centerY + 10.0f, 0.45f, GameEngine::ColourRGBA8(255, 255, 255, 255));

		drawCenteredShadowText("Press R to Restart or ESC to Quit", centerY - 40.0f, 0.5f, GameEngine::ColourRGBA8(255, 230, 120, 255));
	}

	m_virtualControls.draw(m_hudSpriteBatch, m_screenWidth, m_screenHeight, m_spriteFont.get());

	m_hudSpriteBatch.end();

	m_hudSpriteBatch.renderBatch();
}

void MainGame::addBlood(const glm::vec2 & position, int numParticles)
{
	static std::mt19937 randEngine((unsigned int)time(nullptr));
	static std::uniform_real_distribution<float> randAngle(0.0f, 360.0f);

	glm::vec2 velocity(2.0f, 0.0f);
	GameEngine::ColourRGBA8 col(255, 0, 0, 255);

	for (int i = 0; i < numParticles; i++)
	{
		m_bloodParticleBatch->addParticle(position, glm::rotate(velocity, randAngle(randEngine)), col, 30.0f);
	}
}
