#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

namespace GameEngine
{
	struct TouchFinger
	{
		int64_t id = 0;
		glm::vec2 normPos = glm::vec2(0.0f);
		glm::vec2 pixelPos = glm::vec2(0.0f);
		glm::vec2 delta = glm::vec2(0.0f);
	};

	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		void update();

		void pressKey(unsigned int keyId);
		void releaseKey(unsigned int keyId);

		void setMouseCoords(float x, float y);

		void onTouchDown(int64_t fingerId, float normX, float normY, float screenWidth, float screenHeight);
		void onTouchUp(int64_t fingerId, float normX, float normY, float screenWidth, float screenHeight);
		void onTouchMotion(int64_t fingerId, float normX, float normY, float dx, float dy, float screenWidth, float screenHeight);

		bool isKeyDown(unsigned int keyId);

		bool isKeyPressed(unsigned int keyId);

		glm::vec2 getMouseCoords() const { return m_mouseCoords; }

		bool isTouchActive() const { return !m_fingers.empty(); }
		const std::vector<TouchFinger>& getActiveFingers() const { return m_fingers; }
		float getPinchDelta() const { return m_pinchDelta; }
	private:
		bool wasKeyDown(unsigned int keyId);

		std::unordered_map<unsigned int, bool> m_keyMap;
		std::unordered_map<unsigned int, bool> m_previousKeyMap;
		glm::vec2 m_mouseCoords = glm::vec2(0.0f);
		std::vector<TouchFinger> m_fingers;
		float m_pinchDelta = 0.0f;
		float m_prevPinchDist = 0.0f;
	};
}