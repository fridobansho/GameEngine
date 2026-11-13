#pragma once

#include "Box.h"
#include "Light.h"
#include "Player.h"
#include <GameEngine/IGameScreen.h>
#include <Box2D/Box2D.h>
#include <memory>
#include <vector>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/SoA/SpriteFont.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/GLTexture.h>
#include <GameEngine/Window.h>
#include <GameEngine/DebugRenderer.h>
#include <GameEngine/GUI.h>
#include <GameEngine/VirtualControls.h>

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
	glm::vec4 getMenuButtonRect() const;

#ifdef GAMEENGINE_HAS_CEGUI
	bool onExitClicked(const CEGUI::EventArgs& e);
#endif

	GameEngine::SpriteBatch m_spriteBatch;
	GameEngine::SpriteFont m_spriteFont;
	GameEngine::GLSLProgram m_textureProgram;
	GameEngine::GLSLProgram m_lightProgram;
	GameEngine::Camera2D m_camera;
	GameEngine::Camera2D m_hudCamera;
	GameEngine::GLTexture m_texture;
	GameEngine::GLTexture m_blankTexture;
	GameEngine::Window* m_window;
	GameEngine::DebugRenderer m_debugRenderer;
	GameEngine::GUI m_gui;
	GameEngine::VirtualControls m_virtualControls;

	bool m_renderDebug = false;
	bool m_levelComplete = false;

	Player m_player;
	std::vector<Box> m_boxes;
	std::vector<Light> m_lights;
	std::unique_ptr<b2World> m_world;
};
