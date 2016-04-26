#ifndef _PARTICLES_
#define _PARTICLES_

#include "j1Module.h"

#include "C_Point.h"

#include "Controlled.h"
#include "Entity.h"

struct Particle
{
	C_Sprite explosionSprite;
	float animSpeed = 0.25f;
	float timer = 0.0f;
	uint nFrames = 1;
	bool toErase = false;
};

class M_Particles : public j1Module
{
public:
	M_Particles(bool);
	~M_Particles(){};

	bool Start();

	void AddParticle();

	bool Update(float dt);
};

#endif //_PARTICLES_