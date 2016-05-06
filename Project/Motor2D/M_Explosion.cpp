#include "M_Explosion.h"

#include "j1App.h"
#include "M_EntityManager.h"

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


ExplosionSystem::ExplosionSystem()
{
	toSpawn = UNIT_NONE;
}
ExplosionSystem::ExplosionSystem(Unit_Type _toSpawn)
{
	toSpawn = _toSpawn;
}

void ExplosionSystem::SetSpawningUnit(Unit_Type _toSpawn)
{
	toSpawn = _toSpawn;
}

void ExplosionSystem::PushExplosion(float delay, iPoint relativePos, int radius, int damage, int nTicks, float tickDelay, Player_Type objective, bool showStencil, e_Explosion_Types graphic, int innerRadius)
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
	toPush.second.showStencil = showStencil;
	toPush.second.innerRadius = innerRadius;
	
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
					App->explosion->AddExplosion(position + it->second.position, it->second.radius, it->second.damage, it->second.tickDelay, it->second.nTicks, it->second.objective, it->second.graphic, it->second.showStencil);
					if (toSpawn != UNIT_NONE)
					{
						App->entityManager->CreateUnit(position.x + it->second.position.x, position.y + it->second.position.y, toSpawn, COMPUTER);
					}
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

	hugeExplosion.texture = App->tex->Load("graphics/neutral/missiles/explosion large.png");
	hugeExplosion.position = { 0, 0, 252, 252 };
	hugeExplosion.section = { 0, 0, 252, 200 };

	terranExplosion.texture = App->tex->Load("graphics/neutral/missiles/pdriphit.png");
	terranExplosion.position = { 0, 0, 52, 52 };
	terranExplosion.section = { 0, 0, 52, 52 };

	swarmExplosion.texture = App->tex->Load("graphics/neutral/missiles/dark swarm.png");
	swarmExplosion.position = { 0, 0, 252, 252 };
	swarmExplosion.section = { 0, 0, 252, 190 };

	psiStorm.texture = App->tex->Load("graphics/neutral/missiles/PsiStorm.png");
	psiStorm.position = { 0, 0, 167, 144 };
	psiStorm.section = { 0, 0, 167, 144 };

	bossBlood.texture = App->tex->Load("graphics/zerg/boss/boss_blood.png");
	bossBlood.position = { 0, 0, 128, 128 };
	bossBlood.section = { 0, 0, 128, 128 };

	bossCloud.texture = App->tex->Load("graphics/zerg/boss/boss_cloud.png");
	bossCloud.position = { 0, 0, 251, 186 };
	bossCloud.section = { 0, 0, 251, 186 };

	bossAcid.texture = App->tex->Load("graphics/zerg/boss/boss_acid.png");
	bossAcid.position = { 0, 0, 50, 60 };
	bossAcid.section = { 0, 0, 50, 60 };

	bossPoison.texture = App->tex->Load("graphics/zerg/boss/boss_poison.png");
	bossPoison.position = { 0, 0, 56, 56 };
	bossPoison.section = { 0, 0, 56, 56 };




	//spinSystem
	float factor = (float)M_PI / 180.0f;
	float t = 0.0f;
	spinSystem.PushExplosion(0.0f, { 0, 0 }, 110, 0, 1, 5.0f, PLAYER, true, EXPLOSION_NONE);
	for (int n = 45; n <= 360; n += 45)
	{
		spinSystem.PushExplosion(5.0f + t, {  /*radius*/(int)(60 * cos(n * factor)), /*radius*/(int)(60 * sin(n * factor)) }, 30, 200, 1, 0.25f, PLAYER, false, EXPLOSION_BLOOD);
		t += 0.15;
	}
	spinSystem.PushExplosion(5.25f, { 0, 0 }, 220, 0, 1, 5.0f, PLAYER, true, EXPLOSION_NONE, 110);
	t = 0.0f;
	for (int n = 45; n <= 360; n += 45)
	{
		spinSystem.PushExplosion(10.25f + t, {  /*radius*/(int)(140 * cos(n * factor)), /*radius*/(int)(140 * sin(n * factor)) }, 60, 200, 1, 0.25f, PLAYER, false, EXPLOSION_BLOOD);
		t += 0.15;
	}
	spinSystem.duration = 11.0f;

	//First round
	float del = 0.0f;
	int size = 25;
	for (int n = 0; n < 8; n++)
	{
		int x, y;
		x = rand() % 300 - 150;
		y = rand() % 300 - 150;
		testingSystem.PushExplosion(del, { x, y }, size, 45, 1, 3.0f, PLAYER, true, EXPLOSION_ACID);
		testingSystem.PushExplosion(del + 3.0f, { x, y }, size, 4, 8, 0.5f, PLAYER, false, EXPLOSION_POISON);
		del += 0.7f;
		size += 5;
	}
	testingSystem.duration = 9.0f;


	del = 0.0f;
	size = 25;
	for (int n = 0; n < 8; n++)
	{
		int x, y;
		x = rand() % 300 - 150;
		y = rand() % 300 - 150;
		testingSystem2.PushExplosion(del, { x, y }, size, 45, 1, 3.0f, PLAYER, true, EXPLOSION_ACID);
		testingSystem2.PushExplosion(del + 3.5f, { x, y }, size, 5, 8, 0.5f, PLAYER, false, EXPLOSION_POISON);

		del += 0.7f;
		size += 5;
	}
	testingSystem2.duration = 12.0f;

	for (int n = -4; n <= 4; n++)
	{
		crossSystem.PushExplosion(0.0f, { 45 * n, 0 }, 20, 80, 1, 4.0f, PLAYER, true, EXPLOSION_ACID);
		crossSystem.PushExplosion(0.0f, { 0, 35 * n }, 20, 80, 1, 4.0f, PLAYER, true, EXPLOSION_ACID);
	}
	for (int n = -4; n <= 4; n++)
	{
		crossSystem.PushExplosion(4.0f, { 37 * n, 27 * n }, 20, 80, 1, 4.0f, PLAYER, true, EXPLOSION_ACID);
		crossSystem.PushExplosion(4.0f, { 37 * n, -27 * n }, 20, 80, 1, 4.0f, PLAYER, true, EXPLOSION_ACID);
	}
	crossSystem.duration = 8.0f;

	for (int n = 72; n <= 360; n += 72)
	{
		spawnSystem.PushExplosion(2.0f + t, {  /*radius*/(int)(100 * cos(n * factor)), /*radius*/(int)(100 * sin(n * factor)) }, 30, 40, 1, 1.0f, PLAYER, true);
		t += 0.2f;
	}
	spawnSystem.SetSpawningUnit(ZERGLING);
	spawnSystem.duration = 4.0f;

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
				stencil.position = { it->position.x - r, it->position.y - r, r * 2 + it->innerRadius, r * 2 + it->innerRadius };
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
					while (buildIt != App->entityManager->buildingList.end())
					{
						if ((*buildIt)->stats.player == it->objective || it->objective == CINEMATIC)
						{
							bool hit = false;
							SDL_Rect collider = (*buildIt)->GetCollider();
							for (int y = 0; y < 2 && !hit; y++)
							{
								for (int x = 0; x < 2 && !hit; x++)
								{
									fPoint pos((float)(collider.x + collider.w *x), (float)(collider.y + collider.h * y));
									if (pos.DistanceNoSqrt(center) < it->radius * it->radius)
									{
										(*buildIt)->Hit(it->damage);
										hit = true;
									}
								}

							}

						}
						buildIt++;
					}
				}

				switch (it->graphic)
				{
				case (EXPLOSION_PSIONIC_STORM):
				{
					psiStorm.position.x = it->position.x - it->radius;
					psiStorm.position.y = it->position.y - it->radius;
					psiStorm.position.w = it->radius * 2;
					psiStorm.position.h = it->radius * 2;
					App->particles->AddParticle(psiStorm, 14, 0.1f);
					break;
				}
				case (EXPLOSION_TERRAN) :
				{
					terranExplosion.position.x = it->position.x - it->radius;
					terranExplosion.position.y = it->position.y - it->radius;
					terranExplosion.position.w = it->radius * 2;
					terranExplosion.position.h = it->radius * 2;
					App->particles->AddParticle(terranExplosion, 6, 0.1f);
					break;
				}
				case (EXPLOSION_GAS) :
				{
					swarmExplosion.position.x = it->position.x - it->radius;
					swarmExplosion.position.y = it->position.y - it->radius;
					swarmExplosion.position.w = it->radius * 2;
					swarmExplosion.position.h = it->radius * 2;
					App->particles->AddParticle(swarmExplosion, 10, 0.15f, 4, 4);
					break;
				}
				case (EXPLOSION_BLOOD) :
				{
					bossBlood.position.x = it->position.x - it->radius;
					bossBlood.position.y = it->position.y - it->radius;
					bossBlood.position.w = it->radius * 2;
					bossBlood.position.h = it->radius * 2;
					App->particles->AddParticle(bossBlood, 14, 0.1f);
					break;
				}
				case (EXPLOSION_CLOUD) :
				{
					bossCloud.position.x = it->position.x - it->radius;
					bossCloud.position.y = it->position.y - it->radius;
					bossCloud.position.w = it->radius * 2;
					bossCloud.position.h = it->radius * 2;
					App->particles->AddParticle(bossCloud, 10, 0.1f);
					break;
				}
				case (EXPLOSION_ACID) :
				{
					bossAcid.position.x = it->position.x - it->radius;
					bossAcid.position.y = it->position.y - it->radius;
					bossAcid.position.w = it->radius * 2;
					bossAcid.position.h = it->radius * 2;
					App->particles->AddParticle(bossAcid, 10, 0.05f);
					break;
				}
				case (EXPLOSION_POISON) :
				{
					bossPoison.position.x = it->position.x - it->radius;
					bossPoison.position.y = it->position.y - it->radius;
					bossPoison.position.w = it->radius * 2;
					bossPoison.position.h = it->radius * 2;
					App->particles->AddParticle(bossPoison, 4, 0.15f);
					break;
				}
				case(EXPLOSION_NONE) :
				{
					break;
				}
				default:
				{
					hugeExplosion.position.x = it->position.x - it->radius;
					hugeExplosion.position.y = it->position.y - it->radius;
					hugeExplosion.position.w = it->radius * 2;
					hugeExplosion.position.h = it->radius * 2;
					App->particles->AddParticle(hugeExplosion, 14, 0.06f);
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

	App->tex->UnLoad(stencil.texture);

	App->tex->UnLoad(terranExplosion.texture);
	App->tex->UnLoad(swarmExplosion.texture);
	App->tex->UnLoad(psiStorm.texture);
	App->tex->UnLoad(bossBlood.texture);
	App->tex->UnLoad(bossCloud.texture);
	App->tex->UnLoad(bossAcid.texture);
	App->tex->UnLoad(bossPoison.texture);

	return true;
}


void M_Explosion::AddExplosion(iPoint position, int radius, int damage, float delay, int nTicks, Player_Type objective, e_Explosion_Types graphic, bool showStencil, int innerRadius)
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
	toPush.innerRadius = innerRadius;

	explosions.push_back(toPush);
}

void M_Explosion::AddSystem(ExplosionSystem toPush, iPoint pos)
{
	toPush.position = pos;
	explosionSystems.push_back(toPush);
}