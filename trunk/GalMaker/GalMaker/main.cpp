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
	SDL_Event gameEvent;
	int flag=0;

	PictureSurface background("back.png",screen);
	MusicSound backmusic("music.ogg");
	TextSurface text("ÄãºÃ",screen);;
	EffectSound mouseclick("mouseclick.wav");


	background.blit();
	text.blit();
	backmusic.play();
	screen.flip();

	while(flag==0)
	{
		while ( SDL_PollEvent(&gameEvent) != 0 )
		{
			if ( gameEvent.type == SDL_QUIT )
			{
				flag=1;
				break;
			}
			if ( gameEvent.type == SDL_MOUSEBUTTONDOWN)
			{
				if ( gameEvent.button.button == SDL_BUTTON_LEFT )
				{
					mouseclick.play();
				}
			}

		}
	}
	SDL_Delay(10);

	return 0;
}