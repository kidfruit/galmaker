#ifndef SURFACE_CLASS_HPP
#define SURFACE_CLASS_HPP

#include <iostream>
#include <string>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "unzip.h"
#include "zlib.h"
#include "SDL_render_Chinese.h"

class ScreenSurface
{
private:
	//number of screen, make sure there is only ONE screen
	static int screenNum;
	//size & bpp  of screen
	int width;
	int height;
	int bpp;
	//common flags:SDL_SWSURFACE, SDL_HWSURFACE, SDL_DOUBLEBUF, SDL_FULLSCREEN...
	//more: http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fSetVideoMode
	Uint32 flags;
	//other attribute
	SDL_Surface* pScreen;
	std::string windowName;
public:
	//construct & deconstruct
	ScreenSurface();
	ScreenSurface(int w, int h, const std::string& window_name = "NULL", int b = 0, Uint32 f = 0);
	~ScreenSurface();
	//screen's point
	SDL_Surface* point() const;
	//flip the screen
	void flip() const;
	//fill the screen with some color, default is black.
	void fillColor(Uint8 r = 0, Uint8 g = 0, Uint8 b = 0) const;

	//update a rect
	void UpdateRect( const SDL_Rect Rect );
	void UpdateRect( Sint32 x, Sint32 y, Uint32 w, Uint32 h );
};

class BaseSurface
{
private:
	//
protected:
	SDL_Surface* pScreen;
	SDL_Surface* pSurface;
	BaseSurface();
public:
	BaseSurface(const BaseSurface& copy);
	virtual ~BaseSurface();
	BaseSurface& operator=(const BaseSurface& copy);
	//surface's point
	SDL_Surface* point() const;
	//blit surface to screen
	void blit() const;
	void blit(int any_num) const;
	void blit(int at_x, int at_y) const;
	void blit(int at_x, int at_y,
				int from_x, int from_y, int w, int h,
				int delta_x = 0, int delta_y = 0) const;
	//blit surface to another surface
	void blit(const BaseSurface& dst_surface) const;
	void blit(const BaseSurface& dst_surface, int any_num) const;
	void blit(const BaseSurface& dst_surface,
						int at_x, int at_y) const;
	void blit(const BaseSurface& dst_surface,
						int at_x, int at_y,
						int from_x, int from_y, int w, int h,
						int delta_x = 0, int delta_y = 0) const;
	//color keying
	void colorKey(Uint8 r = 0, Uint8 g = 0xFF, Uint8 b = 0xFF, Uint32 flag = SDL_SRCCOLORKEY);


	SDL_RWops* GetFileFromPack( const std::string& fileInPack, const std::string& packName );
	void SetAlpha( const Uint8 alpha, const Uint32 flag = SDL_SRCALPHA );
	

};

class PictureSurface: public BaseSurface
{
private:
	std::string fileName;
public:
	PictureSurface(const std::string& file_name, const ScreenSurface& screen, const std::string& packName="graph.kid");
};

class TextSurface: public BaseSurface
{
private:
	static int textNum;
	std::string message;
	std::string TTF_fileName;
	int TTF_size;
	Uint8 r, g, b;
public:
	TextSurface(const std::string& _message, const ScreenSurface& screen,
		Uint8 _r = 0xFF, Uint8 _g = 0xFF, Uint8 _b = 0xFF, 
		int ttf_size = 20, const std::string& ttf_fileName = "default.ttf");
	TextSurface(const TextSurface& copy);
	~TextSurface();
	//text tools
	void toBlended();
	void toSolid();
	void toShaded(Uint8 _r, Uint8 _g, Uint8 _b);
	void setColor(Uint8 _r, Uint8 _g, Uint8 _b);
	void setSize(int ttf_size);
	void setFont(const std::string& ttf_fileName);
};

class ErrorInfo
{
private:
	std::string info;
public:
	ErrorInfo():info("Unknown ERROR!")
	{}
	ErrorInfo(const char* c_str)
	{
		info = std::string(c_str);
	}
	ErrorInfo(const std::string& str):info(str)
	{}
	void show() const
	{
		std::cerr << info << std::endl;
	}
};

#endif


