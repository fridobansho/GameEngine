#pragma once

namespace GameEngine
{
	class FPSLimiter
	{
	public:
		FPSLimiter();

		void init(float maxFPS);

		void setMaxFPS(float maxFPS);

		void begin();

		float end();
	private:
		void calculateFPS();

		float m_fps = 0.0f;
		float m_maxFPS = 60.0f;
		float m_frameTime = 0.0f;
		unsigned int m_startTicks = 0;
	};
}