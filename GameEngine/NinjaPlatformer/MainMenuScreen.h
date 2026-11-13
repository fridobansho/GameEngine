#pragma once

#include <GameEngine/IGameScreen.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/SoA/SpriteFont.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/GLTexture.h>
#include <GameEngine/Window.h>
#include <GameEngine/GUI.h>
#include <SDL_events.h>

#include "ScreenIndices.h"

class MainMenuScreen : public GameEngine::IGameScreen {
public:
	MainMenuScreen(GameEngine::Window* window);
	~MainMenuScreen();

	virtual int getNextScreenIndex() const override;

	virtual int getPreviousScreenIndex() const override;

	virtual void build() override;

	virtual void destroy() override;

	virtual void onEntry() override;

	virtual void onExit() override;

	virtual void update() override;

	virtual void draw() override;

private:
	void initUI();
	void checkInput();
	void activateMenuItem(int index);

#ifdef GAMEENGINE_HAS_CEGUI
	bool onNewGameClicked(const CEGUI::EventArgs& e);
	bool onEditorClicked(const CEGUI::EventArgs& e);
	bool onExitClicked(const CEGUI::EventArgs& e);
#else
	void drawFallbackMenu();
	int hitTestMenu(float screenX, float screenY);
	glm::vec4 getMenuButtonRect(int index) const;
#endif

	int m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
	GameEngine::Camera2D m_camera;
	GameEngine::Window* m_window;
	GameEngine::GUI m_gui;

#ifndef GAMEENGINE_HAS_CEGUI
	GameEngine::Camera2D m_uiCamera;
	GameEngine::SpriteBatch m_spriteBatch;
	GameEngine::SpriteFont m_spriteFont;
	GameEngine::GLSLProgram m_textureProgram;
	GameEngine::GLTexture m_blankTexture;
	int m_selectedIndex = 0;
#endif
};
