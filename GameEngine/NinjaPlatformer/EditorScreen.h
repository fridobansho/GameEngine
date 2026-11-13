#pragma once

#include "Box.h"
#include "Light.h"
#include "Player.h"
#include <GameEngine/Camera2D.h>
#include <GameEngine/DebugRenderer.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/GLTexture.h>
#include <GameEngine/IGameScreen.h>
#include <GameEngine/InputManager.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/SoA/SpriteFont.h>
#include <GameEngine/Window.h>
#include <GameEngine/GUI.h>
#include <vector>
#include <string>
#include <cstddef>
#include <memory>

#include "ScreenIndices.h"

enum class PhysicsMode {
	RIGID,
	DYNAMIC
};

enum class ObjectMode {
	PLAYER,
	PLATFORM,
	LIGHT,
	FINISH
};

enum class SelectionMode {
	SELECT,
	PLACE
};

const int NO_BOX = -1;
const int NO_LIGHT = -1;

#ifdef GAMEENGINE_HAS_CEGUI
class WidgetLabel {
public:
	WidgetLabel() {}
	WidgetLabel(CEGUI::Window* w, const std::string& text, float scale) :
		widget(w), text(text), scale(scale) {
		// Empty
	}
	void draw(GameEngine::SpriteBatch& sb, GameEngine::SpriteFont& sf, GameEngine::Window* w);

	CEGUI::Window* widget = nullptr;
	std::string text = "";
	GameEngine::ColourRGBA8 colour = GameEngine::ColourRGBA8(255, 255, 255, 255);
	float scale = 0.7f;
};
#endif

class EditorScreen : public GameEngine::IGameScreen {
public:
	EditorScreen(GameEngine::Window* window);
	~EditorScreen();

	/************************************************************************/
	/* IGameScreen Implementation                                           */
	/************************************************************************/
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void destroy() override;
	virtual void onEntry() override;
	virtual void onExit() override;
	virtual void update() override;
	virtual void draw() override;

private:
	void drawUI();
	void drawWorld();

	void clearLevel();

	void initUI();
	void checkInput();
	void updateMouseDown(const SDL_Event& evnt);
	void updateMouseUp(const SDL_Event& evnt);
	void updateMouseMotion(const SDL_Event& evnt);
	void refreshSelectedBox();
	void refreshSelectedBox(const glm::vec2& newPosition);
	void refreshSelectedLight();
	void refreshSelectedLight(const glm::vec2& newPosition);
	bool isMouseInUI();

	void setPlatformWidgetVisibility(bool visible);
	void setLightWidgetVisibility(bool visible);

#ifdef GAMEENGINE_HAS_CEGUI
	/************************************************************************/
	/* Event Handlers                                                       */
	/************************************************************************/
	bool onExitClicked(const CEGUI::EventArgs& e);
	bool onColorPickerRedChanged(const CEGUI::EventArgs& e);
	bool onColorPickerGreenChanged(const CEGUI::EventArgs& e);
	bool onColorPickerBlueChanged(const CEGUI::EventArgs& e);
	bool onColorPickerAlphaChanged(const CEGUI::EventArgs& e);
	bool onRigidMouseClick(const CEGUI::EventArgs& e);
	bool onDynamicMouseClick(const CEGUI::EventArgs& e);
	bool onPlayerMouseClick(const CEGUI::EventArgs& e);
	bool onPlatformMouseClick(const CEGUI::EventArgs& e);
	bool onFinishMouseClick(const CEGUI::EventArgs& e);
	bool onLightMouseClick(const CEGUI::EventArgs& e);
	bool onSelectMouseClick(const CEGUI::EventArgs& e);
	bool onPlaceMouseClick(const CEGUI::EventArgs& e);
	bool onSaveMouseClick(const CEGUI::EventArgs& e);
	bool onLoadMouseClick(const CEGUI::EventArgs& e);
	bool onBackMouseClick(const CEGUI::EventArgs& e);
	bool onRotationValueChange(const CEGUI::EventArgs& e);
	bool onSizeValueChange(const CEGUI::EventArgs& e);
	bool onWidthValueChange(const CEGUI::EventArgs& e);
	bool onHeightValueChange(const CEGUI::EventArgs& e);
	bool onDebugToggleClick(const CEGUI::EventArgs& e);
	bool onSaveCancelClick(const CEGUI::EventArgs& e);
	bool onSave(const CEGUI::EventArgs& e);
	bool onLoadCancelClick(const CEGUI::EventArgs& e);
	bool onLoad(const CEGUI::EventArgs& e);
#else
	enum class ToolbarAction {
		SELECT_MODE,
		PLACE_MODE,
		OBJECT_PLAYER,
		OBJECT_PLATFORM,
		OBJECT_FINISH,
		OBJECT_LIGHT,
		TOGGLE_PHYSICS,
		TOGGLE_DEBUG,
		SAVE_LEVEL,
		LOAD_LEVEL,
		BACK
	};
	struct FallbackButton {
		ToolbarAction action;
		std::string label;
		glm::vec4 rect; // UI camera coordinates
	};
	std::vector<FallbackButton> buildFallbackToolbar() const;
	bool handleFallbackToolbarClick(float screenX, float screenY);
	void drawFallbackToolbar();
#endif

	/************************************************************************/
	/* Member Variables                                                     */
	/************************************************************************/
	float m_colorPickerRed = 255.0f;
	float m_colorPickerGreen = 255.0f;
	float m_colorPickerBlue = 255.0f;
	float m_colorPickerAlpha = 255.0f;
	float m_rotation = 0.0f;
	float m_lightSize = 10.0f;
	glm::vec2 m_boxDims = glm::vec2(1.0f);
	bool m_mouseButtons[2] = { false, false };
	bool m_debugRender = false;

#ifdef GAMEENGINE_HAS_CEGUI
	CEGUI::GroupBox* m_groupBox = nullptr;
	CEGUI::Slider* m_rSlider = nullptr;
	CEGUI::Slider* m_gSlider = nullptr;
	CEGUI::Slider* m_bSlider = nullptr;
	CEGUI::Slider* m_aSlider = nullptr;
	CEGUI::RadioButton* m_rigidRadioButton = nullptr;
	CEGUI::RadioButton* m_dynamicRadioButton = nullptr;
	CEGUI::RadioButton* m_playerRadioButton = nullptr;
	CEGUI::RadioButton* m_platformRadioButton = nullptr;
	CEGUI::RadioButton* m_finishRadioButton = nullptr;
	CEGUI::RadioButton* m_lightRadioButton = nullptr;
	CEGUI::RadioButton* m_selectRadioButton = nullptr;
	CEGUI::RadioButton* m_placeRadioButton = nullptr;
	CEGUI::ToggleButton* m_debugToggle = nullptr;
	CEGUI::Spinner* m_rotationSpinner = nullptr;
	CEGUI::Spinner* m_widthSpinner = nullptr;
	CEGUI::Spinner* m_heightSpinner = nullptr;
	CEGUI::Spinner* m_sizeSpinner = nullptr;
	CEGUI::FrameWindow* m_saveWindow = nullptr;
	CEGUI::PushButton* m_saveWindowSaveButton = nullptr;
	CEGUI::Combobox* m_saveWindowCombobox = nullptr;
	CEGUI::FrameWindow* m_loadWindow = nullptr;
	CEGUI::PushButton* m_loadWindowLoadButton = nullptr;
	CEGUI::Combobox* m_loadWindowCombobox = nullptr;
	std::vector<CEGUI::ListboxTextItem*> m_saveListBoxItems;
	std::vector<CEGUI::ListboxTextItem*> m_loadListBoxItems;
	CEGUI::PushButton* m_saveButton = nullptr;
	CEGUI::PushButton* m_loadButton = nullptr;
	CEGUI::PushButton* m_backButton = nullptr;
	std::vector<WidgetLabel> m_widgetLabels;
#else
	std::string m_statusMessage;
#endif

	GameEngine::SpriteBatch m_spriteBatch;
	GameEngine::SpriteFont m_spriteFont;
	GameEngine::GLSLProgram m_textureProgram;
	GameEngine::GLSLProgram m_lightProgram;
	GameEngine::DebugRenderer m_debugRenderer;

	GameEngine::GLTexture m_blankTexture;

	PhysicsMode m_physicsMode = PhysicsMode::RIGID;
	ObjectMode m_objectMode = ObjectMode::PLAYER;
	SelectionMode m_selectMode = SelectionMode::SELECT;

	GameEngine::InputManager m_inputManager;

	bool m_hasPlayer = false;
	Player m_player;
	std::vector<Box> m_boxes;
	std::vector<Light> m_lights;
	int m_selectedBox = NO_BOX;
	int m_selectedLight = NO_LIGHT;
	bool m_isDragging = false;
	glm::vec2 m_selectOffset = glm::vec2(0.0f);

	GameEngine::Camera2D m_camera;
	GameEngine::Camera2D m_uiCamera;
	GameEngine::Window* m_window;
	GameEngine::GUI m_gui;
	std::unique_ptr<b2World> m_world;
};
