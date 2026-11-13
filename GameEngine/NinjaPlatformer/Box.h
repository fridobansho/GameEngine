#pragma once

#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <GameEngine/Vertex.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/GLTexture.h>

class Box {
public:
	Box();
	~Box();

	void init(b2World* world,
	          const glm::vec2& position,
	          const glm::vec2& dimensions,
	          GameEngine::GLTexture texture,
	          GameEngine::ColourRGBA8 colour,
	          bool fixedRotation,
	          bool isDynamic,
	          float angle = 0.0f,
	          glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	          bool isGoal = false);
	void destroy(b2World* world);

	void draw(GameEngine::SpriteBatch& spriteBatch);

	bool isDynamic() const { return m_body->GetType() == b2_dynamicBody; }
	bool isGoal() const { return m_isGoal; }
	void setIsGoal(bool isGoal);

	// Test if a point is inside the box
	bool testPoint(float x, float y) const { return m_fixture->TestPoint(b2Vec2(x, y)); }

	// Beautiful accessors
	b2Body*                        getBody()          const { return m_body; }
	b2Fixture*                     getFixture()       const { return m_fixture; }
	const glm::vec2&               getDimensions()    const { return m_dimensions; }
	glm::vec2                      getPosition()      const { return glm::vec2(m_body->GetPosition().x, m_body->GetPosition().y); }
	glm::vec4                      getUvRect()        const { return m_uvRect; }
	const GameEngine::ColourRGBA8& getColour()        const { return m_colour; }
	float                          getAngle()         const { return m_body->GetAngle(); }
	const GameEngine::GLTexture&   getTexture()       const { return m_texture; }
	const bool&                    getFixedRotation() const { return m_fixedRotation; }
	const bool&                    getIsDynamic()     const { return m_isDynamic; }
	const bool&                    getIsGoal()        const { return m_isGoal; }
private:
	glm::vec4 m_uvRect;
	b2Body* m_body = nullptr;
	b2Fixture* m_fixture = nullptr;
	glm::vec2 m_dimensions;
	GameEngine::ColourRGBA8 m_colour;
	GameEngine::GLTexture m_texture;
	bool m_fixedRotation = false;
	bool m_isDynamic = false;
	bool m_isGoal = false;
};
