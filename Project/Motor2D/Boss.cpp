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

Boss::Boss() : Unit()
{

}

Boss::Boss(float x, float y, Unit_Type _type, Player_Type owner) : Unit(x, y, _type, owner)
{
	consumption.texture = App->tex->Load("graphics/zerg/boss/boss_consumption.png");
	consumption.position = { 0, 0, 71, 67 };
	consumption.section = { 0, 0, 71, 67 };

}

Boss::~Boss()
{

}

bool Boss::Update(float dt)
{
	bool ret = true;
	bool collided = false;

	if (waitingForPath)
	{
		if (!path.empty())
		{
			GetNewTarget();
			waitingForPath = false;
		}
	}

	//Kerrigan Spell - Explosive Mutation
	if (stats.shield <= 1 && state != STATE_BOSS_STUNNED && state != STATE_DIE)
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
		MoveToSample();
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
		if (state != STATE_BOSS_EXPLOSION && state != STATE_BOSS_STUNNED)
		{
			if (explosionTimer.ReadSec() >= 15)
			{
				Stop();
				state = STATE_BOSS_EXPLOSION;
				movement_state = MOVEMENT_BOSS_EXPLODING;
#pragma region RandomParticles
				int r = rand() % 5;
				switch (r)
				{
				case 0:
				{
					App->explosion->AddSystem(App->explosion->testingSystem, { (int)round(position.x), (int)round(position.y) });
					explosion_time = App->explosion->testingSystem.duration;
					break;
				}
				case 1:
				{
					App->explosion->AddSystem(App->explosion->testingSystem2, { (int)round(position.x), (int)round(position.y) });
					explosion_time = App->explosion->testingSystem2.duration;
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
		}

		CheckMouseHover();
	}
	if (animation.sprite.texture)
	{
		Draw(dt);
	}

	return ret;
}

void Boss::UpdateAttack(float dt)
{
	explosionTimer.Start();

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
				movement_state = MOVEMENT_WAIT;
			}
			else if (attackingBuilding->GetType() == ZERG_SAMPLE && basicAttackTimer.IsStopped())
			{
				attackingBuilding->Hit(stats.attackDmg);
				basicAttackTimer.Start();
			}
			if (attackingBuilding->GetType() == ZERG_SAMPLE && basicAttackTimer.ReadSec() >= ((float)stats.attackSpeed * 3.0f / 4.0f))
			{
				attackingBuilding->Hit(stats.attackDmg);
				basicAttackTimer.Start();
			}
		}
		//Kerrigan Spell - Structure Consumption
		else if (attackingBuilding->GetType() != ZERG_SAMPLE)
		{
			if (!IsInRange(attackingBuilding))
			{
				if (App->entityManager->debug)
				{
					LOG("Building out of range!");
				}
				movement_state = MOVEMENT_WAIT;
			}
			else if (basicAttackTimer.IsStopped()) //BasicAtkTimer will be recycled
			{
				basicAttackTimer.Start();
			}
			else if (basicAttackTimer.ReadSec() >= ((float)stats.attackSpeed * 3.0f / 4.0f))
			{
				consumption.position.x = attackingBuilding->GetWorldPosition().x;
				consumption.position.y = attackingBuilding->GetWorldPosition().y;
				consumption.position.w = 71;
				consumption.position.h = 67;
				App->particles->AddParticle(consumption, 18, 0.1f);

				stats.shield += attackingBuilding->stats.shield;
				attackingBuilding->stats.shield = 0;
				attackingBuilding->Hit(1500);
				basicAttackTimer.Start();
			}
		}
	}
	else
	{
		Stop();
		basicAttackTimer.Stop();
	}
}

void Boss::Stop()
{
	movement_state = MOVEMENT_IDLE;
	state = STATE_STAND;
	attackState = ATTACK_ATTACK;
	attackingBuilding = NULL;
	attackingUnit = NULL;
	path.clear();
	App->entityManager->UpdateCurrentFrame(this);
}

void Boss::Stun()
{
	Stop();
	stunnedTimer.Start();
	explosionTimer.Stop();
	state = STATE_BOSS_STUNNED;
	movement_state = MOVEMENT_BOSS_STUNNED;
	attackState = ATTACK_STAND;
	App->explosion->AddExplosion({ (int)position.x, (int)position.y }, 350, 300, 20.0f, 1, PLAYER, EXPLOSION_CLOUD);
}

void Boss::UpdateStun()
{
	if (stunnedTimer.ReadSec() >= stun_time)
	{
		stunnedTimer.Stop();
		stats.shield = stats.maxShield;
		Stop();
		MoveToSample();
		explosionTimer.Start();
		LOG("Stun finished");
	}
}

void Boss::UpdateExplosion()
{
	if (explosionTimer.ReadSec() >= explosion_time)
	{
		explosionTimer.Start();
		Stop();
		MoveToSample();
		LOG("Explosion finished");
	}
}

void Boss::MoveToSample()
{
	Move(iPoint(28, 159), ATTACK_ATTACK, PRIORITY_MEDIUM);
}

void Boss::SetAttack(Building* building)
{
	attackingBuilding = building;
	attackingUnit = NULL;
	actionTimer.Start();
	movement_state = MOVEMENT_ATTACK_IDLE;
	state = STATE_ATTACK;
	attackState = ATTACK_STAND;
	App->entityManager->UpdateCurrentFrame(this);
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
}