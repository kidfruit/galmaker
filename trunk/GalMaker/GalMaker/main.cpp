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
	int flag=0,k=0;
	int alphaValue=150;


	PictureSurface background("back.png",screen);
	PictureSurface kidyin("yin.png",screen);
	MusicSound backmusic("music.ogg");
	TextSurface text("ÄãºÃ",screen,255,255,255);
	EffectSound mouseclick("mouseclick.wav");
	SDL_Rect mRect={100,100,text.point()->w,text.point()->h};

	text.blit(background,100,100);
	text.blit(background,100,125);
	text.blit(background,100,150);
	text.blit(background,100,175);
	text.blit(background,100,200);
	text.blit(background,140,100);
	text.blit(background,180,100);

	kidyin.SetAlpha(100);
	backmusic.play();
	kidyin.blit();
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
					background.blit();
				}		
				if ( gameEvent.button.button == SDL_BUTTON_RIGHT )
				{
					mouseclick.play();
					kidyin.blit();
				}
				
			}
			if ( gameEvent.type == SDL_KEYDOWN )
			{
				kidyin.blit(200,100, 400, 300, 100, 100);
				if ( gameEvent.key.keysym.sym == SDLK_PAGEUP )
				{
					alphaValue+=50;
					kidyin.SetAlpha(alphaValue);
					//kidyin.blit(200,100, 400, 300, 100, 100);
				}
				if ( gameEvent.key.keysym.sym == SDLK_PAGEDOWN )
				{
					alphaValue-=50;
					kidyin.SetAlpha(alphaValue);
					//kidyin.blit(200,100, 400, 300, 100, 100);
				}
			}

			screen.UpdateRect( mRect );
		}
	}
	SDL_Delay(10);

	return 0;
}