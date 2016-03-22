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
	int x, y;
	int radius;
	Uint8 r, g, b, a;

	bool useCamera;
};

struct C_Quad 
{
	SDL_Rect rect;
	Uint8 r, g, b, a;
	
	bool useCamera;
	bool filled;
};

struct C_Line
{
	int x1, y1, x2, y2;
	Uint8 r, g, b, a;

	bool useCamera;
};
#endif //__SPRITE_H__