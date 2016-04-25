#ifndef _EXPLOSION__
#define _EXPLOSION__

#include "j1Module.h"

#include "C_Point.h"
#include "M_Textures.h"
#include "M_EntityManager.h"

enum e_Explosion_Types
{
	EXPLOSION_DEFAULT,
	EXPLOSION_TERRAN,
};

class Explosion
{
public:
	bool Fuse(float time);
	bool ToErase();

	iPoint position;
	int radius;
	int damage;
	int nTicks;
	float tickDelay;
	/////
	float timer = 0.0f;
	int currentTick = 0;
	Player_Type objective = PLAYER;
	bool showStencil;

	e_Explosion_Types graphic;
};



class M_Explosion : public j1Module
{
public:
	M_Explosion(bool);
	~M_Explosion(){};

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void AddExplosion(iPoint position, int radius, int damage, float delay = 4.0f, int nTicks = 1, Player_Type objective = PLAYER, e_Explosion_Types graphic = EXPLOSION_DEFAULT, bool showStencil = true);


	bool debug = false;
private:
	std::list<Explosion> explosions;

	C_Sprite stencil;
	SDL_Texture* green;
	SDL_Texture* yellow;
	SDL_Texture* red;

	C_Sprite explosionSprite;
	SDL_Texture* TexDefault;
	SDL_Texture* TexTerran;

};

#endif //_EXPLOSION__