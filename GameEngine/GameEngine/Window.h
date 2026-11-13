#pragma once
#include <SDL.h>
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <string>

namespace GameEngine {
	enum WindowFlags
	{
		INVISIBLE = 0x1,
		FULLSCREEN = 0x2,
		BORDERLESS = 0x4,
		RESIZABLE = 0x8
	};

	class Window
	{
	public:
		Window();
		~Window();

		int Create(const std::string&, int, int, unsigned int);

		void Swap();

		void OnResize(int newWidth, int newHeight);

		int GetScreenWidth() const { return m_screenWidth; }
		int GetScreenHeight() const { return m_screenHeight; }

	private:
		SDL_Window* m_sdlWindow = nullptr;
		int m_screenWidth = 0;
		int m_screenHeight = 0;
	};

}