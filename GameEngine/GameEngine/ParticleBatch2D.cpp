#include "ParticleBatch2D.h"

namespace GameEngine
{

	ParticleBatch2D::ParticleBatch2D()
	{
	}

	ParticleBatch2D::~ParticleBatch2D()
	{
		delete[] m_particles;
	}

	void ParticleBatch2D::init(int maxParticles, float decayRate, GLTexture texture, std::function<void(Particle2D&, float)> updateFunc)
	{
		m_maxParticles = maxParticles;
		m_particles = new Particle2D[m_maxParticles];
		m_decayRate = decayRate;
		m_texture = texture;
		m_updateFunc = updateFunc;
	}

	void ParticleBatch2D::update(float deltaTime)
	{
		for (int i = 0; i < m_maxParticles; i++)
		{
			if (m_particles[i].life > 0.0f)
			{
				m_updateFunc(m_particles[i], deltaTime);
				m_particles[i].life -= m_decayRate * deltaTime;
			}
		}
	}

	void ParticleBatch2D::draw(SpriteBatch* spriteBatch)
	{
		const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
		for (int i = 0; i < m_maxParticles; i++)
		{
			if (m_particles[i].life > 0.0f)
			{
				auto& p = m_particles[i];
				glm::vec4 destRect(p.position.x, p.position.y, p.width, p.width);
				spriteBatch->draw(destRect, uvRect, m_texture.id, 0.0f, p.colour);
			}
		}
	}

	void ParticleBatch2D::addParticle(const glm::vec2 & position, const glm::vec2 & velocity, const ColourRGBA8 & colour, float width)
	{
		int particleIndex = findFreeParticle();

		auto& p = m_particles[particleIndex];

		p.life = 1.0f;
		p.position = position;
		p.velocity = velocity;
		p.colour = colour;
		p.width = width;
	}

	int ParticleBatch2D::findFreeParticle()
	{
		for (int i = m_lastFreeParticle; i < m_maxParticles; i++)
		{
			if (m_particles[i].life <= 0.0f)
			{
				m_lastFreeParticle = i;
				return i;
			}
		}
		for (int i = 0; i < m_lastFreeParticle; i++)
		{
			if (m_particles[i].life <= 0.0f)
			{
				m_lastFreeParticle = i;
				return i;
			}
		}
		return 0;
	}
}