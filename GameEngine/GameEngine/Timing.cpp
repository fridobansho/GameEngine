#include "Timing.h"

#include <SDL.h>

namespace GameEngine
{
	FPSLimiter::FPSLimiter()
	{
	}

	void FPSLimiter::init(float maxFPS)
	{
		setMaxFPS(maxFPS);
	}

	void FPSLimiter::setMaxFPS(float maxFPS)
	{
		m_maxFPS = maxFPS;
	}

	void FPSLimiter::begin()
	{
		m_startTicks = SDL_GetTicks();
	}

	float FPSLimiter::end()
	{
		calculateFPS();

		Uint32 frameTicks = SDL_GetTicks() - m_startTicks;

		if ((1000.0f / m_maxFPS) > frameTicks)
		{
			SDL_Delay((Uint32)(1000.0f / m_maxFPS) - frameTicks);
		}
		return m_fps;
	}
	void FPSLimiter::calculateFPS()
	{
		static const int NUM_SAMPLES = 10;
		static Uint32 frameTimes[NUM_SAMPLES];
		static int currentFrame = 0;

		static Uint32 prevTicks = SDL_GetTicks();
		Uint32 currentTicks = SDL_GetTicks();

		m_frameTime = currentTicks - prevTicks;
		frameTimes[currentFrame % NUM_SAMPLES] = m_frameTime;

		prevTicks = currentTicks;

		int count;

		currentFrame++;

		if (currentFrame < NUM_SAMPLES)
		{
			count = currentFrame;
		}
		else
		{
			count = NUM_SAMPLES;
		}

		float frameTimeAverage = 0.0f;

		for (int i = 0; i < count; i++)
		{
			frameTimeAverage += frameTimes[i];
		}
		frameTimeAverage /= count;

		if (frameTimeAverage > 0)
		{
			m_fps = 1000.0f / frameTimeAverage;
		}
		else
		{
			m_fps = 60.0f;
		}
	}
}