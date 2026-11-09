#pragma once

#include <glm/glm.hpp>
#include <GameEngine/Vertex.h>
#include <GameEngine/SpriteBatch.h>

class Light {
public:
	void draw(GameEngine::SpriteBatch& spriteBatch) {
		glm::vec4 destRect;
		destRect.x = position.x - size / 2.0f;
		destRect.y = position.y - size / 2.0f;
		destRect.z = size;
		destRect.w = size;
		spriteBatch.draw(destRect, glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f), 0, 0.0f, colour, 0.0f);
	}

	GameEngine::ColourRGBA8 colour;
	glm::vec2 position;
	float size;
};
