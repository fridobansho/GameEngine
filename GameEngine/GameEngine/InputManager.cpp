#include "InputManager.h"

using namespace GameEngine;

InputManager::InputManager() : m_mouseCoords(0.0f)
{
}


InputManager::~InputManager()
{
}

void GameEngine::InputManager::update()
{
	for (auto& it : m_keyMap)
	{
		m_previousKeyMap[it.first] = it.second;
	}
}

void GameEngine::InputManager::pressKey(unsigned int keyId)
{
	m_keyMap[keyId] = true;
}

void GameEngine::InputManager::releaseKey(unsigned int keyId)
{
	m_keyMap[keyId] = false;
}

void GameEngine::InputManager::setMouseCoords(float x, float y)
{
	m_mouseCoords.x = x;
	m_mouseCoords.y = y;
}

bool GameEngine::InputManager::isKeyDown(unsigned int keyId)
{
	auto it = m_keyMap.find(keyId);
	if (it != m_keyMap.end()) return it->second;
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
	auto it = m_previousKeyMap.find(keyId);
	if (it != m_previousKeyMap.end()) return it->second;
	return false;
}
