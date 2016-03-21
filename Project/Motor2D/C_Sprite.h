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

	bool				useCamera;
	SDL_RendererFlip	flip;

	int					y_ref;
	int					layer;
};

#endif //__SPRITE_H__