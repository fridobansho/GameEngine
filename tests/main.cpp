#include "TestHarness.h"

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0) {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

        SDL_Window* window = SDL_CreateWindow(
            "EngineTests_Hidden",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            640,
            480,
            SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
        );

        if (window) {
            SDL_GLContext glContext = SDL_GL_CreateContext(window);
            if (glContext) {
                glewExperimental = GL_TRUE;
                GLenum glewErr = glewInit();
                if (glewErr == GLEW_OK) {
                    TestHarness::hasGLContext() = true;
                }
            }
        }
    }

    if (!TestHarness::hasGLContext()) {
        std::cout << "[Note] No active OpenGL context available. Offscreen GL tests will be skipped.\n";
    }

    int result = 0;
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--list" || arg == "--list-tests") {
            for (const auto& t : TestHarness::getTests()) {
                std::cout << t.name << "\n";
            }
            SDL_Quit();
            return 0;
        }
        result = TestHarness::runSingleTest(arg);
    } else {
        result = TestHarness::runAllTests();
    }

    SDL_Quit();
    return result;
}
