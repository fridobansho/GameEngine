#include "GameEngineErrors.h"

#include "Window.h"

#include <algorithm>
#include <cstdio>


namespace GameEngine {

	Window::Window()
	{
	}

	Window::~Window()
	{
	}

	int Window::Create(const std::string & windowName, int screenWidth, int screenHeight, unsigned int currentFlags)
	{
		Uint32 flags = SDL_WINDOW_OPENGL;

		m_screenWidth = screenWidth;
		m_screenHeight = screenHeight;

		if (currentFlags & INVISIBLE)
		{
			flags |= SDL_WINDOW_HIDDEN;
		}
		else
		{
			flags |= SDL_WINDOW_RESIZABLE;
			SDL_DisplayMode dm;
			if (SDL_GetCurrentDisplayMode(0, &dm) == 0 && dm.w > 0 && dm.h > 0)
			{
				m_screenWidth = std::min(m_screenWidth, dm.w);
				m_screenHeight = std::min(m_screenHeight, dm.h);
			}
		}

		if (currentFlags & FULLSCREEN)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		if (currentFlags & BORDERLESS)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if (currentFlags & RESIZABLE)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

#ifdef __ANDROID__
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

		m_sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_screenWidth, m_screenHeight, flags);

		if (m_sdlWindow == nullptr)
		{
			FatalError("SDL Window could not be created.");
		}

		SDL_GLContext glContext = SDL_GL_CreateContext(m_sdlWindow);

		if (glContext == nullptr)
		{
			FatalError("SDL GL Context could not be created.");
		}

#ifndef __ANDROID__
		GLenum error = glewInit();

		if (error != GLEW_OK)
		{
			FatalError("Could not initialise glew.");
		}
#endif

		int drawableW = 0;
		int drawableH = 0;
		SDL_GL_GetDrawableSize(m_sdlWindow, &drawableW, &drawableH);
		if (drawableW > 0 && drawableH > 0)
		{
			m_screenWidth = drawableW;
			m_screenHeight = drawableH;
		}
		glViewport(0, 0, m_screenWidth, m_screenHeight);

		printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		SDL_GL_SetSwapInterval(0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return 0;
	}

	void Window::OnResize(int newWidth, int newHeight)
	{
		if (newWidth > 0 && newHeight > 0)
		{
			m_screenWidth = newWidth;
			m_screenHeight = newHeight;
			glViewport(0, 0, m_screenWidth, m_screenHeight);
		}
	}

	void Window::Swap()
	{
		SDL_GL_SwapWindow(m_sdlWindow);
	}

}