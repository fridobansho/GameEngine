#pragma once

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <glm/glm.hpp>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

#include "InputManager.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"

namespace GameEngine {

	enum class VirtualControlLayout {
		PLATFORMER,
		TWIN_STICK,
		SANDBOX_CAMERA,
		BALL_SANDBOX
	};

	struct VirtualButton {
		unsigned int keyCode = 0;
		std::string label;
		// Normalized [0, 1] top-left screen coordinates: (x, y, width, height)
		glm::vec4 normRect = glm::vec4(0.0f);
		bool isPressed = false;
		int64_t activeFingerId = -1;
	};

	struct VirtualStick {
		// Center in normalized [0, 1] top-left screen coordinates
		glm::vec2 normCenter = glm::vec2(0.0f);
		// Radius as a fraction of min(screenWidth, screenHeight)
		float normRadius = 0.14f;
		// Current stick deflection in [-1, 1] (top-left screen axes: +x right, +y down)
		glm::vec2 value = glm::vec2(0.0f);
		bool isActive = false;
		int64_t activeFingerId = -1;
		bool isAimStick = false;
	};

	class VirtualControls {
	public:
		VirtualControls();
		~VirtualControls();

		void init(VirtualControlLayout layout = VirtualControlLayout::PLATFORMER);
		void dispose();

		void setLayout(VirtualControlLayout layout);
		VirtualControlLayout getLayout() const { return m_layout; }

		void setEnabled(bool enabled) { m_enabled = enabled; }
		bool isEnabled() const { return m_enabled; }
		void toggle() { m_enabled = !m_enabled; }

		// Returns true if the event was consumed by a virtual control (or F1 toggle).
		bool onSDLEvent(const SDL_Event& evnt, InputManager& inputManager, int screenWidth, int screenHeight);

		// Syncs virtual stick/button state with InputManager.
		void update(InputManager& inputManager, int screenWidth, int screenHeight);

		// Appends virtual control visuals into an active SpriteBatch (between begin() and end())
		// configured with a bottom-left origin HUD projection of size (screenWidth, screenHeight).
		void draw(SpriteBatch& spriteBatch, int screenWidth, int screenHeight, SpriteFont* font = nullptr);

		glm::vec2 getAimDirection() const { return m_aimDirection; }
		bool isAiming() const { return m_isAiming; }

		const std::vector<VirtualButton>& getButtons() const { return m_buttons; }
		const std::vector<VirtualStick>& getSticks() const { return m_sticks; }

	private:
		void syncInput(InputManager& inputManager, int screenWidth, int screenHeight);
		void clearActiveState();

		VirtualControlLayout m_layout = VirtualControlLayout::PLATFORMER;
#ifdef __ANDROID__
		bool m_enabled = true;
#else
		bool m_enabled = false;
#endif
		std::vector<VirtualButton> m_buttons;
		std::vector<VirtualStick> m_sticks;
		std::unordered_set<unsigned int> m_heldKeys;

		glm::vec2 m_aimDirection = glm::vec2(1.0f, 0.0f);
		bool m_isAiming = false;
		GLuint m_whiteTexture = 0;
	};

}
