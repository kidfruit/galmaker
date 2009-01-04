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
	TextSurface text("一二三四五六七八九十",screen,255,255,255);
	EffectSound mouseclick("mouseclick.wav");
	SDL_Rect mRect={100,100,text.point()->w,text.point()->h};
	Button mButton("按钮",screen,255,0,0,0,0,255);

	text.blit(background,100,100);
	mButton.setup(600,400);

	kidyin.SetAlpha(100);
	backmusic.play();
	kidyin.blit();
	mButton.blitOut();
	screen.flip();

	while(flag==0)
	{
		while ( SDL_PollEvent(&gameEvent) != 0 )
		{
			
			if ( mButton.effectiveClick(gameEvent) )
			{
				text.blit(100,200);
				for (int i=0;i<10;i++)
				{
					screen.UpdateRect( 100+i*20,200,20,25 );
					SDL_Delay(100);
					if ( SDL_PollEvent(&gameEvent)!=0 )
					{
						if ( gameEvent.button.button==SDL_BUTTON_LEFT )
						{
							screen.UpdateRect(100,200,200,25);
							break;
						}
					}
					SDL_Delay(10);
				}
				
			}

			text.clickBlit(gameEvent);

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
		//			background.blit();
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

			screen.flip();

		}
		SDL_Delay(10);
	}
	SDL_Delay(10);

	return 0;
}




