#include "InputManager.h"

using namespace GameEngine;

InputManager::InputManager() : _mouseCoords(0.0f)
{
}


InputManager::~InputManager()
{
}

void GameEngine::InputManager::update()
{
	for (auto& it : _keyMap)
	{
		_previousKeyMap[it.first] = it.second;
	}
}

void GameEngine::InputManager::pressKey(unsigned int keyId)
{
	_keyMap[keyId] = true;
}

void GameEngine::InputManager::releaseKey(unsigned int keyId)
{
	_keyMap[keyId] = false;
}

void GameEngine::InputManager::setMouseCoords(float x, float y)
{
	_mouseCoords.x = x;
	_mouseCoords.y = y;
}

bool GameEngine::InputManager::isKeyDown(unsigned int keyId)
{
	auto it = _keyMap.find(keyId);
	if (it != _keyMap.end()) return it->second;
	return false;
}

bool GameEngine::InputManager::isKeyPressed(unsigned int keyId)
{
	bool isPressed;
	if (isKeyDown(keyId) == true && wasKeyDown(keyId) == false)
		return true;
	return false;
}

bool GameEngine::InputManager::wasKeyDown(unsigned int keyId)
{
	auto it = _previousKeyMap.find(keyId);
	if (it != _previousKeyMap.end()) return it->second;
	return false;
}
