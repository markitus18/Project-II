
#include "j1App.h"

#include "M_Particles.h"

#include "M_Render.h"

M_Particles::M_Particles(bool)
{

}

bool M_Particles::Start()
{
	return true;
}

void M_Particles::AddParticle(C_Sprite startingSprite, uint nFrames, float animSpeed, bool vertical)
{
	Particle toPush;
	toPush.explosionSprite = startingSprite;
	toPush.animSpeed = animSpeed;
	toPush.nFrames = nFrames;
	toPush.vertical = vertical;

	particles.push_back(toPush);

}

bool M_Particles::Update(float dt)
{
	if (!particles.empty())
	{
		std::list <Particle>::iterator it = particles.begin();
		while (it != particles.end())
		{
			//If it has an animation, keep it moving
			(*it).timer += dt;
			if ((*it).timer > (*it).animSpeed && it->toErase == false)
			{
				(*it).timer = 0.0f;
				if (it->vertical)
				{
					(*it).explosionSprite.section.y += (*it).explosionSprite.section.h;
				}
				else
				{
					(*it).explosionSprite.section.x += (*it).explosionSprite.section.w;
				}
				if ((*it).explosionSprite.section.y > (*it).nFrames*(*it).explosionSprite.section.h)
				{
					it->toErase = true;
				}
			}
			App->render->AddSprite(&(*it).explosionSprite, FX);
			it++;
		}


		it = particles.begin();
		while (it != particles.end())
		{
			//Erase ended explosions
			if (it->toErase)
			{
				std::list <Particle>::iterator it2 = it;
				it2++;
				particles.erase(it);
				if (it2 == particles.end())
				{
					break;
				}
				it = it2;
			}
			else
			{
				it++;
			}
		}

	}
	return true;
}