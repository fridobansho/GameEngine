#include "MainMenuScreen.h"
#include <GameEngine/ResourceManager.h>

MainMenuScreen::MainMenuScreen(GameEngine::Window* window) : m_window(window) {
	m_screenIndex = SCREEN_INDEX_MAINMENU;
}

MainMenuScreen::~MainMenuScreen() {
}

int MainMenuScreen::getNextScreenIndex() const {
	return m_nextScreenIndex;
}

int MainMenuScreen::getPreviousScreenIndex() const {
	return SCREEN_INDEX_NO_SCREEN;
}

void MainMenuScreen::build() {

}

void MainMenuScreen::destroy() {

}

void MainMenuScreen::onEntry() {
	// Init camera
	m_camera.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
	m_camera.SetScale(32.0f);

#ifndef GAMEENGINE_HAS_CEGUI
	m_uiCamera.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
	m_uiCamera.SetScale(1.0f);
	m_spriteBatch.init();
	m_spriteFont.init("Fonts/chintzy.ttf", 32);
	m_textureProgram.CompileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
	m_textureProgram.AddAttribute("vertexPosition");
	m_textureProgram.AddAttribute("vertexColor");
	m_textureProgram.AddAttribute("vertexUV");
	m_textureProgram.LinkShaders();
	m_blankTexture = GameEngine::ResourceManager::GetTexture("Assets/blank.png");
	m_selectedIndex = 0;
#endif

	initUI();
}

void MainMenuScreen::onExit() {
#ifdef GAMEENGINE_HAS_CEGUI
	m_gui.destroy();
#else
	m_spriteFont.dispose();
	m_spriteBatch.dispose();
	m_textureProgram.Dispose();
#endif
}

void MainMenuScreen::update() {
	m_camera.Update();
#ifndef GAMEENGINE_HAS_CEGUI
	m_uiCamera.Update();
#endif
	checkInput();
}

void MainMenuScreen::draw() {
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef GAMEENGINE_HAS_CEGUI
	m_gui.draw();
#else
	drawFallbackMenu();
#endif
}

void MainMenuScreen::initUI() {
#ifdef GAMEENGINE_HAS_CEGUI
	// Init the UI
	m_gui.init("GUI");
	m_gui.loadScheme("TaharezLook.scheme");
	m_gui.setFont("DejaVuSans-10");

	CEGUI::PushButton* playGameButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.5f, 0.1f, 0.05f), glm::vec4(0.0f), "NewGameButton"));
	playGameButton->setText("New Game");
	// Set up event to be called when we click
	playGameButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::onNewGameClicked, this));

	CEGUI::PushButton* editorButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.56f, 0.1f, 0.05f), glm::vec4(0.0f), "EditorButton"));
	editorButton->setText("Level Editor");
	// Set up event to be called when we click
	editorButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::onEditorClicked, this));

	CEGUI::PushButton* exitButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.62f, 0.1f, 0.05f), glm::vec4(0.0f), "ExitButton"));
	exitButton->setText("Exit Game");
	// Set the event to be called when we click
	exitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::onExitClicked, this));

	m_gui.setMouseCursor("TaharezLook/MouseArrow");
	m_gui.showMouseCursor();
	SDL_ShowCursor(0);
#else
	SDL_ShowCursor(1);
#endif
}

void MainMenuScreen::activateMenuItem(int index) {
	switch (index) {
		case 0:
			m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
			m_currentState = GameEngine::ScreenState::CHANGE_NEXT;
			break;
		case 1:
			m_nextScreenIndex = SCREEN_INDEX_EDITOR;
			m_currentState = GameEngine::ScreenState::CHANGE_NEXT;
			break;
		case 2:
			m_currentState = GameEngine::ScreenState::EXIT_APPLICATION;
			break;
		default:
			break;
	}
}

void MainMenuScreen::checkInput() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
#ifdef GAMEENGINE_HAS_CEGUI
		m_gui.onSDLEvent(evnt);
#endif
		switch (evnt.type) {
			case SDL_QUIT:
				m_currentState = GameEngine::ScreenState::EXIT_APPLICATION;
				break;
			case SDL_WINDOWEVENT:
				if (evnt.window.event == SDL_WINDOWEVENT_RESIZED ||
				    evnt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					m_window->OnResize(evnt.window.data1, evnt.window.data2);
					m_camera.Resize(m_window->GetScreenWidth(), m_window->GetScreenHeight());
#ifdef GAMEENGINE_HAS_CEGUI
					m_gui.onWindowResize(m_window->GetScreenWidth(), m_window->GetScreenHeight());
#else
					m_uiCamera.Resize(m_window->GetScreenWidth(), m_window->GetScreenHeight());
#endif
				}
				break;
#ifndef GAMEENGINE_HAS_CEGUI
			case SDL_KEYDOWN:
				if (evnt.key.keysym.sym == SDLK_UP || evnt.key.keysym.sym == SDLK_w) {
					m_selectedIndex = (m_selectedIndex + 2) % 3;
				} else if (evnt.key.keysym.sym == SDLK_DOWN || evnt.key.keysym.sym == SDLK_s) {
					m_selectedIndex = (m_selectedIndex + 1) % 3;
				} else if (evnt.key.keysym.sym == SDLK_RETURN || evnt.key.keysym.sym == SDLK_KP_ENTER || evnt.key.keysym.sym == SDLK_SPACE) {
					activateMenuItem(m_selectedIndex);
				}
				break;
			case SDL_MOUSEMOTION: {
				int hit = hitTestMenu(static_cast<float>(evnt.motion.x), static_cast<float>(evnt.motion.y));
				if (hit >= 0) {
					m_selectedIndex = hit;
				}
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
				if (evnt.button.button == SDL_BUTTON_LEFT) {
					int hit = hitTestMenu(static_cast<float>(evnt.button.x), static_cast<float>(evnt.button.y));
					if (hit >= 0) {
						activateMenuItem(hit);
					}
				}
				break;
			case SDL_FINGERDOWN: {
				float sx = evnt.tfinger.x * static_cast<float>(m_window->GetScreenWidth());
				float sy = evnt.tfinger.y * static_cast<float>(m_window->GetScreenHeight());
				int hit = hitTestMenu(sx, sy);
				if (hit >= 0) {
					activateMenuItem(hit);
				}
				break;
			}
#else
			case SDL_FINGERDOWN: {
				// Forward single-finger tap as synthetic mouse down/up so CEGUI buttons respond on touch
				SDL_Event fakeMouse = {};
				fakeMouse.type = SDL_MOUSEBUTTONDOWN;
				fakeMouse.button.button = SDL_BUTTON_LEFT;
				fakeMouse.button.x = static_cast<int>(evnt.tfinger.x * static_cast<float>(m_window->GetScreenWidth()));
				fakeMouse.button.y = static_cast<int>(evnt.tfinger.y * static_cast<float>(m_window->GetScreenHeight()));
				m_gui.onSDLEvent(fakeMouse);
				fakeMouse.type = SDL_MOUSEBUTTONUP;
				m_gui.onSDLEvent(fakeMouse);
				break;
			}
#endif
		}
	}
}

#ifdef GAMEENGINE_HAS_CEGUI
bool MainMenuScreen::onNewGameClicked(const CEGUI::EventArgs& e) {
	activateMenuItem(0);
	return true;
}

bool MainMenuScreen::onEditorClicked(const CEGUI::EventArgs& e) {
	activateMenuItem(1);
	return true;
}

bool MainMenuScreen::onExitClicked(const CEGUI::EventArgs& e) {
	activateMenuItem(2);
	return true;
}
#else
glm::vec4 MainMenuScreen::getMenuButtonRect(int index) const {
	const float btnW = 280.0f;
	const float btnH = 52.0f;
	const float spacing = 70.0f;
	const float startY = 30.0f;
	return glm::vec4(-btnW * 0.5f, startY - index * spacing, btnW, btnH);
}

int MainMenuScreen::hitTestMenu(float screenX, float screenY) {
	glm::vec2 uiPos = m_uiCamera.convertScreenToWorld(glm::vec2(screenX, screenY));
	for (int i = 0; i < 3; ++i) {
		glm::vec4 r = getMenuButtonRect(i);
		if (uiPos.x >= r.x && uiPos.x <= r.x + r.z &&
		    uiPos.y >= r.y && uiPos.y <= r.y + r.w) {
			return i;
		}
	}
	return -1;
}

void MainMenuScreen::drawFallbackMenu() {
	m_textureProgram.Use();
	GLint textureUniform = m_textureProgram.GetUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);
	glActiveTexture(GL_TEXTURE0);

	glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
	GLint pUniform = m_textureProgram.GetUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	m_spriteBatch.begin();

	m_spriteFont.draw(m_spriteBatch, "NINJA PLATFORMER", glm::vec2(0.0f, 140.0f),
	                  glm::vec2(1.0f), 0.0f, GameEngine::ColourRGBA8(255, 220, 100, 255),
	                  GameEngine::Justification::MIDDLE);

	const char* labels[3] = { "New Game", "Level Editor", "Exit Game" };
	for (int i = 0; i < 3; ++i) {
		glm::vec4 rect = getMenuButtonRect(i);
		bool selected = (i == m_selectedIndex);
		GameEngine::ColourRGBA8 bgCol = selected
			? GameEngine::ColourRGBA8(60, 120, 200, 230)
			: GameEngine::ColourRGBA8(40, 50, 80, 200);
		GameEngine::ColourRGBA8 txtCol = selected
			? GameEngine::ColourRGBA8(255, 255, 160, 255)
			: GameEngine::ColourRGBA8(230, 230, 240, 255);

		m_spriteBatch.draw(rect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_blankTexture.id, 0.0f, bgCol);
		m_spriteFont.draw(m_spriteBatch, labels[i], glm::vec2(0.0f, rect.y + 10.0f),
		                  glm::vec2(0.8f), 0.1f, txtCol, GameEngine::Justification::MIDDLE);
	}

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();
	m_textureProgram.UnUse();
}
#endif
