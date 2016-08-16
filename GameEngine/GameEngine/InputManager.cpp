#include "InputManager.h"

using namespace GameEngine;

InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}

void GameEngine::InputManager::pressKey(unsigned int keyId)
{
	_keyMap[keyId] = true;
}

void GameEngine::InputManager::releaseKey(unsigned int keyId)
{
	_keyMap[keyId] = false;
}

bool GameEngine::InputManager::isKeyPressed(unsigned int keyId)
{
	auto it = _keyMap.find(keyId);
	if (it != _keyMap.end()) return it->second;
	return false;
}
