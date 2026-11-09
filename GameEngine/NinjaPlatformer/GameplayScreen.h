#pragma once

#include "Box.h"
#include "Player.h"
#include <GameEngine/IGameScreen.h>
#include <Box2D/Box2D.h>
#include <vector>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/GLTexture.h>
#include <GameEngine/Window.h>
#include <GameEngine/DebugRenderer.h>

#include <GameEngine/GUI.h>

// Our custom gameplay screen that inherits from the IGameScreen
class GameplayScreen : public GameEngine::IGameScreen {
public:
	GameplayScreen(GameEngine::Window* window);
	~GameplayScreen();

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

	bool onExitClicked(const CEGUI::EventArgs& e);

	GameEngine::SpriteBatch m_spriteBatch;
	GameEngine::GLSLProgram m_textureProgram;
	GameEngine::GLSLProgram m_lightProgram;
	GameEngine::Camera2D m_camera;
	GameEngine::GLTexture m_texture;
	GameEngine::Window* m_window;
	GameEngine::DebugRenderer m_debugRenderer;
	GameEngine::GUI m_gui;

	bool m_renderDebug = false;

	Player m_player;
	std::vector<Box> m_boxes;
	std::unique_ptr<b2World> m_world;
};
