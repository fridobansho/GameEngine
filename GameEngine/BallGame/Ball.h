#pragma once

#include <glm/glm.hpp>
#include <GameEngine/Vertex.h>

struct Cell;

// POD
struct Ball {
    Ball(float radius, float mass, const glm::vec2& pos,
         const glm::vec2& vel, unsigned int textureId,
         const GameEngine::ColourRGBA8& colour);

    float radius;
    float mass;
    glm::vec2 velocity;
    glm::vec2 position;
    unsigned int textureId = 0;
    GameEngine::ColourRGBA8 colour;
	Cell* ownerCell = nullptr;
	int cellVectorIndex = -1;
};
