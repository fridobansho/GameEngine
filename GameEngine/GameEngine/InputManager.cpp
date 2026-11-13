#include "InputManager.h"

#include <SDL.h>
#include <algorithm>
#include <cmath>

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
	m_pinchDelta = 0.0f;
	for (auto& finger : m_fingers)
	{
		finger.delta = glm::vec2(0.0f);
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

void GameEngine::InputManager::onTouchDown(int64_t fingerId, float normX, float normY, float screenWidth, float screenHeight)
{
	glm::vec2 normPos(normX, normY);
	glm::vec2 pixelPos(normX * screenWidth, normY * screenHeight);

	bool found = false;
	for (auto& finger : m_fingers)
	{
		if (finger.id == fingerId)
		{
			finger.normPos = normPos;
			finger.pixelPos = pixelPos;
			finger.delta = glm::vec2(0.0f);
			found = true;
			break;
		}
	}
	if (!found)
	{
		TouchFinger finger;
		finger.id = fingerId;
		finger.normPos = normPos;
		finger.pixelPos = pixelPos;
		finger.delta = glm::vec2(0.0f);
		m_fingers.push_back(finger);
	}

	if (m_fingers.size() == 1)
	{
		setMouseCoords(pixelPos.x, pixelPos.y);
		pressKey(SDL_BUTTON_LEFT);
		m_prevPinchDist = 0.0f;
	}
	else if (m_fingers.size() >= 2)
	{
		m_prevPinchDist = glm::length(m_fingers[0].pixelPos - m_fingers[1].pixelPos);
	}
}

void GameEngine::InputManager::onTouchUp(int64_t fingerId, float normX, float normY, float screenWidth, float screenHeight)
{
	if (m_fingers.size() == 1 && m_fingers[0].id == fingerId)
	{
		setMouseCoords(normX * screenWidth, normY * screenHeight);
	}

	m_fingers.erase(
		std::remove_if(m_fingers.begin(), m_fingers.end(),
			[fingerId](const TouchFinger& f) { return f.id == fingerId; }),
		m_fingers.end());

	if (m_fingers.empty())
	{
		releaseKey(SDL_BUTTON_LEFT);
		m_prevPinchDist = 0.0f;
	}
	else if (m_fingers.size() < 2)
	{
		m_prevPinchDist = 0.0f;
	}
}

void GameEngine::InputManager::onTouchMotion(int64_t fingerId, float normX, float normY, float dx, float dy, float screenWidth, float screenHeight)
{
	glm::vec2 normPos(normX, normY);
	glm::vec2 pixelPos(normX * screenWidth, normY * screenHeight);
	glm::vec2 pixelDelta(dx * screenWidth, dy * screenHeight);

	bool found = false;
	for (auto& finger : m_fingers)
	{
		if (finger.id == fingerId)
		{
			finger.normPos = normPos;
			finger.pixelPos = pixelPos;
			finger.delta += pixelDelta;
			found = true;
			break;
		}
	}
	if (!found)
	{
		TouchFinger finger;
		finger.id = fingerId;
		finger.normPos = normPos;
		finger.pixelPos = pixelPos;
		finger.delta = pixelDelta;
		m_fingers.push_back(finger);
	}

	if (m_fingers.size() == 1)
	{
		setMouseCoords(pixelPos.x, pixelPos.y);
	}
	else if (m_fingers.size() >= 2)
	{
		float currentDist = glm::length(m_fingers[0].pixelPos - m_fingers[1].pixelPos);
		float diag = std::sqrt(screenWidth * screenWidth + screenHeight * screenHeight);
		if (m_prevPinchDist > 0.0f && diag > 0.0f)
		{
			m_pinchDelta += (currentDist - m_prevPinchDist) / diag;
		}
		m_prevPinchDist = currentDist;
	}
}

bool GameEngine::InputManager::isKeyDown(unsigned int keyId)
{
	auto it = m_keyMap.find(keyId);
	if (it != m_keyMap.end()) return it->second;
	return false;
}

bool GameEngine::InputManager::isKeyPressed(unsigned int keyId)
{
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
