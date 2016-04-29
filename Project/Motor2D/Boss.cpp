#include <stdlib.h>

#include "Boss.h"
#include "Entity.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_EntityManager.h"
#include "UI_Element.h"
#include "M_PathFinding.h"
#include "Building.h"
#include "S_SceneMap.h"
#include "M_GUI.h"
#include "M_Input.h"
#include "Intersections.h"
#include "M_Map.h"
#include "Building.h"

Boss::Boss() : Unit()
{

}

Boss::Boss(float x, float y, Unit_Type _type, Player_Type owner) : Unit(x, y, _type, owner)
{

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

	//General state machine
	if (movement_state == MOVEMENT_WAIT)
	{
		switch (bossState)
		{
		case (BOSS_STAND) :
		{
							  Stop();
							  break;
		}
		case(BOSS_MOVE) :
		{
							Stop();
							break;
		}
		case(BOSS_ATTACK) :
		{
							  UpdateAttackState(dt);
							  break;
		}
		}
	}
		//Movement state machine
	switch (movement_state)
	{
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

		/*if (state != STATE_GATHER && state != STATE_GATHER_RETURN && state != STATE_MOVE && state != STATE_STAND)
		{
		if (gatheringResource)
		{
		if (gatheringResource->gatheringUnit)
		{
		gatheringResource->gatheringUnit = NULL;
		}
		gatheringResource = NULL;
		}
		}*/

	if (bossState != BOSS_DIE)
	{
		RegenShield();
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
	float time = actionTimer.ReadSec();
	C_Vec2<float> vector;
	if (attackingBuilding)
	{
		iPoint buildingPos = App->pathFinding->MapToWorld(attackingBuilding->GetPosition().x, attackingBuilding->GetPosition().y);
		vector.x = buildingPos.x - position.x;
		vector.y = buildingPos.y - position.y;
	}
	currentVelocity.SetAngle(vector.GetAngle());

	if (attackingBuilding && attackingBuilding->state != BS_DEAD)
	{
		if (!IsInRange(attackingBuilding))
		{
			if (App->entityManager->debug)
			{
				LOG("Kerrigan: Zerg Sample out of range!");
			}
			movement_state = MOVEMENT_WAIT;
		}
	}
	else
	{
		Stop();
	}

	if (attackingBuilding && attackingBuilding->state != BS_DEAD)
	{
		in_combatTimer.Start();
		shieldTimer.Start();
		movement_state = MOVEMENT_ATTACK_ATTACK;

		attackingBuilding->Hit(stats.attackDmg);
		App->entityManager->UpdateCurrentFrame(this);
	}
	else
	{
		Stop();
	}
}

void Boss::Stop()
{
	bossState = BOSS_STAND;
	movement_state = MOVEMENT_IDLE;
	bossAtkState = BOSS_ATK_ATTACK;
	attackingBuilding = NULL;
	attackingUnit = NULL;
	path.clear();
	App->entityManager->UpdateCurrentFrame(this);
}

Boss_State Boss::GetState() const
{
	return bossState;
}

Boss_Attack_State Boss::GetAttackState() const
{
	return bossAtkState;
}

{
	if (unit->GetState() != STATE_DIE)
	{
		attackingUnit = unit;
		attackingBuilding = NULL;
		actionTimer.Start();
		bossState = BOSS_ATTACK;
		movement_state = MOVEMENT_ATTACK_IDLE;
		bossAtkState = BOSS_ATK_STAND;
		App->entityManager->UpdateCurrentFrame(this);
	}
	else
	{
		Stop();
	}

}

void Boss::SetAttack(Building* building)
{
	attackingBuilding = building;
	attackingUnit = NULL;
	actionTimer.Start();
	bossState = BOSS_ATTACK;
	movement_state = MOVEMENT_ATTACK_IDLE;
	bossAtkState = BOSS_ATK_STAND;
	App->entityManager->UpdateCurrentFrame(this);

void Boss::StartDeath()
{
	Stop();
	if (selected)
	{
		App->entityManager->UnselectUnit(this);
	}
	movement_state = MOVEMENT_DIE;
	bossState = BOSS_DIE;
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
	HPBar_Shield->SetActive(false);
	logicTimer.Start();
	actionTimer.Start();
	App->entityManager->UpdateCurrentFrame(this);
}