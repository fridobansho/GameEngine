#pragma once

#include "Player.h"
#include <GameEngine/IGameScreen.h>
#include <vector>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/GLTexture.h>
#include <GameEngine/Window.h>
#include <GameEngine/DebugRenderer.h>

#include <GameEngine/GUI.h>

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

	bool onNewGameClicked(const CEGUI::EventArgs& e);
	bool onEditorClicked(const CEGUI::EventArgs& e);
	bool onExitClicked(const CEGUI::EventArgs& e);

	int m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
	GameEngine::Camera2D m_camera;
	GameEngine::Window* m_window;
	GameEngine::GUI m_gui;
};
