#include "VirtualControls.h"

#include <algorithm>
#include <cmath>

namespace GameEngine {

	namespace {
		bool pointInNormRect(float x, float y, const glm::vec4& rect) {
			return x >= rect.x && x <= (rect.x + rect.z) &&
			       y >= rect.y && y <= (rect.y + rect.w);
		}

		glm::vec2 computeStickValue(float normX, float normY, const VirtualStick& stick, int screenWidth, int screenHeight) {
			float w = static_cast<float>(std::max(1, screenWidth));
			float h = static_cast<float>(std::max(1, screenHeight));
			float radiusPx = stick.normRadius * static_cast<float>(std::min(screenWidth > 0 ? screenWidth : 1,
			                                                              screenHeight > 0 ? screenHeight : 1));
			if (radiusPx <= 0.0f) {
				return glm::vec2(0.0f);
			}
			glm::vec2 centerPx(stick.normCenter.x * w, stick.normCenter.y * h);
			glm::vec2 fingerPx(normX * w, normY * h);
			glm::vec2 offset = (fingerPx - centerPx) / radiusPx;
			float len = glm::length(offset);
			if (len > 1.0f) {
				offset /= len;
			}
			return offset;
		}

		bool pointInStick(float normX, float normY, const VirtualStick& stick, int screenWidth, int screenHeight) {
			float w = static_cast<float>(std::max(1, screenWidth));
			float h = static_cast<float>(std::max(1, screenHeight));
			float radiusPx = stick.normRadius * static_cast<float>(std::min(screenWidth > 0 ? screenWidth : 1,
			                                                              screenHeight > 0 ? screenHeight : 1));
			glm::vec2 centerPx(stick.normCenter.x * w, stick.normCenter.y * h);
			glm::vec2 fingerPx(normX * w, normY * h);
			return glm::length(fingerPx - centerPx) <= radiusPx;
		}
	}

	VirtualControls::VirtualControls() = default;

	VirtualControls::~VirtualControls() {
		dispose();
	}

	void VirtualControls::init(VirtualControlLayout layout) {
		setLayout(layout);
	}

	void VirtualControls::dispose() {
		clearActiveState();
		if (m_whiteTexture != 0) {
#ifndef __ANDROID__
			if (glDeleteTextures != nullptr)
#endif
			{
				glDeleteTextures(1, &m_whiteTexture);
			}
			m_whiteTexture = 0;
		}
	}

	void VirtualControls::clearActiveState() {
		for (auto& btn : m_buttons) {
			btn.isPressed = false;
			btn.activeFingerId = -1;
		}
		for (auto& stick : m_sticks) {
			stick.isActive = false;
			stick.activeFingerId = -1;
			stick.value = glm::vec2(0.0f);
		}
		m_isAiming = false;
	}

	void VirtualControls::setLayout(VirtualControlLayout layout) {
		m_layout = layout;
		m_buttons.clear();
		m_sticks.clear();
		m_isAiming = false;

		switch (layout) {
			case VirtualControlLayout::PLATFORMER:
				m_buttons.push_back({ SDLK_a, "<", glm::vec4(0.03f, 0.74f, 0.08f, 0.14f) });
				m_buttons.push_back({ SDLK_d, ">", glm::vec4(0.13f, 0.74f, 0.08f, 0.14f) });
				m_buttons.push_back({ SDLK_w, "JUMP", glm::vec4(0.76f, 0.74f, 0.09f, 0.14f) });
				m_buttons.push_back({ SDLK_SPACE, "PUNCH", glm::vec4(0.87f, 0.74f, 0.09f, 0.14f) });
				m_buttons.push_back({ SDLK_ESCAPE, "MENU", glm::vec4(0.89f, 0.03f, 0.08f, 0.08f) });
				break;

			case VirtualControlLayout::TWIN_STICK:
				m_sticks.push_back({ glm::vec2(0.13f, 0.76f), 0.14f, glm::vec2(0.0f), false, -1, false });
				m_sticks.push_back({ glm::vec2(0.87f, 0.76f), 0.14f, glm::vec2(0.0f), false, -1, true });
				m_buttons.push_back({ SDLK_1, "1", glm::vec4(0.35f, 0.85f, 0.06f, 0.09f) });
				m_buttons.push_back({ SDLK_2, "2", glm::vec4(0.43f, 0.85f, 0.06f, 0.09f) });
				m_buttons.push_back({ SDLK_3, "3", glm::vec4(0.51f, 0.85f, 0.06f, 0.09f) });
				m_buttons.push_back({ SDLK_r, "RST", glm::vec4(0.89f, 0.03f, 0.08f, 0.08f) });
				break;

			case VirtualControlLayout::SANDBOX_CAMERA:
				m_sticks.push_back({ glm::vec2(0.13f, 0.76f), 0.14f, glm::vec2(0.0f), false, -1, false });
				m_buttons.push_back({ SDLK_q, "ZOOM+", glm::vec4(0.86f, 0.70f, 0.10f, 0.10f) });
				m_buttons.push_back({ SDLK_e, "ZOOM-", glm::vec4(0.86f, 0.83f, 0.10f, 0.10f) });
				break;

			case VirtualControlLayout::BALL_SANDBOX:
				m_buttons.push_back({ SDLK_UP, "^", glm::vec4(0.09f, 0.64f, 0.06f, 0.09f) });
				m_buttons.push_back({ SDLK_DOWN, "v", glm::vec4(0.09f, 0.86f, 0.06f, 0.09f) });
				m_buttons.push_back({ SDLK_LEFT, "<", glm::vec4(0.02f, 0.75f, 0.06f, 0.09f) });
				m_buttons.push_back({ SDLK_RIGHT, ">", glm::vec4(0.16f, 0.75f, 0.06f, 0.09f) });
				m_buttons.push_back({ SDLK_SPACE, "0G", glm::vec4(0.09f, 0.75f, 0.06f, 0.09f) });
				m_buttons.push_back({ SDLK_1, "MODE", glm::vec4(0.87f, 0.03f, 0.10f, 0.09f) });
				break;
		}
	}

	bool VirtualControls::onSDLEvent(const SDL_Event& evnt, InputManager& inputManager, int screenWidth, int screenHeight) {
#ifndef __ANDROID__
		if (evnt.type == SDL_KEYDOWN && evnt.key.keysym.sym == SDLK_F1) {
			toggle();
			if (!m_enabled) {
				clearActiveState();
				syncInput(inputManager, screenWidth, screenHeight);
			}
			return true;
		}
		if (evnt.type == SDL_FINGERDOWN && !m_enabled) {
			m_enabled = true;
		}
#endif

		if (!m_enabled) {
			return false;
		}

		if (evnt.type == SDL_FINGERDOWN) {
			int64_t fingerId = evnt.tfinger.fingerId;
			float fx = evnt.tfinger.x;
			float fy = evnt.tfinger.y;

			for (auto& btn : m_buttons) {
				if (pointInNormRect(fx, fy, btn.normRect)) {
					btn.isPressed = true;
					btn.activeFingerId = fingerId;
					syncInput(inputManager, screenWidth, screenHeight);
					return true;
				}
			}

			for (auto& stick : m_sticks) {
				if (!stick.isActive && pointInStick(fx, fy, stick, screenWidth, screenHeight)) {
					stick.isActive = true;
					stick.activeFingerId = fingerId;
					stick.value = computeStickValue(fx, fy, stick, screenWidth, screenHeight);
					syncInput(inputManager, screenWidth, screenHeight);
					return true;
				}
			}
			return false;
		}

		if (evnt.type == SDL_FINGERMOTION) {
			int64_t fingerId = evnt.tfinger.fingerId;
			float fx = evnt.tfinger.x;
			float fy = evnt.tfinger.y;

			for (const auto& btn : m_buttons) {
				if (btn.isPressed && btn.activeFingerId == fingerId) {
					return true;
				}
			}

			for (auto& stick : m_sticks) {
				if (stick.isActive && stick.activeFingerId == fingerId) {
					stick.value = computeStickValue(fx, fy, stick, screenWidth, screenHeight);
					syncInput(inputManager, screenWidth, screenHeight);
					return true;
				}
			}
			return false;
		}

		if (evnt.type == SDL_FINGERUP) {
			int64_t fingerId = evnt.tfinger.fingerId;

			for (auto& btn : m_buttons) {
				if (btn.isPressed && btn.activeFingerId == fingerId) {
					btn.isPressed = false;
					btn.activeFingerId = -1;
					syncInput(inputManager, screenWidth, screenHeight);
					return true;
				}
			}

			for (auto& stick : m_sticks) {
				if (stick.isActive && stick.activeFingerId == fingerId) {
					stick.isActive = false;
					stick.activeFingerId = -1;
					stick.value = glm::vec2(0.0f);
					syncInput(inputManager, screenWidth, screenHeight);
					return true;
				}
			}
			return false;
		}

		return false;
	}

	void VirtualControls::update(InputManager& inputManager, int screenWidth, int screenHeight) {
		syncInput(inputManager, screenWidth, screenHeight);
	}

	void VirtualControls::syncInput(InputManager& inputManager, int screenWidth, int screenHeight) {
		std::unordered_set<unsigned int> desiredKeys;
		bool aimingNow = false;

		if (m_enabled) {
			for (const auto& btn : m_buttons) {
				if (btn.isPressed) {
					desiredKeys.insert(btn.keyCode);
				}
			}

			for (const auto& stick : m_sticks) {
				if (!stick.isActive) {
					continue;
				}
				if (!stick.isAimStick) {
					const float deadzone = 0.25f;
					if (stick.value.x < -deadzone) desiredKeys.insert(SDLK_a);
					if (stick.value.x > deadzone)  desiredKeys.insert(SDLK_d);
					if (stick.value.y < -deadzone) desiredKeys.insert(SDLK_w);
					if (stick.value.y > deadzone)  desiredKeys.insert(SDLK_s);
				} else {
					glm::vec2 worldDir(stick.value.x, -stick.value.y);
					float len = glm::length(worldDir);
					if (len > 0.15f) {
						m_aimDirection = worldDir / len;
						m_isAiming = true;
						aimingNow = true;
						float aimRadius = 200.0f;
						float w = static_cast<float>(std::max(1, screenWidth));
						float h = static_cast<float>(std::max(1, screenHeight));
						inputManager.setMouseCoords(
							w * 0.5f + stick.value.x * aimRadius,
							h * 0.5f + stick.value.y * aimRadius
						);
						desiredKeys.insert(SDL_BUTTON_LEFT);
					}
				}
			}
		}

		if (!aimingNow) {
			m_isAiming = false;
		}

		for (unsigned int held : m_heldKeys) {
			if (desiredKeys.find(held) == desiredKeys.end()) {
				inputManager.releaseKey(held);
			}
		}

		for (unsigned int desired : desiredKeys) {
			if (m_heldKeys.find(desired) == m_heldKeys.end()) {
				inputManager.pressKey(desired);
			}
		}

		m_heldKeys = std::move(desiredKeys);
	}

	void VirtualControls::draw(SpriteBatch& spriteBatch, int screenWidth, int screenHeight, SpriteFont* font) {
		if (!m_enabled || screenWidth <= 0 || screenHeight <= 0) {
			return;
		}

		if (m_whiteTexture == 0) {
			glGenTextures(1, &m_whiteTexture);
			glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
			unsigned char whitePixel[4] = { 255, 255, 255, 255 };
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
		float sw = static_cast<float>(screenWidth);
		float sh = static_cast<float>(screenHeight);
		float minDim = static_cast<float>(std::min(screenWidth, screenHeight));

		// Draw virtual sticks
		for (const auto& stick : m_sticks) {
			float radiusPx = stick.normRadius * minDim;
			float centerX = stick.normCenter.x * sw;
			float centerY = (1.0f - stick.normCenter.y) * sh;

			glm::vec4 baseRect(centerX - radiusPx, centerY - radiusPx, radiusPx * 2.0f, radiusPx * 2.0f);
			ColourRGBA8 baseCol = stick.isAimStick
				? ColourRGBA8(220, 120, 120, 65)
				: ColourRGBA8(200, 200, 200, 65);
			spriteBatch.draw(baseRect, uvRect, m_whiteTexture, 0.5f, baseCol);

			float knobRadius = radiusPx * 0.4f;
			float knobX = centerX + stick.value.x * (radiusPx * 0.6f);
			float knobY = centerY - stick.value.y * (radiusPx * 0.6f);
			glm::vec4 knobRect(knobX - knobRadius, knobY - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);
			ColourRGBA8 knobCol = stick.isActive
				? ColourRGBA8(255, 255, 255, 190)
				: ColourRGBA8(230, 230, 230, 115);
			spriteBatch.draw(knobRect, uvRect, m_whiteTexture, 0.4f, knobCol);
		}

		// Draw virtual buttons
		for (const auto& btn : m_buttons) {
			float bx = btn.normRect.x * sw;
			float bw = btn.normRect.z * sw;
			float bh = btn.normRect.w * sh;
			float by = (1.0f - (btn.normRect.y + btn.normRect.w)) * sh;

			glm::vec4 destRect(bx, by, bw, bh);
			ColourRGBA8 bgCol = btn.isPressed
				? ColourRGBA8(255, 255, 255, 180)
				: ColourRGBA8(200, 200, 200, 80);
			spriteBatch.draw(destRect, uvRect, m_whiteTexture, 0.5f, bgCol);

			if (font != nullptr && !btn.label.empty()) {
				glm::vec2 textSize = font->measure(btn.label.c_str());
				float fontH = static_cast<float>(std::max(1, font->getFontHeight()));
				float scaleX = (bw * 0.75f) / std::max(textSize.x, 1.0f);
				float scaleY = (bh * 0.55f) / fontH;
				float scale = std::min(std::min(scaleX, scaleY), 0.7f);
				glm::vec2 textPos(bx + bw * 0.5f, by + (bh - textSize.y * scale) * 0.5f);
				font->draw(spriteBatch, btn.label.c_str(), textPos, glm::vec2(scale), 0.0f,
				           ColourRGBA8(255, 255, 255, 230), Justification::MIDDLE);
			}
		}
	}

}
