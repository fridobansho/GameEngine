#include <iostream>

#include "MainGame.h"

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
	MainGame mainGame;

	mainGame.Run();

	return 0;
}