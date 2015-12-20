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

		int GetScreenWidth() { return _screenWidth; }
		int GetScreenHeight() { return _screenHeight; }

	private:
		SDL_Window* _sdlWindow;
		int _screenWidth;
		int _screenHeight;
	};

}