#ifndef _EXPLOSION__
#define _EXPLOSION__

#include "j1Module.h"

#include "C_Point.h"

#include "Controlled.h"
#include "Entity.h"

class Explosion
{
	iPoint position;
};



class M_Explosion : public j1Module
{
public:
	M_Explosion(bool);
	~M_Explosion(){};

	bool Start();

	void AddExplosion(iPoint position, int radius, int damage);

	bool Update(float dt);

private:
};

#endif //_EXPLOSION__