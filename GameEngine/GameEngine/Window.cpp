#include "GameEngineErrors.h"

#include "Window.h"


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

		if (currentFlags & INVISIBLE)
		{
			flags |= SDL_WINDOW_HIDDEN;
		}

		if (currentFlags & FULLSCREEN)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		if (currentFlags & BORDERLESS)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		m_screenWidth = screenWidth;
		m_screenHeight = screenHeight;

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

		GLenum error = glewInit();

		if (error != GLEW_OK)
		{
			FatalError("Could not initialise glew.");
		}

		printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		SDL_GL_SetSwapInterval(0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return 0;
	}

	void Window::Swap()
	{
		SDL_GL_SwapWindow(m_sdlWindow);
	}

}