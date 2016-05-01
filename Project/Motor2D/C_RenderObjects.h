#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "SDL\include\SDL.h"
#include <map>
enum C_Sprite_Type
{
	DECAL,
	SCENE, 
	FLYER,
	FX,
	GUI,
	OVER_GUI,
	CURSOR,
};

enum C_Animation_Type
{
	A_LEFT,
	A_RIGHT,
	A_UP,
	A_DOWN,
	A_VERTICAL,
	A_HORIZONTAL
};

struct C_Sprite
{
	C_Sprite(){ tint = { 255, 255, 255, 255 }; layer = -1; }
	C_Sprite(SDL_Texture* _texture, SDL_Rect* _position, bool _useCamera = true, SDL_Rect* _section = NULL, SDL_RendererFlip _flip = SDL_FLIP_NONE, SDL_Color _tint = { 255, 255, 255, 0 })
		{texture = _texture; if (_position) position = *_position; useCamera = _useCamera; if (_section) section = *_section; flip = _flip; tint = _tint;}
	~C_Sprite()
	{ 
		
	}
	SDL_Texture*		texture;
	SDL_Rect			position;
	SDL_Rect			section;
	SDL_Color			tint;

	bool				useCamera = true;
	bool				inList = false;
	SDL_RendererFlip	flip = SDL_FLIP_NONE;

	std::multimap<int, C_Sprite>* list;
	int					y_ref;
	int					layer;
};

struct C_Animation
{
	C_Sprite sprite;
	C_Animation_Type type;

	bool direction = true;
	bool loopable = true;
	bool loopEnd = false;

	int rect_size_x;
	int rect_size_y;

	float currentRect = 0;
	
	int firstRect;
	int lastRect;

	float animSpeed = 0;

	C_Animation(){};

	C_Animation(const C_Animation& toCopy)
	{
		sprite = toCopy.sprite;
		type = toCopy.type;

		rect_size_x = toCopy.rect_size_x;
		rect_size_y = toCopy.rect_size_y;

		firstRect = toCopy.firstRect;
		lastRect = toCopy.lastRect;

		animSpeed = toCopy.animSpeed;
	}

	void Update(float dt)
	{
		if (animSpeed && !(!loopable && loopEnd))
		{
			if (type != A_VERTICAL && type != A_HORIZONTAL )
			{
				if (type != A_UP && type != A_LEFT)
				{
					currentRect += animSpeed * dt;
					if (currentRect >= lastRect + 1)
					{
						if (loopable)
							currentRect = firstRect;
						else
						{
							loopEnd = true;
							currentRect = lastRect;
						}
					}
				}
				else
				{
					currentRect -= animSpeed * dt;
					if (currentRect < lastRect)
					{
						if (loopable)
							currentRect = lastRect;
						else
						{
							loopEnd = true;
							currentRect = lastRect;
						}
					}
				}
			}
			else
			{
				if(direction)
				{
					currentRect += animSpeed * dt;
					if (currentRect >= lastRect + 1)
					{
						currentRect = lastRect - 0.1;
						direction = false;
					}
				}
				else
				{
					currentRect -= animSpeed * dt;
					if (currentRect < firstRect)
					{
						currentRect = firstRect + 1;
						direction = true;
					}
				}
			}
			if (type == A_VERTICAL || type == A_DOWN || type == A_UP)
			{
				sprite.section.y = (int)currentRect * rect_size_y;
			}
			else
			{
				sprite.section.x = (int)currentRect * rect_size_x;
			}
		}
	}
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