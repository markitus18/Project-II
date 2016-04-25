
#include "j1App.h"

#include "M_Explosion.h"

#include "M_Render.h"

#include "M_EntityManager.h"
#include "Unit.h"
#include "Building.h"

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
				fPoint center = { (float)it->position.x, (float)it->position.y };
				if (App->entityManager->unitList.empty() == false)
				{
					std::list<Unit*>::iterator unitIt = App->entityManager->unitList.begin();
					while (unitIt != App->entityManager->unitList.end())
					{
						if ((*unitIt)->GetPosition().DistanceNoSqrt(center) < it->radius * it->radius)
						{
							(*unitIt)->Hit(it->damage);
						}
						unitIt++;
					}
				}

				if (App->entityManager->buildingList.empty() == false)
				{
					std::list<Building*>::iterator buildIt = App->entityManager->buildingList.begin();
					{
						while (buildIt != App->entityManager->buildingList.end())
						{
							SDL_Rect tmp = (*buildIt)->GetCollider();
							fPoint pos((float)(tmp.x + tmp.w / 2), (float)(tmp.y + tmp.h / 2));
							if (pos.DistanceNoSqrt(center) < it->radius * it->radius)
							{
								(*buildIt)->Hit(it->damage);
							}
							buildIt++;
						}
					}
				}



			}


			App->render->AddCircle(it->position.x, it->position.y, it->radius * (it->timer / it->tickDelay), true, 255, 0, 0);
			App->render->AddCircle(it->position.x, it->position.y, it->radius, true, 255, 0, 255);
			it++;
		}

		it = explosions.begin();
		while (it != explosions.end())
		{
			if (it->ToErase())
			{
				std::list<Explosion>::iterator it2 = it;
				it2++;
				explosions.erase(it);
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