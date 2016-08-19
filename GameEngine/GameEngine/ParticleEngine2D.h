#pragma once

#include <vector>


namespace GameEngine
{
	class ParticleBatch2D;
	class SpriteBatch;

	class ParticleEngine2D
	{
	public:
		ParticleEngine2D();
		~ParticleEngine2D();

		void addParticleBatch(ParticleBatch2D* particleBatch);

		void update(float deltaTime);

		void draw(SpriteBatch* spriteBatch);
	private:
		std::vector<ParticleBatch2D*> m_batches;
	};

}