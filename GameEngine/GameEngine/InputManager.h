#pragma once

#include <unordered_map>

namespace GameEngine
{
	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		void pressKey(unsigned int keyId);
		void releaseKey(unsigned int keyId);

		bool isKeyPressed(unsigned int keyId);
	private:
		std::unordered_map<unsigned int, bool> _keyMap;
	};
}