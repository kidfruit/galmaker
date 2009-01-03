#ifndef BUTTON_CLASS_HPP
#define BUTTON_CLASS_HPP

#include "SurfaceClass.h"
#include "MixSoundClass.h"

class BaseButton
{
private:
	//sound effect
	static EffectSound es;
protected:
	int atX;
	int atY;
	int offset;
	int w;
	int h;
	//ButtonEffect
	bool inBox;
	bool clickDown;
	bool clickUp;
public:
	BaseButton();
	virtual ~BaseButton();
	void setup(int at_x, int at_y, int _offset = 0);
	virtual void colorKey(Uint8 r, Uint8 g, Uint8 b) = 0;
	virtual void blitOut() const = 0;
	virtual void blitOver() const = 0;
	virtual void blitDown() const = 0;
	virtual void addText(const TextSurface& out_text, const TextSurface& over_text) = 0;
	bool mouseOver(const SDL_Event& gameEvent) const;
	bool mouseDown(const SDL_Event& gameEvent) const;
	bool mouseUp(const SDL_Event& gameEvent) const;
	bool mouseUpOutside(const SDL_Event& gameEvent) const;
	bool effectiveClick(const SDL_Event& game_event);
};

class Button: public BaseButton
{
private:
	//
protected:
	BaseSurface outImg;
	BaseSurface overImg;
public:
	Button(const std::string& outImg_fileName, const std::string& overImg_fileName, const ScreenSurface& screen);
	Button(const BaseSurface& out_img, const BaseSurface& over_img);
	Button(const std::string buttonText, const ScreenSurface& screen,
		Uint8 out_r = 0xFF, Uint8 out_g = 0xFF, Uint8 out_b = 0xFF, Uint8 on_r = 0, Uint8 on_g = 0, Uint8 on_b = 0xFF,
		int ttf_size = 20, const std::string& ttf_fileName = "default.ttf");
	virtual ~Button();
	virtual void colorKey(Uint8 r = 0, Uint8 g = 0xFF, Uint8 b = 0xFF);
	virtual void blitOut() const;
	virtual void blitOver() const;
	virtual void blitDown() const;
	virtual void addText(const TextSurface& out_text, const TextSurface& over_text);
};

class ButtonPlus: public Button
{
private:
	BaseSurface downImg;
public:
	ButtonPlus(const std::string& outImg_fileName, const std::string& overImg_fileName, const std::string& downImg_fileName,
		const ScreenSurface& screen);
	ButtonPlus(const BaseSurface& out_img, const BaseSurface& over_img, const BaseSurface& down_img);
	virtual void colorKey(Uint8 r = 0, Uint8 g = 0xFF, Uint8 b = 0xFF);
	virtual void blitDown() const;
	virtual void addText(const TextSurface& out_text, const TextSurface& over_text);
};

class SpriteButton: public BaseButton
{
private:
	PictureSurface spriteSheet;
	int outX;
	int outY;
	int overX;
	int overY;
	int downX;
	int downY;
public:
	SpriteButton(const std::string& spriteSheet_fileName, const ScreenSurface& screen,
		int button_w, int button_h,
		int out_x, int out_y, int over_x, int over_y, int down_x, int down_y);
	virtual void colorKey(Uint8 r = 0, Uint8 g = 0xFF, Uint8 b = 0xFF);
	virtual void blitOut() const;
	virtual void blitOver() const;
	virtual void blitDown() const;
	virtual void addText(const TextSurface& out_text, const TextSurface& over_text);
};

#endif

