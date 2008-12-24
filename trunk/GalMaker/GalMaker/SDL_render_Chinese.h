#ifndef RENDER_CHINESE_H
#define RENDER_CHINESE_H

#include "gb2312_to_Unicode.h"
#include "SDL/SDL_ttf.h"

SDL_Surface* myTTF_RenderString_Blended(TTF_Font* font, const std::string& str, SDL_Color fg);
SDL_Surface* myTTF_RenderString_Solid(TTF_Font* font, const std::string& str, SDL_Color fg);
SDL_Surface* myTTF_RenderString_Shaded(TTF_Font* font, const std::string& str, SDL_Color fg, SDL_Color bg);

#endif