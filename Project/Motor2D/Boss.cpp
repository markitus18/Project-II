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

Boss::~Boss()
{

}

bool Boss::Start()
{

	movement_state = MOVEMENT_IDLE;
	in_combatTimer.Start();
	shieldTimer.Start();

	App->entityManager->UpdateCurrentFrame(this);

	return true;
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
			ret = UpdateDeath(dt);
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

		if (bossState != STATE_DIE)
		{
			RegenShield();
			CheckMouseHover();
		}
		if (animation.sprite)
		{
			Draw(dt);
		}
		return ret;
	}
}