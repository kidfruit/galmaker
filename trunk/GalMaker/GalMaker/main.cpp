#include <windows.h>
#include <stdio.h>							// printf
#include <sys/stat.h>
#include <direct.h>							
#include <fcntl.h>							// Used for O_WRONLY | O_EXCL | O_CREAT, _S_IREAD
#include <io.h>
#include <sdl/SDL.h>
#include <sdl/SDL_image.h>

#pragma comment(lib,"SDL.lib")
#pragma comment(lib,"SDLmain.lib")
#pragma comment(lib,"SDL_ttf.lib")
#pragma comment(lib,"SDL_image.lib")



//Screen attributes
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *screen = NULL;
SDL_Surface *background =NULL;

//The event structure
SDL_Event event;




char *GetBufferFromResource(char *resourcefilename, char *resourcename, int *filesize) 
{
	//Try to open the resource file in question
	int fd = open(resourcefilename, O_RDONLY);
	if (fd < 0) 
	{
		perror("Error opening resource file");
		exit(1);
	}

	//Make sure we're at the beginning of the file
	lseek(fd, 0, SEEK_SET);

	//Read the first INT, which will tell us how many files are in this resource
	int numfiles;
	read(fd, &numfiles, sizeof(int));

	//Get the pointers to the stored files
	int *filestart = (int *) malloc(sizeof(int) * numfiles);	// this is probably wrong in the zip
	read(fd, filestart, sizeof(int) * numfiles);

	//Loop through the files, looking for the file in question
	int filenamesize;
	char *buffer;
	int i;
	for(i=0;i<numfiles;i++) 
	{
		char *filename;
		//Seek to the location
		lseek(fd, filestart[i], SEEK_SET);
		//Get the filesize value
		read(fd, filesize, sizeof(int));
		//Get the size of the filename string
		read(fd, &filenamesize, sizeof(int));
		//Size the buffer and read the filename
		filename = (char *) malloc(filenamesize + 1);
		read(fd, filename, filenamesize);
		//Remember to terminate the string properly!
		filename[filenamesize] = '\0';
		//Compare to the string we're looking for
		if (strcmp(filename, resourcename) == 0) 
		{
			//Get the contents of the file
			buffer = (char *) malloc(*filesize);
			read(fd, buffer, *filesize);
			free(filename);
			break;
		}
		//Free the filename buffer
		free(filename);
	}

	//Release memory
	free(filestart);

	//Close the resource file!
	close(fd);

	//Did we find the file within the resource that we were looking for?
	if (buffer == NULL) 
	{
		printf("Unable to find '%s' in the resource file!\n", resourcename);
		exit(1);
	}

	//Return the buffer
	return buffer;
}

SDL_Surface *LoadBitmap(char *resourcefilename, char *bitmapfilename) 
{
	//Get the bitmap's buffer and size from the resource file
	int filesize = 0;
	char *buffer = GetBufferFromResource(resourcefilename, bitmapfilename, &filesize);

	//Load the buffer into a surface using RWops
	SDL_RWops *rw = SDL_RWFromMem(buffer, filesize);
	SDL_Surface *temp = IMG_Load_RW(rw, 1);

	//Release the bitmap buffer memory
	free(buffer);

	//Were we able to load the bitmap?
	if (temp == NULL) 
	{
		printf("Unable to load bitmap: %s\n", SDL_GetError());
		exit(1);
	}

	//Convert the image to optimal display format
	SDL_Surface *image;
	image = SDL_DisplayFormat(temp);

	//Free the temporary surface
	SDL_FreeSurface(temp);

	//Return our loaded image
	return image;
}



SDL_Surface *load_image( char* filename ) 
{
	//The image that's loaded
	SDL_Surface* loadedImage = NULL;

	//The optimized surface that will be used
	SDL_Surface* optimizedImage = NULL;


	loadedImage = IMG_Load( filename );


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

//	background = load_image("back.png");

	background = LoadBitmap("data.kid","setup.bmp");

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
