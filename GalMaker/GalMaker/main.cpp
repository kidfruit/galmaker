#include "SurfaceClass.h"
#include "MixSoundClass.h"
#include "ButtonClass.h"







int main(int argc, char* argv[])
{
	//Create a SDL screen.
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	const Uint32 SCREEN_FLAGS = 0; //SDL_FULLSCREEN | SDL_DOUBLEBUF | SDL_HWSURFACE
	const std::string WINDOW_NAME = "GalMaker Alpha";
	ScreenSurface screen(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_NAME, 0, SCREEN_FLAGS);


	PictureSurface background("back.png",screen);

	background.blit();

	screen.flip();

	SDL_Delay(1500);

	return 0;
}