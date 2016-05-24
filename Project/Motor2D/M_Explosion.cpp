#include "M_Explosion.h"

#include "j1App.h"
#include "M_EntityManager.h"

#include "M_Render.h"

#include "Unit.h"
#include "Building.h"
#include "Boss.h"

#include "M_Particles.h"
#include "M_Audio.h"

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


void ExplosionSystem::PushExplosion(float delay, iPoint relativePos, int radius, int damage, int nTicks, float tickDelay, Player_Type objective, bool showStencil, e_Explosion_Types graphic, float innerRadius, bool shake, e_load_graphic load)
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
	toPush.second.shake = shake;
	toPush.second.load = load;
	
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
					App->explosion->AddExplosion(position + it->second.position, it->second.radius, it->second.damage, it->second.tickDelay, it->second.nTicks, it->second.objective, it->second.graphic, it->second.showStencil, it->second.innerRadius, it->second.shake, it->second.load);
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

bool M_Explosion::Awake(pugi::xml_node&)
{
	//spinSystem
	//spinSystem.sprite.texture = App->tex->Load("graphics/zerg/boss/boss_blood_load.png");

	float factor = (float)M_PI / 180.0f;
	float t = 0.0f;
	spinSystem.PushExplosion(0.0f, { 0, 0 }, 110, 0, 1, 5.0f, PLAYER, true, EXPLOSION_NONE, 0.0f, true);
	for (int n = 45; n <= 360; n += 45)
	{
		spinSystem.PushExplosion(5.0f + t, { (int)(60 * cos(n * factor)), (int)(60 * sin(n * factor)) }, 30, 100, 1, 0.25f, PLAYER, true, EXPLOSION_BLOOD);
		t += 0.15;
	}
	spinSystem.PushExplosion(0.0f, { 0, 0 }, 220, 0, 1, 10.25f, PLAYER, true, EXPLOSION_NONE, 5.0f, true, E_LOAD_SPIN);
	t = 0.0f;
	for (int n = 45; n <= 360; n += 45)
	{
		spinSystem.PushExplosion(10.25f + t, {  /*radius*/(int)(140 * cos(n * factor)), /*radius*/(int)(140 * sin(n * factor)) }, 60, 120, 1, 0.25f, PLAYER, true, EXPLOSION_BLOOD);
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
		testingSystem.PushExplosion(del, { x, y }, size, 45, 1, 3.0f, PLAYER, true, EXPLOSION_ACID, 0.0f, false, E_LOAD_TEST);
		testingSystem.PushExplosion(del + 2.5f, { x, y }, size, 4, 11, 0.5f, PLAYER, true, EXPLOSION_POISON);
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
		testingSystem2.PushExplosion(del, { x, y }, size, 45, 1, 3.0f, PLAYER, true, EXPLOSION_ACID, 0.0f, false, E_LOAD_TEST);
		testingSystem2.PushExplosion(del + 2.5f, { x, y }, size, 5, 8, 0.5f, PLAYER, true, EXPLOSION_POISON);

		del += 0.7f;
		size += 5;
	}
	testingSystem2.duration = 12.0f;

	crossSystem.PushExplosion(0.0f, { 0, 0 }, 200, 0, 1, 4.0f, PLAYER, false, EXPLOSION_NONE, 0.0f, false, E_LOAD_CROSS1);
	for (int n = -4; n <= 4; n++)
	{
		crossSystem.PushExplosion(0.0f, { 45 * n, 0 }, 20, 80, 1, 4.0f, PLAYER, false, EXPLOSION_ACID);
		crossSystem.PushExplosion(0.0f, { 0, 35 * n }, 20, 80, 1, 4.0f, PLAYER, false, EXPLOSION_ACID);
	}
	crossSystem.PushExplosion(4.0f, { 0, 0 }, 200, 0, 1, 4.0f, PLAYER, false, EXPLOSION_NONE, 0.0f, false, E_LOAD_CROSS2);
	for (int n = -4; n <= 4; n++)
	{
		crossSystem.PushExplosion(4.0f, { 37 * n, 27 * n }, 20, 80, 1, 4.0f, PLAYER, false, EXPLOSION_ACID);
		crossSystem.PushExplosion(4.0f, { 37 * n, -27 * n }, 20, 80, 1, 4.0f, PLAYER, false, EXPLOSION_ACID);
	}
	crossSystem.duration = 8.0f;

	for (int n = 72; n <= 360; n += 72)
	{
		spawnSystem.PushExplosion(1.0f + t, {  /*radius*/(int)(100 * cos(n * factor)), /*radius*/(int)(100 * sin(n * factor)) }, 30, 40, 1, 1.0f, PLAYER, true);
		t += 0.2f;
	}
	spawnSystem.SetSpawningUnit(ZERGLING);
	spawnSystem.duration = 2.0f;

	return true;
}

bool M_Explosion::Start()
{
	testLoad = App->tex->Load("graphics/zerg/boss/boss_poison_load.png");
	cloudLoad = App->tex->Load("graphics/zerg/boss/boss_cloud_load.png");
	spinLoad = App->tex->Load("graphics/zerg/boss/boss_blood_load.png");
	cross1 = App->tex->Load("graphics/zerg/boss/boss_acid_load.png");
	cross2 = App->tex->Load("graphics/zerg/boss/boss_acid_load2.png");

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

	sfx_explosion1 = App->audio->LoadFx("sounds/zerg/units/kerrigan/boss_raw_explosion.ogg");
	sfx_explosion2 = App->audio->LoadFx("sounds/zerg/units/kerrigan/boss_raw_explosion2.ogg");
	sfx_poison = App->audio->LoadFx("sounds/zerg/units/kerrigan/boss_raw_poison.ogg");

	ClearExplosions();

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
			if (it->sprite.texture != NULL)
			{
#pragma region //Drawing sprite
				it->sprite.position.x = it->position.x - it->radius;
				it->sprite.position.y = it->position.y - it->radius;
				it->sprite.position.w = it->sprite.position.h = it->radius * 2;
				if (it->sectionChange == false)
				{
					it->spriteTimer += dt;
					if (it->spriteTimer >= it->timePerFrame)
					{
						it->sprite.section.y += it->sprite.section.h;
						it->spriteTimer = 0.0f;
					}
				}
				App->render->AddSprite(&it->sprite, DECAL);
				if (it->sectionChange)
				{
					C_Sprite tmp;
					tmp.texture = it->sprite.texture;

					int r = it->sprite.position.h/2 * (it->timer / it->tickDelay);
					int r2 = it->sprite.section.h/2 * (it->timer / it->tickDelay);
					CAP(r, 1, INT_MAX);
					tmp.position = { it->sprite.position.x + it->sprite.position.w / 2 - r, it->sprite.position.y + it->sprite.position.h / 2 - r, r * 2, r * 2 };

					tmp.section = { it->sprite.section.w / 2 - r2, it->sprite.section.h + it->sprite.section.h/2 - r2, r2*2, r2*2 };
		
					App->render->AddSprite(&tmp, DECAL);
				}
#pragma endregion
			}
			
			if (it->showStencil)
			{
#pragma region //drawingStencil
				if (it->innerRadius == 0.0f)
				{
					//if (it->timer / it->tickDelay < 0.45f)
					//{
					//	stencil.texture = green;
					//}
					//else if (it->timer / it->tickDelay < 0.75f)
					//{
					//	stencil.texture = yellow;
					//}
					//else
					//{
						stencil.texture = red;
					//}
				}
				else
				{
					//if (it->timer / it->tickDelay < 0.72f)
					//{
					//	stencil.texture = green;
					//}
					//else if (it->timer / it->tickDelay < 0.87f)
					//{
					//	stencil.texture = yellow;
					//}
					//else
					//{
						stencil.texture = red;
					//}
				}
				//int r = it->radius * (it->timer / it->tickDelay);
				int s = it->radius * (it->innerRadius / it->tickDelay);
				//CAP(r, 1, INT_MAX);
				if (it->innerRadius <= it->timer)
				{
				//	stencil.position = { it->position.x - r, it->position.y - r, r * 2, r * 2};
				//	App->render->AddSprite(&stencil, DECAL);
					stencil.position = { it->position.x - it->radius, it->position.y - it->radius, it->radius * 2, it->radius * 2 };
					App->render->AddSprite(&stencil, DECAL);

					if (it->innerRadius != 0)
					{
						stencil.position = { it->position.x - s, it->position.y - s, s * 2, s * 2 };
						App->render->AddSprite(&stencil, DECAL);
					}
				}
#pragma endregion
			}

			if (it->Fuse(dt))
			{
#pragma region //exploding

				if (it->currentTick == 1 && it->shake)
				{
					if (it->position.x >= App->render->camera.x / 2 && it->position.y >= App->render->camera.y / 2 &&
						it->position.x <= App->render->camera.x / 2 + App->render->camera.w * 2 && it->position.y <= App->render->camera.y / 2 + App->render->camera.h * 2
						)
					{
						int radius = ceil(it->radius / 12);
						CAP(radius, 1, 30);
						App->render->ShakeCamera(radius, 5, 20);
					}
				}

				fPoint center = { (float)it->position.x, (float)it->position.y };
				if (App->entityManager->boss && !App->entityManager->boss->dead && App->entityManager->boss->active)
				{
					Boss* boss = App->entityManager->boss;
					if (boss->stats.player == it->objective || it->objective == CINEMATIC)
					{
						if (boss->GetPosition().DistanceNoSqrt(center) < it->radius * it->radius)
						{
							boss->Hit(it->damage);
						}
					}
				}
				if (App->entityManager->unitList.empty() == false)
				{
					for (int i = 0; i < App->entityManager->unitList.size(); i++)
					{
						if (App->entityManager->unitList[i].dead == false)
						{
							Unit* unit = &App->entityManager->unitList[i];
							if (unit->stats.player == it->objective || it->objective == CINEMATIC)
							{
								if (unit->GetPosition().DistanceNoSqrt(center) < it->radius * it->radius)
								{
									unit->Hit(it->damage);
								}
							}
						}
					}
				}

				if (App->entityManager->buildingList.empty() == false)
				{
					for (int i = 0; i < App->entityManager->buildingList.size(); i++)
					{
						if (App->entityManager->buildingList[i].dead == false)
						{
							Building* building = &App->entityManager->buildingList[i];
							if (building->stats.player == it->objective || it->objective == CINEMATIC)
							{
								bool hit = false;
								SDL_Rect collider = building->GetCollider();
								for (int y = 0; y < 2 && !hit; y++)
								{
									for (int x = 0; x < 2 && !hit; x++)
									{
										fPoint pos((float)(collider.x + collider.w *x), (float)(collider.y + collider.h * y));
										if (pos.DistanceNoSqrt(center) < it->radius * it->radius)
										{
											building->Hit(it->damage);
											hit = true;
										}
									}

								}

							}
						}
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
	App->tex->UnLoad(testLoad);
	App->tex->UnLoad(cloudLoad);
	App->tex->UnLoad(spinLoad);
	App->tex->UnLoad(cross1);
	App->tex->UnLoad(cross2);

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

	explosions.clear();
	explosionSystems.clear();

	return true;
}


// Load Game State
bool M_Explosion::Load(pugi::xml_node& data)
{

	explosions.clear();
	explosionSystems.clear();

	for (pugi::xml_node expl = data.child("explosion"); expl; expl = expl.next_sibling("explosion"))
	{
		int x = expl.attribute("x").as_int();
		int y = expl.attribute("y").as_int();

		int currentTick = expl.attribute("currentTick").as_int();
		int dmg = expl.attribute("dmg").as_int();
		float innerRadius = expl.attribute("innerRadius").as_float();
		int nTicks = expl.attribute("nTicks").as_int();
		Player_Type objective = static_cast<Player_Type>(expl.attribute("objective").as_int());
		int radius = expl.attribute("radius").as_int();
		bool showStencil = expl.attribute("showStencil").as_int();
		float tickDelay = expl.attribute("tickDelay").as_float();
		float timer = expl.attribute("timer").as_float();
		e_Explosion_Types type = static_cast<e_Explosion_Types>(expl.attribute("type").as_int());

		AddExplosion({ x, y }, radius, dmg, tickDelay, nTicks, objective, type, showStencil, innerRadius);

		explosions.back().currentTick = currentTick;
		explosions.back().timer = timer;

	}
	return true;
}

// Save Game State
bool M_Explosion::Save(pugi::xml_node& data) const
{
	for (std::list<Explosion>::const_iterator expl = explosions.cbegin(); expl != explosions.cend(); expl++)
	{
		pugi::xml_node toPush = data.append_child("explosion");
		toPush.append_attribute("x") = expl->position.x;
		toPush.append_attribute("y") = expl->position.y;

		toPush.append_attribute("currentTick") = expl->currentTick;
		toPush.append_attribute("dmg") = expl->damage;
		toPush.append_attribute("innerRadius") = expl->innerRadius;
		toPush.append_attribute("nTicks") = expl->nTicks;
		toPush.append_attribute("objective") = expl->objective;
		toPush.append_attribute("radius") = expl->radius;
		toPush.append_attribute("showStencil") = expl->showStencil;
		toPush.append_attribute("tickDelay") = expl->tickDelay;
		toPush.append_attribute("timer") = expl->timer;
		toPush.append_attribute("type") = expl->graphic;
	}

	return true;
}


void M_Explosion::AddExplosion(iPoint position, int radius, int damage, float delay, int nTicks, Player_Type objective, e_Explosion_Types graphic, bool showStencil, float innerRadius, bool shake, e_load_graphic load)
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
	toPush.shake = shake;
	toPush.sprite.useCamera = true;
	toPush.sprite.texture = NULL;
	switch (load)
	{
	case E_LOAD_TEST:
	{
		toPush.sprite.texture = testLoad;
		toPush.sprite.section = { 0, 0, 90, 90 };
		toPush.SetNFrames(4);
		break;
	}
	case E_LOAD_CLOUD:
	{
		toPush.sprite.texture = cloudLoad;
		toPush.sprite.section = { 0, 0, 350, 350 };
		toPush.SetNFrames(15);
		break;
	}
	case E_LOAD_SPIN:
	{
		toPush.sprite.texture = spinLoad;
		toPush.sprite.section = { 0, 0, 220, 220 };
		toPush.SetNFrames(10);
		break;
	}
	case E_LOAD_CROSS1:
	{
		toPush.sprite.texture = cross1;
		toPush.sprite.section = { 0, 0, 220, 220 };
		toPush.SetNFrames(10);
		break;
	}
	case E_LOAD_CROSS2:
	{
		toPush.sprite.texture = cross2;
		toPush.sprite.section = { 0, 0, 220, 220 };
		toPush.SetNFrames(10);
		break;
	}
	}

	explosions.push_back(toPush);
}

void M_Explosion::AddSystem(ExplosionSystem toPush, iPoint pos)
{
	toPush.position = pos;
	explosionSystems.push_back(toPush);
}