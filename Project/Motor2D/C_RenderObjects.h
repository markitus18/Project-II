#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "SDL\include\SDL.h"

enum C_Sprite_Type
{
	SCENE, 
	GUI,
};

struct C_Sprite
{
	SDL_Texture*		texture;
	SDL_Rect			position;
	SDL_Rect			section;

	bool				useCamera = true;
	SDL_RendererFlip	flip = SDL_FLIP_NONE;

	int					y_ref;
	int					layer;
};

struct C_Circle
{
	C_Circle(int _x1, int _y1, int _radius, Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a, bool _useCamera)
		{x = _x1; y = _y1; radius = _radius; r = _r; g = _g; b = _b; a = _a; useCamera = _useCamera;}

	int x, y;
	int radius;
	Uint8 r, g, b, a;

	bool useCamera;
};

struct C_Rect
{
	C_Rect(SDL_Rect _rect, Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a, bool _useCamera, bool _filled)
		{ rect = _rect; r = _r; g = _g; b = _b; a = _a; useCamera = _useCamera; filled = _filled; }

	SDL_Rect rect;
	Uint8 r, g, b, a;
	
	bool useCamera;
	bool filled;
};

struct C_Line
{
	C_Line(int _x1, int _y1, int _x2, int _y2, Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a, bool _useCamera)
		{x1 = _x1; y1 = _y1; x2 = _x2; y2 = _y2; r = _r; g = _g; b = _b; a = _a; useCamera = _useCamera;}

	int x1, y1, x2, y2;
	Uint8 r, g, b, a;

	bool useCamera;
};
#endif //__SPRITE_H__