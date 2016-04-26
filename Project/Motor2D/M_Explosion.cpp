
#include "j1App.h"

#include "M_Explosion.h"

#include "M_Render.h"

#include "Unit.h"
#include "Building.h"
#include "M_Particles.h"

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



void ExplosionSystem::PushExplosion(float delay, iPoint relativePos, int radius, int damage, int nTicks, float tickDelay, Player_Type objective, e_Explosion_Types graphic)
{
	std::pair<float, StoredExplosion> toPush;
	
	toPush.first = delay;

	toPush.second.timeUntilExplosion = delay;
	toPush.second.position = relativePos;
	toPush.second.radius = radius;
	toPush.second.damage = damage;
	toPush.second.nTicks = nTicks;
	toPush.second.tickDelay = tickDelay;
	toPush.second.objective = objective;
	toPush.second.blown = false;
	toPush.second.graphic = graphic;
	
	explosions.insert(toPush);

}

bool ExplosionSystem::Update(float dt)
{
	bool ret = false;
	if (explosions.empty() == false)
	{
		timer += dt;
		std::multimap<float, StoredExplosion>::iterator it = explosions.begin();
		while (it != explosions.end())
		{
			if (it->second.blown == false)
			{
				if (timer >= it->first)
				{
					App->explosion->AddExplosion(position + it->second.position, it->second.radius, it->second.damage, it->second.tickDelay, it->second.nTicks, it->second.objective, it->second.graphic);
					it->second.blown = true;
				}
				ret = true;
			}
			it++;
		}
	}
	return ret;
}



M_Explosion::M_Explosion(bool start_enabled) : j1Module(start_enabled)
{
	name.create("Explosion");
}

bool M_Explosion::Start()
{
	green = App->tex->Load("graphics/ui/Stencil/1.png");
	yellow = App->tex->Load("graphics/ui/Stencil/2.png");
	red = App->tex->Load("graphics/ui/Stencil/3.png");

	hugeExplosion.texture = App->tex->Load("graphics/neutral/missiles/spider mine explosion.png");
	hugeExplosion.position = { 0, 0, 80, 80 };
	hugeExplosion.section = { 0, 0, 80, 80 };

	terranExplosion.texture = App->tex->Load("graphics/neutral/missiles/pdriphit.png");
	terranExplosion.position = { 0, 0, 52, 52 };
	terranExplosion.section = { 0, 0, 52, 52 };

	//First round
	testingSystem.PushExplosion(0.0f, { 50, 0 }, 40, 100, 1, 3.0f, PLAYER, EXPLOSION_TERRAN);
	testingSystem.PushExplosion(0.0f, { -50, 0 }, 40, 100, 1, 3.0f, PLAYER, EXPLOSION_TERRAN);
	testingSystem.PushExplosion(0.0f, { 0, 50 }, 40, 100, 1, 3.0f, PLAYER, EXPLOSION_TERRAN);
	testingSystem.PushExplosion(0.0f, { 0, -50 }, 40, 100, 1, 3.0f, PLAYER, EXPLOSION_TERRAN);
	//Second round
	testingSystem.PushExplosion(3.5f, { 110, 50 }, 60, 75, 1, 4.0f);
	testingSystem.PushExplosion(3.5f, { 110, -50 }, 60, 75, 1, 4.0f);
	testingSystem.PushExplosion(3.5f, { -110, 50 }, 60, 75, 1, 4.0f);
	testingSystem.PushExplosion(3.5f, { -110, -50 }, 60, 75, 1, 4.0f);

	testingSystem.PushExplosion(3.5f, { 50, 110 }, 60, 75, 1, 4.0f);
	testingSystem.PushExplosion(3.5f, { -50, 110 }, 60, 75, 1, 4.0f);
	testingSystem.PushExplosion(3.5f, { 50, -110 }, 60, 75, 1, 4.0f);
	testingSystem.PushExplosion(3.5f, { -50, -110 }, 60, 75, 1, 4.0f);
	//Third Round
	testingSystem.PushExplosion(8.0f, { 0, 0 }, 200, 150, 1, 8);



	return true;
}

bool M_Explosion::Update(float dt)
{
	if (!explosions.empty())
	{
		std::list<Explosion>::iterator it = explosions.begin();
		while (it != explosions.end())
		{
			//Rendering
			if (it->showStencil)
			{
				if (it->timer / it->tickDelay < 0.45f)
				{
					stencil.texture = green;
				}
				else if (it->timer / it->tickDelay < 0.75f)
				{
					stencil.texture = yellow;
				}
				else
				{
					stencil.texture = red;
				}
				int r = it->radius * (it->timer / it->tickDelay);
				CAP(r, 1, INT_MAX);
				stencil.position = { it->position.x - r, it->position.y - r, r * 2, r * 2 };
				App->render->AddSprite(&stencil, DECAL);
				stencil.position = { it->position.x - it->radius, it->position.y - it->radius, it->radius * 2, it->radius * 2 };
				App->render->AddSprite(&stencil, DECAL);
			}

			if (it->Fuse(dt))
			{
#pragma region //exploding
				fPoint center = { (float)it->position.x, (float)it->position.y };
				if (App->entityManager->unitList.empty() == false)
				{
					std::list<Unit*>::iterator unitIt = App->entityManager->unitList.begin();
					while (unitIt != App->entityManager->unitList.end())
					{
						if ((*unitIt)->stats.player == it->objective || it->objective == CINEMATIC)
						{
							if ((*unitIt)->GetPosition().DistanceNoSqrt(center) < it->radius * it->radius)
							{
								(*unitIt)->Hit(it->damage);
							}
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
							if ((*buildIt)->stats.player == it->objective || it->objective == CINEMATIC)
							{
								SDL_Rect tmp = (*buildIt)->GetCollider();
								fPoint pos((float)(tmp.x + tmp.w / 2), (float)(tmp.y + tmp.h / 2));
								if (pos.DistanceNoSqrt(center) < it->radius * it->radius)
								{
									(*buildIt)->Hit(it->damage);
								}
							}
							buildIt++;
						}
					}
				}

				switch (it->graphic)
				{
				case (EXPLOSION_TERRAN):
					{
						terranExplosion.position.x = it->position.x - it->radius;
						terranExplosion.position.y = it->position.y - it->radius;
						terranExplosion.position.w = it->radius * 2;
						terranExplosion.position.h = it->radius * 2;
						App->particles->AddParticle(terranExplosion, 6, 0.1f);
						break;
					}
				default:
				{
					hugeExplosion.position.x = it->position.x - it->radius;
					hugeExplosion.position.y = it->position.y - it->radius;
					hugeExplosion.position.w = it->radius * 2;
					hugeExplosion.position.h = it->radius * 2;
					App->particles->AddParticle(hugeExplosion, 9, 0.06f);
				}
				}
#pragma endregion
			}
			//Debug Rendering
			if (debug)
			{
				App->render->AddCircle(it->position.x, it->position.y, it->radius * (it->timer / it->tickDelay), true, 255, 0, 0);
				App->render->AddCircle(it->position.x, it->position.y, it->radius, true, 255, 0, 255);
			}
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

	if (explosionSystems.empty() == false)
	{
		std::list<ExplosionSystem>::iterator it = explosionSystems.begin();
		{
			while (it != explosionSystems.end())
			{
				if (it->Update(dt) == false)
				{
					std::list<ExplosionSystem>::iterator it2 = it;
					it2++;
					explosionSystems.erase(it);
					it = it2;
				}
				else
				{
					it++;
				}

			}
		}
	}


	return true;
}

bool M_Explosion::CleanUp()
{
	App->tex->UnLoad(green);
	App->tex->UnLoad(yellow);
	App->tex->UnLoad(red);
	return true;
}


void M_Explosion::AddExplosion(iPoint position, int radius, int damage, float delay, int nTicks, Player_Type objective, e_Explosion_Types graphic, bool showStencil)
{
	Explosion toPush;
	toPush.position = position;
	toPush.radius = radius;
	toPush.damage = damage;
	toPush.tickDelay = delay;
	toPush.nTicks = nTicks;
	toPush.objective = objective;
	toPush.showStencil = showStencil;
	toPush.graphic = graphic;

	explosions.push_back(toPush);
}

void M_Explosion::AddSystem(ExplosionSystem toPush, iPoint pos)
{
	toPush.position = pos;
	explosionSystems.push_back(toPush);
}