#include "Box.h"


Box::Box() {
}


Box::~Box() {
}

void Box::init(b2World* world,
               const glm::vec2& position,
               const glm::vec2& dimensions,
               GameEngine::GLTexture texture,
               GameEngine::ColourRGBA8 colour,
               bool fixedRotation,
               bool isDynamic,
               float angle, /* = 0.0f */
               glm::vec4 uvRect, /*= glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)*/
               bool isGoal /* = false */) {
	m_dimensions = dimensions;
	m_colour = colour;
	m_texture = texture;
	m_uvRect = uvRect;
	m_fixedRotation = fixedRotation;
	m_isDynamic = isDynamic;
	m_isGoal = isGoal;

	// Make the body
	b2BodyDef bodyDef;
	if (isDynamic) {
		bodyDef.type = b2_dynamicBody;
	} else {
		bodyDef.type = b2_staticBody;
	}
	bodyDef.position.Set(position.x, position.y);
	bodyDef.fixedRotation = fixedRotation;
	bodyDef.angle = angle;
	m_body = world->CreateBody(&bodyDef);

	b2PolygonShape boxShape;
	boxShape.SetAsBox(dimensions.x / 2.0f, dimensions.y / 2.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &boxShape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.isSensor = isGoal;
	m_fixture = m_body->CreateFixture(&fixtureDef);
}

void Box::setIsGoal(bool isGoal) {
	m_isGoal = isGoal;
	if (m_fixture) {
		m_fixture->SetSensor(isGoal);
	}
}

void Box::destroy(b2World* world) {
	if (m_body) {
		world->DestroyBody(m_body);
		m_body = nullptr;
		m_fixture = nullptr;
	}
}

void Box::draw(GameEngine::SpriteBatch& spriteBatch) {
	glm::vec4 destRect;
	destRect.x = m_body->GetPosition().x - m_dimensions.x / 2.0f;
	destRect.y = m_body->GetPosition().y - m_dimensions.y / 2.0f;
	destRect.z = m_dimensions.x;
	destRect.w = m_dimensions.y;
	spriteBatch.draw(destRect, m_uvRect, m_texture.id, 0.0f, m_colour, m_body->GetAngle());
}
