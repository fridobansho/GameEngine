#include <cstdio>
#include <SDL.h>
#include <GL/glew.h>

#if _MSC_VER >= 1400
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

void stub()
{
	fprintf(NULL, "");
}
#endif

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	return 0;
}