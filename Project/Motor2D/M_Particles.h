#ifndef _PARTICLES_
#define _PARTICLES_

#include "j1Module.h"

struct Particle
{
	C_Sprite explosionSprite;
	float animSpeed = 0.25f;
	float timer = 0.0f;
	uint nFrames = 1;
	bool vertical = true;
	bool toErase = false;
	float duration = 0.0f;
	int loopFrames = 1;
	C_Sprite_Type type = FX;
};

class M_Particles : public j1Module
{
public:
	M_Particles(bool);
	~M_Particles(){};

	bool Start();

	Particle* AddParticle(C_Sprite startingSprite, uint nFrames, float animSpeed, float duration = 0.0f, int loopFrames = 1, bool vertical = true);

	bool Update(float dt);

	std::list<Particle> particles;
};

#endif //_PARTICLES_