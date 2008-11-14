#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>



//Screen attributes
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *screen = NULL;
SDL_Surface *background =NULL;

//The event structure
SDL_Event event;



SDL_Surface *load_image( std::string filename ) 
{
	//The image that's loaded
	SDL_Surface* loadedImage = NULL;

	//The optimized surface that will be used
	SDL_Surface* optimizedImage = NULL;

	//Load the image
	loadedImage = IMG_Load( filename.c_str() );

	//If the image loaded
	if( loadedImage != NULL )
	{
		//Create an optimized surface
		optimizedImage = SDL_DisplayFormat( loadedImage );

		//Free the old surface
		SDL_FreeSurface( loadedImage );

		//If the surface was optimized
		if( optimizedImage != NULL )
		{
			//Color key surface
			SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
		}
	}

	//Return the optimized surface
	return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
	//Holds offsets
	SDL_Rect offset;

	//Get offsets
	offset.x = x;
	offset.y = y;

	//Blit
	SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
	//Initialize all SDL subsystems
	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		return false;    
	}

	//Set up the screen
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

	//If there was an error in setting up the screen
	if( screen == NULL )
	{
		return false;    
	}

	//Set the window caption
	SDL_WM_SetCaption( "GalMaker Alpha", NULL );

	//If everything initialized fine
	return true;
}

void clean_up()
{
	SDL_FreeSurface( screen );
	//Quit SDL
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Quit flag
	bool quit = false;

	//Initialize
	if( init() == false )
	{
		return 1;
	}

	background = load_image("background.png");
	apply_surface(0,0,background,screen);


	//Update the screen
	if( SDL_Flip( screen ) == -1 )
	{
		return 1;    
	}

	//While the user hasn't quit
	while( quit == false )
	{
		//While there's events to handle
		while( SDL_PollEvent( &event ) )
		{
			//If the user has Xed out the window
			if( event.type == SDL_QUIT )
			{
				//Quit the program
				quit = true;
			}    
		}
	}

	//Free surfaces and font then quit SDL_ttf and SDL 
	clean_up();

	return 0;    
}
