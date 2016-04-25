
#include "j1App.h"

#include "M_Explosion.h"

#include "M_Render.h"

#include "M_EntityManager.h"

bool Explosion::Fuse(float time)
{
	timer += time;
	if (timer >= tickDelay)
	{
		timer = 0.0f;
		currentTick++;
		return true;
	}
	return false;
}

bool Explosion::ToErase()
{
	return (currentTick >= nTicks);
}



M_Explosion::M_Explosion(bool start_enabled) : j1Module(start_enabled)
{
	name.create("Explosion");
}

bool M_Explosion::Start()
{
			
	return true;
}



void M_Explosion::AddExplosion(iPoint position, int radius, int damage, float delay, int nTicks)
{
	Explosion toPush;
	toPush.position = position;
	toPush.radius = radius;
	toPush.damage = damage;
	toPush.tickDelay = delay;
	toPush.nTicks = nTicks;

	explosions.push_back(toPush);
}

bool M_Explosion::Update(float dt)
{
	if (!explosions.empty())
	{
		std::list<Explosion>::iterator it = explosions.begin();
		while (it != explosions.end())
		{
			if (it->Fuse(dt))
			{




			}
		}
	}


	return true;
}