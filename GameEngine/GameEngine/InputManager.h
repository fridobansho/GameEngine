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

		void pressKey(unsigned int keyId);
		void releaseKey(unsigned int keyId);

		void setMouseCoords(float x, float y);

		bool isKeyPressed(unsigned int keyId);

		glm::vec2 getMouseCoords() const { return _mouseCoords; }
	private:
		std::unordered_map<unsigned int, bool> _keyMap;
		glm::vec2 _mouseCoords;
	};
}