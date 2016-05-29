#include <stdlib.h>

#include "Boss.h"
#include "Entity.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_EntityManager.h"

#include "M_PathFinding.h"

#include "M_Map.h"
#include "M_Explosion.h"
#include "M_Particles.h"
#include "Building.h"
#include "M_Audio.h"

Boss::Boss() : Unit()
{

}

Boss::Boss(float x, float y, Unit_Type _type, Player_Type owner) : Unit(x, y, _type, owner)
{
	consumption.texture = App->tex->Load("graphics/zerg/boss/boss_consumption.png");
	consumption.position = { 0, 0, 71, 67 };
	consumption.section = { 0, 0, 71, 67 };

	char* number = new char[10];
	for (int n = 0; ; n++)
	{
		sprintf_s(number, CHAR_BIT, "%i", n + 1);
		C_String path = "sounds/zerg/units/kerrigan/att/";
		path += number;
		path += ".ogg";
		uint toPush = App->audio->LoadFx(path.GetString());
		if (toPush == 0)
		{
			break;
		}
		else
		{
			attackSounds.push_back(toPush);
		}
	}

	for (int n = 0;; n++)
	{
		sprintf_s(number, CHAR_BIT, "%i", n + 1);
		C_String path = "sounds/zerg/units/kerrigan/stun/";
		path += number;
		path += ".ogg";
		uint toPush = App->audio->LoadFx(path.GetString());
		if (toPush == 0)
		{
			break;
		}
		else
		{
			stunSounds.push_back(toPush);
		}
	}
	delete[] number;
	sfx_consumption = App->audio->LoadFx("sounds/zerg/units/kerrigan/boss_consume.ogg");
}

Boss::~Boss()
{
	App->tex->UnLoad(consumption.texture);
}

bool Boss::Update(float dt)
{
	bool ret = true;
	bool collided = false;

	if (waitingForPath && state != STATE_BOSS_STUNNED)
	{
		if (!path.empty())
		{
			GetNewTarget();
			waitingForPath = false;
		}
	}

	if (spawn_explosion == true && waitingForPath == false)
	{

		Stun();
	}

	//Kerrigan Spell - Explosive Mutation
	if (stats.shield <= 1 && state != STATE_BOSS_STUNNED && state != STATE_DIE &&  state != STATE_BOSS_EXPLOSION && waitingForPath == false)
	{
		Stun();
	}

	//General state machine
	if (movement_state == MOVEMENT_WAIT)
	{
		switch (state)
		{
		case (STATE_STAND) :
		{
			Stop();
			MoveToSample();
			break;
		}
		case(STATE_MOVE) :
		{
			Stop();
			MoveToSample();
			break;
		}
		case(STATE_ATTACK) :
		{
			UpdateAttackState(dt);
			break;
		}
		}
	}
		//Movement state machine
		switch (movement_state)
		{
		case(MOVEMENT_IDLE) :
		{
//			MoveToSample();
			break;
		}
		case (MOVEMENT_MOVE) :
		{
			UpdateMovement(dt);
			break;
		}
		case (MOVEMENT_ATTACK_IDLE) :
		{
			UpdateAttack(dt);
			break;
		}
		case (MOVEMENT_ATTACK_ATTACK) :
		{
			UpdateAttack(dt);
			break;
		}
		case(MOVEMENT_BOSS_STUNNED) :
		{
			UpdateStun();
			break;
		}
		case(MOVEMENT_BOSS_EXPLODING) :
		{
			UpdateExplosion();
			break;
		}
		case (MOVEMENT_DIE) :
		{
			UpdateDeath();
			break;
		}
		case (MOVEMENT_DEAD) :
		{
			ret = EraseUnit();
			break;
		}
		}

	if (state != STATE_DIE)
	{
		if (state != STATE_BOSS_EXPLOSION && state != STATE_BOSS_STUNNED && movement_state != MOVEMENT_WAIT)
		{
			if (explosionSpaceTimer.ReadSec() >= explosion_space)
			{
				Explode();
			}
		}

		CheckMouseHover();
	}
	if (animation.sprite.texture)
	{
		Draw(dt);
	}

	return ret;
}

void Boss::Stop()
{
	movement_state = MOVEMENT_IDLE;
	state = STATE_STAND;
	attackState = ATTACK_ATTACK;
	attackingBuilding = NULL;
	path.clear();
	App->entityManager->UpdateCurrentFrame(this);
}

void Boss::Stun()
{
	App->render->ShakeCamera(40, 6, 40);
	Stop();
	stunnedTimer.Start();
	explosionTimer.Stop();
	state = STATE_BOSS_STUNNED;
	movement_state = MOVEMENT_BOSS_STUNNED;
	attackState = ATTACK_STAND;
	if (spawn_explosion)
		SpawningExplosion();
	else
		ExplosiveMutation();
}

void Boss::UpdateStun()
{
	int tmp = stunnedTimer.ReadSec();
	if (tmp >= stun_time)
	{
		stunnedTimer.Stop();
		stats.shield = stats.maxShield;
		Stop();
		MoveToSample();
		explosionSpaceTimer.Start();

		stun_time = 23;

		LOG("Stun finished");
	}
}

void Boss::ExplosiveMutation()
{
	PlayStunSound();
	App->explosion->AddExplosion({ (int)position.x, (int)position.y }, 350, 500, 20.0f, 1, PLAYER, EXPLOSION_CLOUD, false, 0.0f, true, E_LOAD_CLOUD);
}

void Boss::SpawningExplosion()
{
	App->explosion->AddExplosion({ (int)position.x, (int)position.y }, 200, 10, 9.5f, 1, CINEMATIC, EXPLOSION_BLOOD, false, 0.0f, true, E_LOAD_CLOUD);
	spawn_explosion = false;
}

void Boss::Explode()
{
	Stop();
	state = STATE_BOSS_EXPLOSION;
	movement_state = MOVEMENT_BOSS_EXPLODING;
	attackState = ATTACK_STAND;
	PlayAttackSound();
#pragma region RandomParticles
	int r = rand() % 5;
	switch (r)
	{
	case 0:
	{
		App->explosion->AddSystem(App->explosion->testingSystem, { (int)round(position.x), (int)round(position.y) });
		explosion_time = App->explosion->testingSystem.duration - 2;
		break;
	}
	case 1:
	{
		App->explosion->AddSystem(App->explosion->testingSystem2, { (int)round(position.x), (int)round(position.y) });
		explosion_time = App->explosion->testingSystem2.duration - 2;
		break;
	}
	case 2:
	{
		App->explosion->AddSystem(App->explosion->spinSystem, { (int)round(position.x), (int)round(position.y) });
		explosion_time = App->explosion->spinSystem.duration;
		break;
	}
	case 3:
	{
		App->explosion->AddSystem(App->explosion->crossSystem, { (int)round(position.x), (int)round(position.y) });
		explosion_time = App->explosion->crossSystem.duration;
		break;
	}
	case 4:
	{
		App->explosion->AddSystem(App->explosion->spawnSystem, { (int)round(position.x), (int)round(position.y) });
		explosion_time = App->explosion->crossSystem.duration;
		break;
	}
	}
#pragma endregion
	explosionTimer.Start();
}

void Boss::UpdateExplosion()
{
	if (explosionTimer.ReadSec() >= explosion_time)
	{
		LOG("Explosion finished");
		explosionTimer.Stop();
		explosionSpaceTimer.Start();

		Stop();
		MoveToSample();
	}
}

void Boss::Attack()
{
	if (attackingBuilding)
	{
		LookAt(attackingBuilding);
	}

	if (attackingBuilding && attackingBuilding->state != BS_DEAD)
	{
		if (attackingBuilding->GetType() == ZERG_SAMPLE)
		{
			if (!IsInRange(attackingBuilding))
			{
				if (App->entityManager->debug)
				{
					LOG("Building out of range!");
				}
				Stop();
				MoveToSample();
			}
			else
			{
				attackingBuilding->Hit(stats.attackDmg);
				attackTimer.Start();
				movement_state = MOVEMENT_ATTACK_ATTACK;
				UpdateSpriteState();
			}
		}
		//Kerrigan Spell - Structure Consumption
		else
		{
			if (!IsInRange(attackingBuilding))
			{
				if (App->entityManager->debug)
				{
					LOG("Building out of range!");
				}
				Stop();
				MoveToSample();
			}
			else
			{
				consumption.position.x = attackingBuilding->GetCollider().x + attackingBuilding->GetCollider().w / 2 - consumption.position.w / 2;
				consumption.position.y = attackingBuilding->GetCollider().y + attackingBuilding->GetCollider().h / 2 - consumption.position.h / 2;
				App->particles->AddParticle(consumption, 18, 0.1f);
				App->audio->PlayFx(sfx_consumption);

				stats.shield += attackingBuilding->stats.shield;
				attackingBuilding->stats.shield = 0;
				attackingBuilding->Hit(1500);
				attackTimer.Start();
			}
		}
	}
	else
	{
		Stop();
		attackTimer.Start();
		MoveToSample();
	}
}

void Boss::MoveToSample()
{
	Move(iPoint(28, 159), ATTACK_ATTACK, PRIORITY_MEDIUM);
}

void Boss::StartDeath()
{
	Stop();
	if (selected)
	{
		App->entityManager->UnselectUnit(this);
	}
	movement_state = MOVEMENT_DIE;
	state = STATE_DIE;
	HPBar->SetActive(false);
	logicTimer.Start();
	actionTimer.Start();
	App->entityManager->UpdateCurrentFrame(this);
	App->gui->DeleteUIElement(HPBar);
}

void Boss::PlayAttackSound()
{
	if (attackSounds.empty() == false)
	{
		uint n = rand() % 100 + 1;
		if (n <= 70)
		{
			uint toPlay = rand() % attackSounds.size();
			App->audio->PlayFx(attackSounds[toPlay]);
		}
	}
}

void Boss::PlayStunSound()
{
	if (stunSounds.empty() == false)
	{
		uint toPlay = rand() % stunSounds.size();
		App->audio->PlayFx(stunSounds[toPlay]);
	}
}