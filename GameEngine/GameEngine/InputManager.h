#pragma once

#include <unordered_map>
#include <glm/glm.hpp>

namespace GameEngine
{
	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		void update();

		void pressKey(unsigned int keyId);
		void releaseKey(unsigned int keyId);

		void setMouseCoords(float x, float y);

		bool isKeyDown(unsigned int keyId);

		bool isKeyPressed(unsigned int keyId);

		glm::vec2 getMouseCoords() const { return _mouseCoords; }
	private:
		bool wasKeyDown(unsigned int keyId);
		std::unordered_map<unsigned int, bool> _keyMap;
		std::unordered_map<unsigned int, bool> _previousKeyMap;
		glm::vec2 _mouseCoords;
	};
}