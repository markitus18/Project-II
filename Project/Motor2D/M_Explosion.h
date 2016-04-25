#ifndef _EXPLOSION__
#define _EXPLOSION__

#include "j1Module.h"

#include "C_Point.h"
#include "M_Textures.h"
#include "M_EntityManager.h"

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
};



class M_Explosion : public j1Module
{
public:
	M_Explosion(bool);
	~M_Explosion(){};

	bool Start();

	void AddExplosion(iPoint position, int radius, int damage, float delay = 4.0f, int nTicks = 1, Player_Type objective = PLAYER);

	bool Update(float dt);

	bool debug = true;
private:
	std::list<Explosion> explosions;
};

#endif //_EXPLOSION__