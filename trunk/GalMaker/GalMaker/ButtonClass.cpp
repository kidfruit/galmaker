//UVi Soft ( 2008 )
//Long Fei ( lf426 ), E-mail: zbln426@163.com
//Laboratory of ZaiBieLiunNian
//http://www.cppblog.com/lf426/

#include "ButtonClass.h"

//*************************
//class BaseButton

EffectSound BaseButton::es("./sounds/menu1.wav");

BaseButton::BaseButton():
atX(0), atY(0), w(0), h(0), offset(0),
inBox(false), clickDown(false), clickUp(false)
{}

BaseButton::~BaseButton()
{}

void BaseButton::setup(int at_x, int at_y, int _offset)
{
	atX = at_x;
	atY = at_y;
	offset = _offset;
}

bool BaseButton::mouseOver(const SDL_Event& gameEvent) const
{
	if ( gameEvent.type == SDL_MOUSEMOTION ){
		int mouse_at_x = gameEvent.motion.x;
		int mouse_at_y = gameEvent.motion.y;
		if ( mouse_at_x >= atX && mouse_at_x <= atX + w &&
			mouse_at_y >= atY && mouse_at_y <= atY + h ){
				if ( inBox == false )
					es.play();
				return true;
		}
		else return false;
	}
	else return false;
}

bool BaseButton::mouseDown(const SDL_Event& gameEvent) const
{
	if ( gameEvent.type == SDL_MOUSEBUTTONDOWN )
		if( gameEvent.button.button == SDL_BUTTON_LEFT ){
			int mouse_at_x = gameEvent.button.x;
			int mouse_at_y = gameEvent.button.y;
			if ( mouse_at_x >= atX && mouse_at_x <= atX + w &&
				mouse_at_y >= atY && mouse_at_y <= atY + h )
				return true;
			else return false;
		}
		else return false;
	else return false;
}

bool BaseButton::mouseUp(const SDL_Event& gameEvent) const
{
	if ( gameEvent.type == SDL_MOUSEBUTTONUP )
		if( gameEvent.button.button == SDL_BUTTON_LEFT ){
			int mouse_at_x = gameEvent.button.x;
			int mouse_at_y = gameEvent.button.y;
			if ( mouse_at_x >= atX && mouse_at_x <= atX + w &&
				mouse_at_y >= atY && mouse_at_y <= atY + h )
				return true;
			else return false;
		}
		else return false;
	else return false;
}

bool BaseButton::mouseUpOutside(const SDL_Event& gameEvent) const
{
	if ( gameEvent.type == SDL_MOUSEBUTTONUP )
		if( gameEvent.button.button == SDL_BUTTON_LEFT ){
			int mouse_at_x = gameEvent.button.x;
			int mouse_at_y = gameEvent.button.y;
			if ( mouse_at_x <= atX || mouse_at_x >= atX + w ||
				mouse_at_y <= atY || mouse_at_y >= atY + h )
				return true;
			else return false;
		}
		else return false;
	else return false;
}

bool BaseButton::effectiveClick(const SDL_Event& game_event)
{
	inBox = this->mouseOver(game_event);
	if ( this->mouseDown(game_event) == true ){
		clickDown = true;
		inBox = true;
	}
	if ( this->mouseUp(game_event) == true ){
		if ( clickDown == true )
			clickUp = true;
		inBox = true;
	}
	if ( this->mouseUpOutside(game_event) == true )
		clickDown = false;

	if ( inBox == true && clickDown == false ){
		this->blitOver();
		return false;
	}
	else if ( inBox == true && clickDown == true ){
		if ( clickUp == true ){
			clickUp = false;
			clickDown = false;
			this->blitOver();
			return true;
		} else {
			this->blitDown();
			return false;
		}
	}
	else {
		this->blitOut();
		return false;
	}
}

//*************************


//*************************
//class Button
Button::Button(const std::string& outImg_fileName, const std::string& overImg_fileName, const ScreenSurface& screen):
BaseButton(),
outImg(PictureSurface(outImg_fileName, screen)),
overImg(PictureSurface(overImg_fileName, screen))
{
	w = outImg.point()->w;
	h = outImg.point()->h;
}

Button::Button(const BaseSurface& out_img, const BaseSurface& over_img):
BaseButton(),
outImg(out_img), overImg(over_img)
{
	w = outImg.point()->w;
	h = outImg.point()->h;
}

Button::Button(const std::string buttonText, const ScreenSurface& screen,
			   Uint8 out_r, Uint8 out_g, Uint8 out_b, Uint8 on_r, Uint8 on_g, Uint8 on_b,
			   int ttf_size, const std::string& ttf_fileName):
BaseButton(),
outImg(TextSurface(buttonText, screen, out_r, out_g, out_b, ttf_size, ttf_fileName)),
overImg(TextSurface(buttonText, screen, on_r, on_g, on_b, ttf_size, ttf_fileName))
{
	w = outImg.point()->w;
	h = outImg.point()->h;
}

Button::~Button()
{}

void Button::colorKey(Uint8 r, Uint8 g, Uint8 b)
{
	outImg.colorKey(r, g, b);
	overImg.colorKey(r, g, b);
}

void Button::blitOut() const
{
	outImg.blit(atX, atY);
}

void Button::blitOver() const
{
	overImg.blit(atX, atY);
}

void Button::blitDown() const
{
	overImg.blit(atX+offset, atY+offset);
}

void Button::addText(const TextSurface& out_text, const TextSurface& over_text)
{
	out_text.blit(outImg);
	over_text.blit(overImg);
}

//****************************

//****************************
//class ButtonPlus
ButtonPlus::ButtonPlus(const std::string& outImg_fileName, const std::string& overImg_fileName, const std::string& downImg_fileName,
		const ScreenSurface& screen):
Button(outImg_fileName, overImg_fileName, screen),
downImg(PictureSurface(downImg_fileName, screen))
{}

ButtonPlus::ButtonPlus(const BaseSurface& out_img, const BaseSurface& over_img, const BaseSurface& down_img):
Button(out_img, over_img),
downImg(down_img)
{}

void ButtonPlus::colorKey(Uint8 r, Uint8 g, Uint8 b)
{
	Button::colorKey(r, g, b);
	downImg.colorKey(r, g, b);
}

void ButtonPlus::blitDown() const
{
	downImg.blit(atX+offset, atY+offset);
}

void ButtonPlus::addText(const TextSurface& out_text, const TextSurface& over_text)
{
	Button::addText(out_text, over_text);
	over_text.blit(downImg);
}

//****************************

//****************************
//class SpriteButton

SpriteButton::SpriteButton(const std::string& spriteSheet_fileName, const ScreenSurface& screen,
		int button_w, int button_h,
		int out_x, int out_y, int over_x, int over_y, int down_x, int down_y):
BaseButton(),
spriteSheet(spriteSheet_fileName, screen),
outX(out_x), outY(out_y), overX(over_x), overY(over_y), downX(down_x), downY(down_y)
{
	w = button_w;
	h = button_h;
}

void SpriteButton::colorKey(Uint8 r, Uint8 g, Uint8 b)
{
	spriteSheet.colorKey(r, g, b);
}

void SpriteButton::blitOut() const
{
	spriteSheet.blit(atX, atY, outX, outY, w, h);
}

void SpriteButton::blitOver() const
{
	spriteSheet.blit(atX, atY, overX, overY, w, h);
}

void SpriteButton::blitDown() const
{
	spriteSheet.blit(atX+offset, atY+offset, downX, downY, w, h);
}

void SpriteButton::addText(const TextSurface& out_text, const TextSurface& over_text)
{
	const int DELTA_outX = (w - out_text.point()->w) / 2;
	const int DELTA_outY = (h - out_text.point()->h) / 2;
	const int DELTA_overX = (w - over_text.point()->w) / 2;
	const int DELTA_overY = (h - over_text.point()->h) / 2;

	out_text.blit(spriteSheet, outX+DELTA_outX, outY+DELTA_outY);
	over_text.blit(spriteSheet, overX+DELTA_overX, overY+DELTA_overY);
	over_text.blit(spriteSheet, downX+DELTA_overX, downY+DELTA_overY);
}

//*********************************

