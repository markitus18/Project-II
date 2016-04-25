#ifndef _EXPLOSION__
#define _EXPLOSION__

#include "j1Module.h"

#include "C_Point.h"
#include "M_Textures.h"

class Explosion
{
public:
	bool Fuse(float time);
	bool ToErase();

	iPoint position;
	int radius = 100;
	int damage = 100;
	int nTicks = 1;
	float tickDelay = 1.0f;
private:
	float timer = 0.0f;
	int currentTick = 0;
};



class M_Explosion : public j1Module
{
public:
	M_Explosion(bool);
	~M_Explosion(){};

	bool Start();

	void AddExplosion(iPoint position, int radius, int damage, float delay = 1.0f, int nTicks = 1);

	bool Update(float dt);

private:
	std::list<Explosion> explosions;
};

#endif //_EXPLOSION__