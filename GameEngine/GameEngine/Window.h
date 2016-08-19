#pragma once
#include <SDL.h>
#include <GL/glew.h>
#include <string>

namespace GameEngine {
	enum WindowFlags
	{
		INVISIBLE = 0x1,
		FULLSCREEN = 0x2,
		BORDERLESS = 0x4
	};

	class Window
	{
	public:
		Window();
		~Window();

		int Create(const std::string&, int, int, unsigned int);

		void Swap();

		int GetScreenWidth() { return m_screenWidth; }
		int GetScreenHeight() { return m_screenHeight; }

	private:
		SDL_Window* m_sdlWindow;
		int m_screenWidth = 0;
		int m_screenHeight = 0;
	};

}