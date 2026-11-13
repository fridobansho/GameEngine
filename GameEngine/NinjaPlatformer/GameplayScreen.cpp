#include "GameplayScreen.h"
#include <iostream>
#include <SDL.h>
#include <GameEngine/IMainGame.h>
#include <GameEngine/ResourceManager.h>
#include <random>

#include "LevelReaderWriter.h"
#include "ScreenIndices.h"

namespace {
	bool endsWithBin(const std::string& path) {
		return path.size() >= 4 && path.compare(path.size() - 4, 4, ".bin") == 0;
	}
}

GameplayScreen::GameplayScreen(GameEngine::Window* window) : m_window(window) {
	m_screenIndex = SCREEN_INDEX_GAMEPLAY;
}

GameplayScreen::~GameplayScreen() {

}

int GameplayScreen::getNextScreenIndex() const {
	return SCREEN_INDEX_NO_SCREEN;
}

int GameplayScreen::getPreviousScreenIndex() const {
	return SCREEN_INDEX_MAINMENU;
}

void GameplayScreen::build() {
}

void GameplayScreen::destroy() {
}

void GameplayScreen::onEntry() {
	m_levelComplete = false;

	b2Vec2 gravity(0.0f, -25.0);
	m_world = std::make_unique<b2World>(gravity);

	m_debugRenderer.init();

	// Load textures
	m_texture = GameEngine::ResourceManager::GetTexture("Assets/bricks_top.png");
	m_blankTexture = GameEngine::ResourceManager::GetTexture("Assets/blank.png");

	// Initialize spritebatch, font, and virtual controls
	m_spriteBatch.init();
	m_spriteFont.init("Fonts/chintzy.ttf", 32);
	m_virtualControls.init(GameEngine::VirtualControlLayout::PLATFORMER);

	// Shader init
	// Compile our texture shader
	m_textureProgram.CompileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
	m_textureProgram.AddAttribute("vertexPosition");
	m_textureProgram.AddAttribute("vertexColor");
	m_textureProgram.AddAttribute("vertexUV");
	m_textureProgram.LinkShaders();
	// Compile our light shader
	m_lightProgram.CompileShaders("Shaders/lightShading.vert", "Shaders/lightShading.frag");
	m_lightProgram.AddAttribute("vertexPosition");
	m_lightProgram.AddAttribute("vertexColor");
	m_lightProgram.AddAttribute("vertexUV");
	m_lightProgram.LinkShaders();

	// Init cameras
	const int w = m_window->GetScreenWidth();
	const int h = m_window->GetScreenHeight();
	m_camera.Init(w, h);
	m_camera.SetScale(32.0f);

	// Bottom-left origin HUD camera matching VirtualControls::draw projection
	m_hudCamera.Init(w, h);
	m_hudCamera.SetScale(1.0f);
	m_hudCamera.SetPosition(glm::vec2(w * 0.5f, h * 0.5f));

	// Load level from current path, falling back to procedural boxes if loading fails
	const std::string& levelPath = LevelReaderWriter::getCurrentLevelPath();
	bool loaded = endsWithBin(levelPath)
		? LevelReaderWriter::loadAsBinary(levelPath, m_world.get(), m_player, m_boxes, m_lights)
		: LevelReaderWriter::loadAsText(levelPath, m_world.get(), m_player, m_boxes, m_lights);

	if (!loaded) {
		m_boxes.clear();
		m_lights.clear();

		// Make the ground
		Box groundBox;
		groundBox.init(m_world.get(), glm::vec2(0.0f, -20.0f), glm::vec2(50.0f, 10.0f), m_texture, GameEngine::ColourRGBA8(255, 255, 255, 255), false, false);
		m_boxes.push_back(groundBox);

		// Make a bunch of boxes
		std::mt19937 randGenerator;
		std::uniform_real_distribution<float> xPos(-10.0, 10.0f);
		std::uniform_real_distribution<float> yPos(-10.0, 25.0f);
		std::uniform_real_distribution<float> size(0.5, 2.5f);
		std::uniform_int_distribution<int> colour(50, 255);
		const int NUM_BOXES = 10;

		for (int i = 0; i < NUM_BOXES; i++) {
			GameEngine::ColourRGBA8 randColour;
			randColour.r = colour(randGenerator);
			randColour.g = colour(randGenerator);
			randColour.b = colour(randGenerator);
			randColour.a = 255;
			Box newBox;
			newBox.init(m_world.get(), glm::vec2(xPos(randGenerator), yPos(randGenerator)), glm::vec2(size(randGenerator), size(randGenerator)), m_texture, randColour, false, true);
			m_boxes.push_back(newBox);
		}

		// Init player
		m_player.init(m_world.get(), glm::vec2(0.0f, 30.0f), glm::vec2(2.0f), glm::vec2(1.0f, 1.8f), GameEngine::ColourRGBA8(255, 255, 255, 255));
	}

	m_camera.SetPosition(m_player.getPosition());

	initUI();
}

void GameplayScreen::onExit() {
	m_gui.destroy();
	m_spriteBatch.dispose();
	m_textureProgram.Dispose();
	m_lightProgram.Dispose();
	m_debugRenderer.dispose();
	m_virtualControls.dispose();
	m_spriteFont.dispose();
	m_boxes.clear();
	m_lights.clear();
	m_world.reset();
}

void GameplayScreen::update() {
	m_camera.Update();
	m_hudCamera.Update();
	checkInput();

	if (!m_world || m_currentState != GameEngine::ScreenState::RUNNING) {
		return;
	}

	m_virtualControls.update(m_game->inputManager, m_window->GetScreenWidth(), m_window->GetScreenHeight());

	if (!m_levelComplete) {
		m_player.update(m_game->inputManager);

		// Update the physics simulation
		m_world->Step(1.0f / 60.0f, 6, 2);

		m_camera.SetPosition(m_player.getPosition());

		// Check if player touched any goal box
		b2Body* playerBody = m_player.getCapsule().getBody();
		if (playerBody) {
			for (b2ContactEdge* ce = playerBody->GetContactList(); ce != nullptr; ce = ce->next) {
				b2Contact* c = ce->contact;
				if (!c || !c->IsTouching()) continue;
				b2Fixture* fixA = c->GetFixtureA();
				b2Fixture* fixB = c->GetFixtureB();
				for (const auto& b : m_boxes) {
					if (b.isGoal() && (fixA == b.getFixture() || fixB == b.getFixture())) {
						m_levelComplete = true;
						break;
					}
				}
				if (m_levelComplete) break;
			}
		}
	}

	m_gui.update();
}

void GameplayScreen::draw() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_textureProgram.Use();

	// Upload texture uniform
	GLint textureUniform = m_textureProgram.GetUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);
	glActiveTexture(GL_TEXTURE0);

	// Camera matrix
	glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
	GLint pUniform = m_textureProgram.GetUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	m_spriteBatch.begin();

	// Draw all the boxes
	for (auto& b : m_boxes) {
		b.draw(m_spriteBatch);
	}
	m_player.draw(m_spriteBatch);

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();
	m_textureProgram.UnUse();

	// Debug rendering
	if (m_renderDebug) {
		glm::vec4 destRect;
		for (auto& b : m_boxes) {
			destRect.x = b.getBody()->GetPosition().x - b.getDimensions().x / 2.0f;
			destRect.y = b.getBody()->GetPosition().y - b.getDimensions().y / 2.0f;
			destRect.z = b.getDimensions().x;
			destRect.w = b.getDimensions().y;
			GameEngine::ColourRGBA8 col = b.isGoal()
				? GameEngine::ColourRGBA8(0, 255, 255, 255)
				: GameEngine::ColourRGBA8(255, 255, 255, 255);
			m_debugRenderer.drawBox(destRect, col, b.getBody()->GetAngle());
		}
		m_player.drawDebug(m_debugRenderer);
		m_debugRenderer.end();
		m_debugRenderer.render(projectionMatrix, 2.0f);
	}

	// Render level lights (or fallback lights if level has none)
	m_lightProgram.Use();
	pUniform = m_lightProgram.GetUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	// Additive blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	m_spriteBatch.begin();

	if (!m_lights.empty()) {
		for (auto& l : m_lights) {
			l.draw(m_spriteBatch);
		}
	} else {
		Light playerLight;
		playerLight.colour = GameEngine::ColourRGBA8(255, 255, 255, 128);
		playerLight.position = m_player.getPosition();
		playerLight.size = 30.0f;
		playerLight.draw(m_spriteBatch);
	}

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_lightProgram.UnUse();

	// Reset to regular alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Render HUD (virtual controls, fallback Menu button, victory banner)
	m_textureProgram.Use();
	glUniform1i(textureUniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glm::mat4 hudMatrix = m_hudCamera.getCameraMatrix();
	pUniform = m_textureProgram.GetUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &hudMatrix[0][0]);

	m_spriteBatch.begin();

	const int sw = m_window->GetScreenWidth();
	const int sh = m_window->GetScreenHeight();
	m_virtualControls.draw(m_spriteBatch, sw, sh, &m_spriteFont);

#ifndef GAMEENGINE_HAS_CEGUI
	{
		glm::vec4 menuRect = getMenuButtonRect();
		m_spriteBatch.draw(menuRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_blankTexture.id, 0.0f,
		                   GameEngine::ColourRGBA8(40, 50, 80, 210));
		m_spriteFont.draw(m_spriteBatch, "Menu",
		                  glm::vec2(menuRect.x + menuRect.z * 0.5f, menuRect.y + 6.0f),
		                  glm::vec2(0.65f), 0.1f, GameEngine::ColourRGBA8(255, 255, 255, 255),
		                  GameEngine::Justification::MIDDLE);
	}
#endif

	if (m_levelComplete) {
		const float bannerW = 620.0f;
		const float bannerH = 110.0f;
		const float bx = sw * 0.5f - bannerW * 0.5f;
		const float by = sh * 0.5f - bannerH * 0.5f;
		m_spriteBatch.draw(glm::vec4(bx, by, bannerW, bannerH), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		                   m_blankTexture.id, 0.2f, GameEngine::ColourRGBA8(15, 25, 45, 225));
		m_spriteFont.draw(m_spriteBatch, "Level Complete!",
		                  glm::vec2(sw * 0.5f, sh * 0.5f + 12.0f),
		                  glm::vec2(0.95f), 0.3f, GameEngine::ColourRGBA8(255, 220, 80, 255),
		                  GameEngine::Justification::MIDDLE);
		m_spriteFont.draw(m_spriteBatch, "Press Enter or Tap to return to Menu",
		                  glm::vec2(sw * 0.5f, sh * 0.5f - 32.0f),
		                  glm::vec2(0.55f), 0.3f, GameEngine::ColourRGBA8(230, 240, 255, 255),
		                  GameEngine::Justification::MIDDLE);
	}

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();
	m_textureProgram.UnUse();

	m_gui.draw();
}

glm::vec4 GameplayScreen::getMenuButtonRect() const {
	const float sh = static_cast<float>(m_window->GetScreenHeight());
	return glm::vec4(12.0f, sh - 48.0f, 110.0f, 36.0f);
}

void GameplayScreen::initUI() {
#ifdef GAMEENGINE_HAS_CEGUI
	// Init the UI
	m_gui.init("GUI");
	m_gui.loadScheme("TaharezLook.scheme");
	m_gui.setFont("DejaVuSans-10");
	CEGUI::PushButton* testButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.01f, 0.01f, 0.1f, 0.05f), glm::vec4(0.0f), "TestButton"));
	testButton->setText("Exit Game!");

	// Set the event to be called when we click
	testButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::onExitClicked, this));

	m_gui.setMouseCursor("TaharezLook/MouseArrow");
	m_gui.showMouseCursor();
	SDL_ShowCursor(0);
#else
	SDL_ShowCursor(1);
#endif
}

void GameplayScreen::checkInput() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
#ifndef GAMEENGINE_HAS_CEGUI
		if (evnt.type == SDL_MOUSEBUTTONDOWN && evnt.button.button == SDL_BUTTON_LEFT) {
			glm::vec4 menuRect = getMenuButtonRect();
			float hx = static_cast<float>(evnt.button.x);
			float hy = static_cast<float>(m_window->GetScreenHeight() - evnt.button.y);
			if (hx >= menuRect.x && hx <= menuRect.x + menuRect.z &&
			    hy >= menuRect.y && hy <= menuRect.y + menuRect.w) {
				m_currentState = GameEngine::ScreenState::CHANGE_PREVIOUS;
				continue;
			}
		} else if (evnt.type == SDL_FINGERDOWN) {
			glm::vec4 menuRect = getMenuButtonRect();
			float hx = evnt.tfinger.x * static_cast<float>(m_window->GetScreenWidth());
			float hy = (1.0f - evnt.tfinger.y) * static_cast<float>(m_window->GetScreenHeight());
			if (hx >= menuRect.x && hx <= menuRect.x + menuRect.z &&
			    hy >= menuRect.y && hy <= menuRect.y + menuRect.w) {
				m_currentState = GameEngine::ScreenState::CHANGE_PREVIOUS;
				continue;
			}
		}
#endif

		if (m_levelComplete) {
			if ((evnt.type == SDL_KEYDOWN &&
			     (evnt.key.keysym.sym == SDLK_RETURN || evnt.key.keysym.sym == SDLK_KP_ENTER ||
			      evnt.key.keysym.sym == SDLK_SPACE || evnt.key.keysym.sym == SDLK_ESCAPE)) ||
			    (evnt.type == SDL_MOUSEBUTTONDOWN && evnt.button.button == SDL_BUTTON_LEFT) ||
			    evnt.type == SDL_FINGERDOWN) {
				m_currentState = GameEngine::ScreenState::CHANGE_PREVIOUS;
				continue;
			}
		}

		if (evnt.type == SDL_KEYDOWN && evnt.key.keysym.sym == SDLK_ESCAPE) {
			m_currentState = GameEngine::ScreenState::CHANGE_PREVIOUS;
			continue;
		}

		const bool consumedByVirtual = m_virtualControls.onSDLEvent(
			evnt, m_game->inputManager, m_window->GetScreenWidth(), m_window->GetScreenHeight());

		if (!consumedByVirtual) {
			m_game->onSDLEvent(evnt);
			m_gui.onSDLEvent(evnt);
		}

		if (evnt.type == SDL_WINDOWEVENT &&
		    (evnt.window.event == SDL_WINDOWEVENT_RESIZED ||
		     evnt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
			m_window->OnResize(evnt.window.data1, evnt.window.data2);
			const int w = m_window->GetScreenWidth();
			const int h = m_window->GetScreenHeight();
			m_camera.Resize(w, h);
			m_hudCamera.Resize(w, h);
			m_hudCamera.SetPosition(glm::vec2(w * 0.5f, h * 0.5f));
			m_gui.onWindowResize(w, h);
		}
	}
}

#ifdef GAMEENGINE_HAS_CEGUI
bool GameplayScreen::onExitClicked(const CEGUI::EventArgs& e) {
	m_currentState = GameEngine::ScreenState::EXIT_APPLICATION;
	return true;
}
#endif
