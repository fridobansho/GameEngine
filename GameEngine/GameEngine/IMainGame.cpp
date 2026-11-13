#include "IMainGame.h"
#include "Timing.h"

#include "ScreenList.h"
#include "IGameScreen.h"

namespace GameEngine {

	IMainGame::IMainGame() {
		m_screenList = std::make_unique<ScreenList>(this);
	}

	IMainGame::~IMainGame() {
		// Empty
	}

	void IMainGame::run() {

		if (!init()) return;

		FPSLimiter limiter;
		limiter.setMaxFPS(60.0f);

		// Game loop
		m_isRunning = true;
		while (m_isRunning) {
			limiter.begin();

			inputManager.update();
			// Call the custom update and draw method
			update();
			if (m_isRunning) {
				draw();

				m_fps = limiter.end();
				m_window.Swap();
			}
		}

	}

	void IMainGame::exitGame() {
		m_currentScreen->onExit();
		if (m_screenList) {
			m_screenList->destroy();
			m_screenList.reset();
		}
		m_isRunning = false;
	}

	void IMainGame::onSDLEvent(SDL_Event& evnt) {
		switch (evnt.type) {
			case SDL_QUIT:
				exitGame();
				break;
			case SDL_MOUSEMOTION:
				inputManager.setMouseCoords((float)evnt.motion.x, (float)evnt.motion.y);
				break;
			case SDL_KEYDOWN:
				inputManager.pressKey(evnt.key.keysym.sym);
				break;
			case SDL_KEYUP:
				inputManager.releaseKey(evnt.key.keysym.sym);
				break;
			case SDL_MOUSEBUTTONDOWN:
				inputManager.pressKey(evnt.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				inputManager.releaseKey(evnt.button.button);
				break;
			case SDL_WINDOWEVENT:
				if (evnt.window.event == SDL_WINDOWEVENT_RESIZED ||
					evnt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					m_window.OnResize(evnt.window.data1, evnt.window.data2);
				}
				break;
			case SDL_FINGERDOWN:
				inputManager.onTouchDown(evnt.tfinger.fingerId, evnt.tfinger.x, evnt.tfinger.y,
										 (float)m_window.GetScreenWidth(), (float)m_window.GetScreenHeight());
				break;
			case SDL_FINGERUP:
				inputManager.onTouchUp(evnt.tfinger.fingerId, evnt.tfinger.x, evnt.tfinger.y,
									   (float)m_window.GetScreenWidth(), (float)m_window.GetScreenHeight());
				break;
			case SDL_FINGERMOTION:
				inputManager.onTouchMotion(evnt.tfinger.fingerId, evnt.tfinger.x, evnt.tfinger.y,
										   evnt.tfinger.dx, evnt.tfinger.dy,
										   (float)m_window.GetScreenWidth(), (float)m_window.GetScreenHeight());
				break;
		}
	}

	bool IMainGame::init() {
		GameEngine::Init();

		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		if (!initSystems()) return false;

		onInit();
		addScreens();

		m_currentScreen = m_screenList->getCurrent();
		m_currentScreen->onEntry();
		m_currentScreen->setRunning();

		return true;
	}

	bool IMainGame::initSystems() {
		m_window.Create("Default", 1920, 1080, 0);
		return true;
	}


	void IMainGame::update() {
		if (m_currentScreen) {
			switch (m_currentScreen->getState()) {
				case ScreenState::RUNNING:
					m_currentScreen->update();
					break;
				case ScreenState::CHANGE_NEXT:
					m_currentScreen->onExit();
					m_currentScreen = m_screenList->moveNext();
					if (m_currentScreen) {
						m_currentScreen->setRunning();
						m_currentScreen->onEntry();
					}
					break;
				case ScreenState::CHANGE_PREVIOUS:
					m_currentScreen->onExit();
					m_currentScreen = m_screenList->movePrevious();
					if (m_currentScreen) {
						m_currentScreen->setRunning();
						m_currentScreen->onEntry();
					}
					break;
				case ScreenState::EXIT_APPLICATION:
					exitGame();
					break;
				default:
					break;
			}
		} else {
			exitGame();
		}
	}

	void IMainGame::draw() {
		glViewport(0, 0, m_window.GetScreenWidth(), m_window.GetScreenHeight());
		if (m_currentScreen && m_currentScreen->getState() == ScreenState::RUNNING) {
			m_currentScreen->draw();
		}
	}
}
