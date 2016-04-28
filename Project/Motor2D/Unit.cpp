#include <stdlib.h>

#include "Unit.h"
#include "Entity.h"
#include "Controlled.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_EntityManager.h"
#include "UI_Element.h"
#include "M_PathFinding.h"
#include "Resource.h"
#include "Building.h"
#include "S_SceneMap.h"
#include "M_GUI.h"
#include "M_Input.h"
#include "Intersections.h"
#include "M_Map.h"
#include "Building.h"
#include "M_FogOfWar.h"
#include "M_Explosion.h"

Unit::Unit() :Controlled()
{
	LoadLibraryData();
}

Unit::Unit(float x, float y, Unit_Type _type, Player_Type playerType) : Controlled()
{
	position = { x, y };
	stats.type = _type;
	stats.player = playerType;
	LoadLibraryData();
}
Unit::Unit(fPoint pos) : Controlled()
{
	position = pos;
	LoadLibraryData();
}

Unit::~Unit()
{

}

bool Unit::Start()
{
	currentVelocity.position = position;
	currentVelocity.y = currentVelocity.x = 1;

	currentVelocity.SetAngle(30);

	currentVelocity.Normalize();
	currentVelocity *= stats.speed;

	UpdateCollider();
	UpdateBarPosition();

	movement_state = MOVEMENT_IDLE;
	in_combatTimer.Start();
	shieldTimer.Start();

	App->entityManager->UpdateCurrentFrame(this);

	return true;
}

bool Unit::Update(float dt)
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
		switch (state)
		{
		case (STATE_STAND) :
		{
			Stop();
			break;
		}
		case(STATE_MOVE) :
		{
			Stop();
			break;
		}
		case(STATE_GATHER) :
		{
			UpdateGatherState();
			break;
		}
		case(STATE_GATHER_RETURN) :
		{
			UpdateGatherReturnState();
			break;
		}
		case(STATE_ATTACK) :
		{
			UpdateAttackState(dt);
			break;
		}
		case(STATE_BUILD) :
			UpdateBuildState();
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
	case (MOVEMENT_GATHER) :
	{
		UpdateGather(dt);
		break;
	}
	case (MOVEMENT_ATTACK_IDLE) :
	{
		UpdateAttack(dt);
		break;
	}
	case (MOVEMENT_ATTACK_ATTACK) :
	{
		//UpdateAttack(dt);
		break;
	}
	case (MOVEMENT_DIE):
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

	if (state != STATE_GATHER && state != STATE_GATHER_RETURN && state != STATE_MOVE && state != STATE_STAND)
	{
		if (gatheringResource)
		{
			if (gatheringResource->gatheringUnit)
			{
				gatheringResource->gatheringUnit = NULL;
			}
			gatheringResource = NULL;
		}
	}

	if (stats.type == PROBE)
	{
		UpdateGatherSprite();
		if (movement_state == MOVEMENT_GATHER || movement_state == MOVEMENT_ATTACK_ATTACK || movement_state == MOVEMENT_ATTACK_IDLE)
			UpdateGatherSpark(dt);
	}

	if (state != STATE_DIE)
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

void Unit::UpdateMovement(float dt)
{
	if (state == STATE_ATTACK)
	{
		if (logicTimer.ReadSec() >= 0.1)
		{
			if (attackingUnit)
			{
				if (IsInRange(attackingUnit))
				{
					movement_state = MOVEMENT_WAIT;
				}
				logicTimer.Start();
			}
			else if (attackingBuilding)
			{
				if (IsInRange(attackingBuilding))
				{
					movement_state = MOVEMENT_WAIT;
				}
				logicTimer.Start();
			}
		}
	}
	if (!targetReached)
	{
		if (!isAngleReached())
			UpdateVelocity(dt);
		else if (!UpdatePosition(dt))
		{
			targetReached = true;
		}
	}

	if (targetReached)
	{
		if (!GetNewTarget())
		{
			movement_state = MOVEMENT_WAIT;
		}
	}
	UpdateBarPosition();
}

bool Unit::UpdateVelocity(float dt)
{
	bool ret = true;
	if (stats.type != DRAGOON)
	{
		if (!isAngleReached())
		{
			Rotate(dt);
			ret = false;
		}
	}
	else
	{
		currentVelocity = desiredVelocity;
	}
	return ret;
}

//Get the desired velocity: target position - current position
void Unit::GetDesiredVelocity()
{
	C_Vec2<float> velocity;

	velocity.x = (target.x - position.x);
	velocity.y = (target.y - position.y);
	velocity.position = position;

	velocity.Normalize();
	velocity *= stats.speed;
	desiredVelocity = velocity;
}

bool Unit::UpdatePosition(float dt)
{
	bool ret = true;

	C_Vec2<float> vel = currentVelocity * dt;

	//Continuous evaluation
	if (true)
	{
		//Splitting the velocity into smaller pieces to check if the unit reaches the target
		float module = vel.GetModule();
		int steps = (int)(floor(vel.GetModule() / (targetRadius / 2)));
		C_Vec2<float> velStep = (vel.GetNormal() * (targetRadius / 2));
		C_Vec2<float> rest = vel - vel.GetNormal() * targetRadius / 2 * (float)steps;

		for (int i = 0; i < steps && ret; i++)
		{
			position.x += velStep.x;
			position.y += velStep.y;
			UpdateCollider();
			if (isTargetReached())
				ret = false;
		}
		if (ret)
		{
			position.x += rest.x;
			position.y += rest.y;
			UpdateCollider();
			if (isTargetReached())
				ret = false;
		}
	}
	//Normal movement
	else
	{
		position.x += vel.x;
		position.y += vel.y;
		if (isTargetReached())
			ret = false;
	}

	return ret;
}

bool Unit::Rotate(float dt)
{
	bool ret = true;
	int positive = 1;

	float currentAngle = currentVelocity.GetAngle();
	float desiredAngle = desiredVelocity.GetAngle();

	//Getting the direction of the rotation
	float diffAngle = abs(currentAngle - desiredAngle);
	bool currBigger = (currentAngle > desiredAngle);
	bool difBigger = (diffAngle > 180);
	if (currBigger == difBigger)
		positive = 1;
	else
		positive = -1;

	//Adding rotation angle by continuous evaluation: split the total rotation into
	//smaller rotations to check if we reach the expected direction
	float stepAngle = 4.5;
	float angle = rotationSpeed * dt;
	int steps = (int)(angle / stepAngle);
	float rest = angle - stepAngle * steps;

	for (int i = 0; i < steps && ret; i++)
	{
		currentVelocity.SetAngle(currentVelocity.GetAngle() + stepAngle * positive);
		if (isAngleReached())
			ret = false;
	}
	if (ret)
	{
		currentVelocity.SetAngle(currentVelocity.GetAngle() + stepAngle * positive);
	}
	if (isAngleReached())
		ret = false;

	return ret;
}

bool Unit::GetNewTarget()
{
	if ((uint)currentNode + 1 < path.size())
	{
		currentNode++;
		iPoint newPos = App->pathFinding->MapToWorld(path[currentNode].x, path[currentNode].y);
		newPos += {8, 8};

		SetTarget(newPos.x, newPos.y);
		return true;
	}
	else if (movement_state == MOVEMENT_MOVE)
	{
		movement_state = MOVEMENT_IDLE;
		attackState = ATTACK_ATTACK;
		App->entityManager->UpdateCurrentFrame(this);
	}

	return false;
}

bool Unit::isTargetReached()
{
	C_Vec2<float> vec;
	vec.x = target.x - position.x;
	vec.y = target.y - position.y;
	float distance = vec.GetModule();
	if (movementType == FLYING)
	{
		distance /= 4;
	}
	if (distance < targetRadius)
	{
		position.x = (float)target.x;
		position.y = (float)target.y;
		currentVelocity.position = desiredVelocity.position = position;
		return true;
	}
	return false;
}

bool Unit::isAngleReached()
{
	float diffVel = abs(currentVelocity.GetAngle() - desiredVelocity.GetAngle());
	if (diffVel < 5.0 || diffVel > 355)
	{
		currentVelocity.SetAngle(desiredVelocity.GetAngle());
		return true;
	}
	return false;
}

//Update general state
void Unit::UpdateGatherState()
{
	if (gatheringResource)
	{
		if (gatheredAmount)
		{
			ReturnResource();
		}
		else if (gatheringResource->resourceAmount != 0)
		{
			if (gatheringResource->gatheringUnit)
			{
				Resource* newResource = App->entityManager->FindClosestResource(this);
				if (newResource)
				{
					SetGathering(newResource);
				}
			}
			else
			{
				actionTimer.Start();
				movement_state = MOVEMENT_GATHER;
				attackState = ATTACK_STAND;
				LookAt(gatheringResource);
				App->entityManager->UpdateCurrentFrame(this);
				gatheringResource->gatheringUnit = this;
			}
		}
		else
		{
			if (gatheringResource = App->entityManager->FindClosestResource(this))
			{
				SetGathering(gatheringResource);
			}
			else
			{
				state = STATE_STAND;
				movement_state = MOVEMENT_IDLE;
				attackState = ATTACK_ATTACK;
				App->entityManager->UpdateCurrentFrame(this);
			}
		}
	}
	else if (gatheringBuilding)
	{
		if (!gatheredAmount)
		{
			gatheringBuilding->AskToEnter(this);
		}
		else
		{
			ReturnResource();
		}
	}
}

void Unit::UpdateGatherReturnState()
{
	if (gatheringResource)
	{
		App->sceneMap->player.mineral += gatheredAmount;
		gatheredAmount = 0;
		SetGathering(gatheringResource);
	}
	else if (gatheringBuilding)
	{
		App->sceneMap->player.gas += gatheredAmount;
		gatheredAmount = 0;
		SetGathering(gatheringBuilding);
	}
	else
	{
		state = STATE_STAND;
		movement_state = MOVEMENT_IDLE;
		attackState = ATTACK_ATTACK;
		App->entityManager->UpdateCurrentFrame(this);
	}

}

void Unit::UpdateGather(float dt)
{
	if (gatheringResource)
	{
		if (gatheringResource->resourceAmount > 0)
		{
			if (actionTimer.ReadSec() >= 3)
			{
				gatheredAmount = gatheringResource->Extract(8);
				gatheringResource->gatheringUnit = NULL;
				if (gatheredAmount < 8)
				{
					gatheredAmount = 0;
					if (gatheringResource = App->entityManager->FindClosestResource(this))
					{
						SetGathering(gatheringResource);
					}
					else
					{
						movement_state = MOVEMENT_IDLE;
						state = STATE_STAND;
						attackState = ATTACK_ATTACK;
					}
				}
				else
				{
					gatheredType = MINERAL;
					movement_state = MOVEMENT_WAIT;
				}
			}
		}
		else
		{
			if (gatheringResource = App->entityManager->FindClosestResource(this))
			{
				SetGathering(gatheringResource);
			}
			else
			{
				movement_state = MOVEMENT_IDLE;
				state = STATE_STAND;
				attackState = ATTACK_ATTACK;
			}
		}
	}
	else
	{
		movement_state = MOVEMENT_IDLE;
		state = STATE_STAND;
		attackState = ATTACK_ATTACK;
	}
}

void Unit::UpdateGatherSprite()
{
	if (gatheredAmount)
	{
		C_Vec2<float> vec = currentVelocity;
		vec.position = position;
		vec.Normalize();
		vec *= 20;
		
		switch (gatheredType)
		{
		case(MINERAL) :
			gatherSprite.texture = App->entityManager->gather_mineral_tex;
			gatherShadow.texture = App->entityManager->gather_mineral_shadow_tex;
			break;
		case(GAS) :
			gatherSprite.texture = App->entityManager->gather_gas_tex;
			gatherShadow.texture = App->entityManager->gather_gas_shadow_tex;
			if (gatheredAmount == 2)
				gatherSprite.section.y = 32;
			break;
		}
		gatherShadow.position.x = gatherSprite.position.x = vec.position.x + vec.x - 15;
		gatherShadow.position.y = gatherSprite.position.y = vec.position.y + vec.y - 15;
		gatherSprite.y_ref = gatherSprite.position.y;
		gatherShadow.y_ref = gatherSprite.y_ref - 1;
		App->entityManager->UpdateSpriteRect(this, gatherSprite, 0);
		gatherShadow.section = gatherSprite.section;
		gatherShadow.flip = gatherSprite.flip;
		if (gatheredAmount == 2)
		{
			gatherSprite.section.y = 32;
			gatherShadow.section.y = 32;
		}
		gatherShadow.tint = { 0, 0, 0, 130 };
	}
}
void Unit::UpdateGatherSpark(float dt)
{
	C_Vec2<float> vec = currentVelocity;
	vec.Normalize();
	vec *= 15;
	gatherFrame += 10 * dt;
	if (gatherFrame > 8)
		gatherFrame = 0;
	gatherSpark.section.y = (int)gatherFrame * 40;

	gatherSpark.position.x = position.x + vec.x - 20;
	gatherSpark.position.y = position.y + vec.y - 20;
	gatherSpark.y_ref = App->pathFinding->width * App->pathFinding->height;
}

void Unit::LookAt(Unit* unit)
{
	if (unit)
	{
		C_Vec2<float> vec;
		vec.x = unit->GetPosition().x - position.x;
		vec.y = unit->GetPosition().y - position.y;
		currentVelocity.SetAngle(vec.GetAngle());
	}
}

void Unit::LookAt(Building* building)
{
	if (building)
	{
		C_Vec2<float> vec;
		iPoint buildingPos = App->pathFinding->MapToWorld(building->GetPosition().x, building->GetPosition().y);
		vec.x = buildingPos.x + (building->width_tiles / 2) * 16 - position.x;
		vec.y = buildingPos.y + (building->height_tiles / 2) * 16 - position.y;
		currentVelocity.SetAngle(vec.GetAngle());
	}
}

void Unit::LookAt(Resource* resource)
{
	if (resource)
	{
		C_Vec2<float> vec;
		iPoint resPos = App->pathFinding->MapToWorld(resource->GetPosition().x, resource->GetPosition().y);
		vec.x = resPos.x + (resource->width_tiles / 2) * 16 - position.x;
		vec.y = resPos.y + (resource->height_tiles / 2) * 16 - position.y;
		currentVelocity.SetAngle(vec.GetAngle());
	}
}

void Unit::UpdateAttackState(float dt)
{
	if (attackingUnit)
	{
		if (IsInRange(attackingUnit))
		{
			movement_state = MOVEMENT_ATTACK_IDLE;
			App->entityManager->UpdateCurrentFrame(this);
			actionTimer.Start();
		}
		else if (!waitingForPath)
		{
			iPoint dst = App->pathFinding->WorldToMap(attackingUnit->GetPosition().x, attackingUnit->GetPosition().y);
			SetNewPath(dst, PRIORITY_HIGH);
			logicTimer.Start();
		}
		else
		{
			Stop();
		}
	}
	else if (attackingBuilding)
	{
		if (IsInRange(attackingBuilding))
		{
			movement_state = MOVEMENT_ATTACK_IDLE;
			App->entityManager->UpdateCurrentFrame(this);
			actionTimer.Start();
		}
		else if (!waitingForPath)
		{
			iPoint dst = App->entityManager->GetClosestCorner(this, attackingBuilding);
			SetNewPath(dst, PRIORITY_HIGH);
			logicTimer.Start();
		}
		else
		{
			Stop();
		}
	}
	else
	{
		Stop();
	}

}

void Unit::UpdateAttack(float dt)
{
	float time = actionTimer.ReadSec();
	if (stats.type != DRAGOON)
	{
		if (attackingUnit)
		{
			LookAt(attackingUnit);
		}
		else if (attackingBuilding)
		{
			LookAt(attackingBuilding);
		}
	}

	if (time < ((float)stats.attackSpeed * 3.0f / 4.0f))
	{
		if (attackingUnit && attackingUnit->GetState() != STATE_DIE)
		{
			if (!IsInRange(attackingUnit))
			{
				if (App->entityManager->debug)
				{
					LOG("Unit out of range!");
				}
				movement_state = MOVEMENT_WAIT;
			}
		}
		else if (attackingBuilding && attackingBuilding->state != BS_DEAD)
		{
			if (!IsInRange(attackingBuilding))
			{
				if (App->entityManager->debug)
				{
					LOG("Building out of range!");
				}
				movement_state = MOVEMENT_WAIT;
			}
		}
		else
		{
			Stop();
		}
	}

	if (movement_state != MOVEMENT_WAIT && time >= (float)stats.attackSpeed)
	{
		if (attackingUnit && attackingUnit->GetState() != STATE_DIE)
		{
			in_combatTimer.Start();
			shieldTimer.Start();
			movement_state = MOVEMENT_ATTACK_ATTACK;
			if (App->entityManager->debug)
			{
				LOG("Hitting unit");
			}
			if (stats.type == DRAGOON || stats.type == HYDRALISK || stats.type == MUTALISK)
			{
				if (attackingUnit->GetHP() <= 0)
				{
					movement_state = MOVEMENT_IDLE;
					state = STATE_STAND;
					App->entityManager->UpdateCurrentFrame(this);
				}
				else
				{
					if (stats.type == DRAGOON)
					{
						App->missiles->AddMissil(position, attackingUnit, stats.attackDmg, DRAGOON_MISSILE);
					}
					else if (stats.type == MUTALISK)
					{
						App->missiles->AddMissil(position, attackingUnit, stats.attackDmg, MUTALISK_MISSILE);
					}
					else
					{
						App->missiles->AddMissil(position, attackingUnit, stats.attackDmg, HYDRALISK_MISSILE);
					}
				}
			}
			else if (stats.type == INFESTED_TERRAN)
			{
				StartDeath();
			}
			else
			{
				attackingUnit->Hit(stats.attackDmg);
			}
			App->entityManager->UpdateCurrentFrame(this);
		}
		else if (attackingBuilding && attackingBuilding->state != BS_DEAD)
		{
			in_combatTimer.Start();
			shieldTimer.Start();
			movement_state = MOVEMENT_ATTACK_ATTACK;
			if (stats.type == DRAGOON || stats.type == HYDRALISK || stats.type == MUTALISK)
			{
				if (attackingBuilding->GetHP() <= 0)
				{
					Stop();
				}
				else
				{
					if (stats.type == DRAGOON)
					{
						App->missiles->AddMissil(position, attackingBuilding, stats.attackDmg, DRAGOON_MISSILE, true);
					}
					else if (stats.type == MUTALISK)
					{
						App->missiles->AddMissil(position, attackingBuilding, stats.attackDmg, MUTALISK_MISSILE, true);
					}
					else
					{
						App->missiles->AddMissil(position, attackingBuilding, stats.attackDmg, HYDRALISK_MISSILE, true);
					}
				}
			}
			else if (stats.type == INFESTED_TERRAN)
			{
				StartDeath();
			}
			else
				attackingBuilding->Hit(stats.attackDmg);
			App->entityManager->UpdateCurrentFrame(this);
		}
		else
		{
			Stop();
		}

	}
}

void Unit::SetTarget(int x, int y)
{
	if (position.x == x && position.y == y)
	{
		movement_state = MOVEMENT_WAIT;
		App->entityManager->UpdateCurrentFrame(this);
	}
	else
	{
		target.x = x;
		target.y = y;
		targetReached = false;
		GetDesiredVelocity();
		if (movement_state != MOVEMENT_MOVE)
		{
			movement_state = MOVEMENT_MOVE;
			App->entityManager->UpdateCurrentFrame(this);
		}
	}
}

void Unit::SetType(Unit_Type _type)
{
	stats.type = _type;
}

void Unit::SetMaxSpeed(float speed)
{
	stats.speed = speed;
}

void Unit::SetPriority(int _priority)
{
	priority = _priority;
}

C_Vec2<float> Unit::GetVelocity() const
{
	return currentVelocity;
}

Unit_Type Unit::GetType() const
{
	return stats.type;
}

Unit_State Unit::GetState() const
{
	return state;
}

Unit_Movement_State Unit::GetMovementState() const
{
	return movement_state;
}

Attack_State Unit::GetAttackState() const
{
	return attackState;
}

void Unit::StartDeath()
{
	Stop();
	if (stats.type == INFESTED_TERRAN)
	{
		App->explosion->AddExplosion({ (int)position.x, (int)position.y }, 110, stats.attackDmg, 0.5f, 1, PLAYER, EXPLOSION_TERRAN, false);
	}
	if (selected)
	{
		App->entityManager->UnselectUnit(this);
	}
	movement_state = MOVEMENT_DIE;
	state = STATE_DIE;
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
	HPBar_Shield->SetActive(false);
	logicTimer.Start();
	actionTimer.Start();
	App->entityManager->UpdateCurrentFrame(this);
}

void Unit::Destroy()
{
	LOG("Unit destroyed");
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
	HPBar_Shield->SetActive(false);
}

void Unit::CheckMouseHover()
{
	int x = 0, y = 0;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);

	if (mousePos.x > collider.x && mousePos.x < collider.x + collider.w &&
		mousePos.y > collider.y && mousePos.y < collider.y + collider.h)
	{
		App->entityManager->SetUnitHover(this);
	}
	else if (App->entityManager->hoveringUnit == this)
	{
		App->entityManager->hoveringUnit = NULL;
	}
}

void Unit::Move(iPoint dst, Attack_State _attackState, e_priority priority)
{
	if (!waitingForPath)
	{
		if (SetNewPath(dst, priority))
		{
			if (gatheringResource && gatheringResource->gatheringUnit == this)
			{
				gatheringResource->gatheringUnit = NULL;
			}
			state = STATE_MOVE;
			attackState = _attackState;
		}
	}
}

bool Unit::SetNewPath(iPoint dst, e_priority priority)
{
	bool ret = true;

	if (movementType == GROUND)
	{
		path.clear();
		movement_state = MOVEMENT_IDLE;
		iPoint start = App->pathFinding->WorldToMap(position.x, position.y);
		App->pathFinding->GetNewPath(start, dst, &path, priority);
		waitingForPath = true;
		currentNode = -1;
	}
	else
	{
		iPoint target = App->pathFinding->MapToWorld(dst.x, dst.y);
		target += {8, 8};
		SetTarget(target.x, target.y);
		waitingForPath = false;
	}

	App->entityManager->UpdateCurrentFrame(this);

	return ret;
}

void Unit::SetGathering(Resource* resource)
{
	if (resource)
	{
		gatheringResource = resource;
		gatheringBuilding = NULL;
		attackState = ATTACK_STAND;
		if (gatheredAmount)
		{
			ReturnResource();
		}
		else if (!waitingForPath)
		{
			iPoint startPos = App->pathFinding->WorldToMap(position.x, position.y);
			iPoint endPos = App->entityManager->GetClosestCorner(this, resource);
			SetNewPath(endPos, PRIORITY_HIGH);
			state = STATE_GATHER;
		}
	}
	else
	{
		Stop();
	}
}

void Unit::SetGathering(Building* building)
{

	if (gatheringResource)
	{
		if (gatheringResource->gatheringUnit == this)
			gatheringResource->gatheringUnit = NULL;
		gatheringResource = NULL;
	}

	if (building)
	{
		gatheringBuilding = building;
		attackState = ATTACK_STAND;
		if (gatheredAmount)
		{
			ReturnResource();
		}
		else if (!waitingForPath)
		{
			iPoint startPos = App->pathFinding->WorldToMap(position.x, position.y);
			iPoint endPos = App->entityManager->GetClosestCorner(this, building);
			SetNewPath(endPos, PRIORITY_HIGH);
			state = STATE_GATHER;
		}
	}
	else
	{
		Stop();
	}
}

void Unit::ExitAssimilator(bool hasResource)
{
	if (hasResource)
	{
		gatheredAmount = 8;
		movement_state = MOVEMENT_WAIT;
	}
	else
	{
		gatheredAmount = 2;
		movement_state = MOVEMENT_WAIT;
	}

}

void Unit::ReturnResource()
{
	gatheringNexus = App->entityManager->FindClosestNexus(this);
	if (gatheringNexus)
	{
		if (!waitingForPath)
		{
			iPoint endPos = App->entityManager->GetClosestCorner(this, gatheringNexus);
			SetNewPath(endPos, PRIORITY_HIGH);
			state = STATE_GATHER_RETURN;
			attackState = ATTACK_STAND;
		}
	}
	else
	{
		Stop();
	}
}

//Building functions --------------------------
void Unit::UpdateBuildState()
{
	if (App->entityManager->IsBuildingCreationWalkable(tileToBuild.x, tileToBuild.y, buildingToCreate))
	{
		App->entityManager->CreateBuilding(tileToBuild.x, tileToBuild.y, buildingToCreate, stats.player);
	}
	Stop();
}

void Unit::SendToBuild(Building_Type building, iPoint tile)
{
	Stop();
	buildingToCreate = building;
	tileToBuild = tile;
	iPoint dst = tile;

	if (building == ASSIMILATOR)
	{
		dst -= {1, 1};
	}
	else
	{
		dst += {2, 2};
	}

	state = STATE_BUILD;
	attackState = ATTACK_STAND;
	SetNewPath(dst, PRIORITY_HIGH);
}

void Unit::SetAttack(Unit* unit)
{
	if (unit->GetState() != STATE_DIE)
	{
		attackingUnit = unit;
		attackingBuilding = NULL;
		actionTimer.Start();
		state = STATE_ATTACK;
		movement_state = MOVEMENT_ATTACK_IDLE;
		attackState = ATTACK_STAND;
		App->entityManager->UpdateCurrentFrame(this);
	}
	else 
	{
		Stop();
	}

}

void Unit::SetAttack(Building* building)
{
	attackingBuilding = building;
	attackingUnit = NULL;
	actionTimer.Start();
	state = STATE_ATTACK;
	movement_state = MOVEMENT_ATTACK_IDLE;
	attackState = ATTACK_STAND;
	App->entityManager->UpdateCurrentFrame(this);
}

bool Unit::Hit(int amount)
{
	in_combatTimer.Start();
	shieldTimer.Start();

	int toHit = (amount - stats.armor);

	if (toHit > 0)
	{
		if (stats.shield >= toHit)
		{
			stats.shield -= toHit;
		}
		else
		{
			int lifeLost = toHit - stats.shield;
			stats.shield = 0;
			currHP -= lifeLost;
			if (state != STATE_DIE)
			{
				UpdateBarTexture();
			}
			if (currHP <= 0 && state != STATE_DIE)
			{
				StartDeath();
				return false;
			}
		}
	}

	return true;
}

void Unit::RegenShield()
{
	if (in_combatTimer.ReadSec() >= 10)
	{
		if (shieldTimer.ReadSec() >= 1)
		{
			stats.shield += 2;
			if (stats.shield > stats.maxShield)
				stats.shield = stats.maxShield;
			shieldTimer.Start();
		}
	}
}

bool Unit::IsInRange(Unit* unit)
{
	if (unit->GetMovementType() == FLYING && !stats.canAttackFlying)
		return false;

	iPoint unitPos = { (int)unit->GetPosition().x, (int)unit->GetPosition().y };

	return I_Point_Cicle(unitPos, position.x, position.y, stats.attackRange);
}

bool Unit::IsInRange(Building* building)
{
	if (building->GetMovementType() == FLYING && !stats.canAttackFlying)
		return false;

	iPoint buildingPos = App->pathFinding->MapToWorld(building->GetPosition().x, building->GetPosition().y);
	SDL_Rect buildingRect = { buildingPos.x, buildingPos.y, building->GetCollider().w, building->GetCollider().h };

	return I_Rect_Circle(buildingRect, position.x, position.y, stats.attackRange);
}

bool Unit::HasVision(Unit* unit)
{
	if (unit->GetMovementType() == FLYING && !stats.canAttackFlying)
		return false;

	iPoint unitPos = { (int)unit->GetPosition().x, (int)unit->GetPosition().y };

	return I_Point_Cicle(unitPos, position.x, position.y, stats.visionRange);
}

bool Unit::HasVision(Building* building)
{
	if (building->GetMovementType() == FLYING && !stats.canAttackFlying)
		return false;

	iPoint buildingPos = App->pathFinding->MapToWorld(building->GetPosition().x, building->GetPosition().y);
	SDL_Rect buildingRect = { buildingPos.x, buildingPos.y, building->GetCollider().w, building->GetCollider().h };

	return I_Rect_Circle(buildingRect, position.x, position.y, stats.visionRange);
}

void Unit::Stop()
{
	state = STATE_STAND;
	movement_state = MOVEMENT_IDLE;
	attackState = ATTACK_ATTACK;
	attackingBuilding = NULL;
	attackingUnit = NULL;
	gatheringResource = NULL;
	gatheringBuilding = NULL;
	gatheringNexus = NULL;
	path.clear();
	App->entityManager->UpdateCurrentFrame(this);
}

void Unit::UpdateCollider()
{
	collider.x = round(position.x - collider.w / 2);
	collider.y = round(position.y - collider.h / 2);
	int size = App->entityManager->GetUnitSprite(stats.type)->size;
	animation.sprite.position = { (int)round(position.x - size / 2), (int)round(position.y - size / 2) };
	animation.sprite.y_ref = position.y;

	base.position = { (int)round(position.x - base_offset_x), (int)round(position.y - base_offset_y) };
	base.y_ref = position.y - 2;

}

void Unit::UpdateBarPosition()
{
	const HPBarData* HPBar = App->entityManager->GetHPBarSprite(HPBar_type - 1);

	HPBar_Empty->localPosition.x = collider.x + collider.w / 2 - HPBar->size_x / 2;
	HPBar_Empty->localPosition.y = collider.y + collider.h + 10;
	HPBar_Filled->localPosition.x = collider.x + collider.w / 2 - HPBar->size_x / 2;
	HPBar_Filled->localPosition.y = collider.y + collider.h + 10;
	HPBar_Shield->localPosition.x = collider.x + collider.w / 2 - HPBar->size_x / 2;
	HPBar_Shield->localPosition.y = collider.y + collider.h + 10;
	if (movementType == FLYING)
	{
		HPBar_Empty->localPosition.y += 10;
		HPBar_Filled->localPosition.y += 10;
		HPBar_Shield->localPosition.y += 10;
	}

	HPBar_Empty->UpdateSprite();
	HPBar_Filled->UpdateSprite();
	HPBar_Shield->UpdateSprite();
}

void Unit::UpdateDeath()
{
	if (animation.loopEnd)
	{
		movement_state = MOVEMENT_DEAD;
		logicTimer.Start();
		App->entityManager->UpdateCurrentFrame(this);
	}
}

bool Unit::EraseUnit()
{
	if (logicTimer.ReadSec() > TIME_TO_ERASE_UNIT && !waitingForPath && animation.loopEnd)
	{
		if (App->entityManager->debug)
		{
			LOG("Unit death");
		}
		return false;
	}
	return true;
}

void Unit::LoadLibraryData()
{
	//Loading all stats data
	const UnitStatsData* statsData = App->entityManager->GetUnitStats(stats.type);
	maxHP = currHP = statsData->HP;
	psi = statsData->psi;
	movementType = statsData->movementType;
	stats.attackRange = statsData->attackRange;
	stats.attackSpeed = statsData->cooldown;
	stats.speed = statsData->speed;
	stats.visionRange = statsData->visionRange;
	psi = statsData->psi;
	stats.attackDmg = statsData->damage;
	maxHP = statsData->HP;
	stats.armor = statsData->armor;
	stats.shield = stats.maxShield = statsData->shield;
	stats.canAttackFlying = statsData->canAttackFlying;

	//Loading all sprites data
	const UnitSpriteData* spriteData = App->entityManager->GetUnitSprite(stats.type);
	animation.sprite.texture = spriteData->texture;
	App->entityManager->UpdateSpriteRect(this, animation.sprite, 1);
	animation.sprite.section.w = animation.rect_size_x = spriteData->size;
	animation.sprite.section.h = animation.rect_size_y = spriteData->size;
	animation.firstRect = spriteData->idle_line_start;
	animation.lastRect = spriteData->idle_line_end;
	animation.sprite.y_ref = position.y;
	animation.animSpeed = spriteData->animationSpeed;
	animation.type = A_DOWN;

	//Shadow

	//HP Bar
	HPBar_type = spriteData->HPBar_type;
	const HPBarData* HPBar = App->entityManager->GetHPBarSprite(HPBar_type - 1);
	HPBar_Empty = App->gui->CreateUI_Image({ position.x + collider.w / 2 - HPBar->size_x / 2, position.y + collider.h + 10, 0, 0 }, HPBar->empty, { 0, 0, HPBar->size_x, HPBar->size_y });
	HPBar_Filled = App->gui->CreateUI_ProgressBar({ position.x + collider.w / 2 - HPBar->size_x / 2, position.y + collider.h + 10, 0, 0 }, HPBar->fill, &maxHP, &currHP, { 0, 0, HPBar->size_x, HPBar->size_y });
	HPBar_Shield = App->gui->CreateUI_ProgressBar({ position.x + collider.w / 2 - HPBar->size_x, position.y + collider.h + 10, 0, 0 }, HPBar->shield, &stats.maxShield, &stats.shield, { 0, 0, HPBar->size_x, HPBar->size_y });
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
	HPBar_Shield->SetActive(false);
	HPBar_Empty->sprite.useCamera = HPBar_Filled->sprite.useCamera = HPBar_Shield->sprite.useCamera = true;

	//Base data
	base.texture = spriteData->base.texture;
	base.section = { 0, 0, spriteData->base.size_x, spriteData->base.size_y };
	base.position = { position.x - spriteData->base.offset_x, position.y - spriteData->base.offset_y, 0, 0 };
	base_offset_x = spriteData->base.offset_x;
	base_offset_y = spriteData->base.offset_y;
	base.useCamera = true;
	base.y_ref = position.y - 2;
	base.tint = { 0, 200, 0, 255 };

	if (stats.type == PROBE)
	{
		gatherSprite.section = { 0, 0, 32, 32 };
		gatherSprite.useCamera = true;
		gatherSpark.texture = App->entityManager->probe_spark_tex;
		gatherSpark.section = { 0, 0, 40, 40 };
		gatherSpark.useCamera = true;
	}
}

void Unit::Draw(float dt)
{
		App->entityManager->UpdateSpriteRect(this, animation.sprite, dt);
	if (App->entityManager->render)
	{
		if (movement_state == MOVEMENT_DEAD && stats.type != DRAGOON)
		{
			App->render->AddSprite(&animation.sprite, DECAL);
		}
		else if (App->fogOfWar->IsVisible(position.x, position.y))
		{
			if (selected)
			{
				App->render->AddSprite(&base, SCENE);
			}

			if (movementType == FLYING)
			{
				App->render->AddSprite(&animation.sprite, FLYER);
			}
			else
			{
				App->render->AddSprite(&animation.sprite, SCENE);
			}

			if (stats.type == PROBE)
			{
				if (gatheredAmount)
					App->render->AddSprite(&gatherSprite, SCENE);
				if (movement_state == MOVEMENT_GATHER || movement_state == MOVEMENT_ATTACK_ATTACK || movement_state == MOVEMENT_ATTACK_IDLE)
					App->render->AddSprite(&gatherSpark, SCENE);
			}

			if (App->entityManager->shadows)
			{
				if (stats.type == PROBE && gatheredAmount)
				{
					App->render->AddSprite(&gatherShadow, SCENE);
				}

				//App->render->AddSprite(&shadow, SCENE);
			}
		}
	}
	//Should be independent from scene
	if (App->entityManager->debug)
	{
		DrawDebug();
	}
}

void Unit::DrawDebug()
{
	SDL_Rect rect = collider;
	App->render->AddRect(rect, true, 0, 255, 0, 255, false);
	App->render->AddRect(rect, true, 0, 255, 0, 255, false);

	if (GetMovementState() != MOVEMENT_DEAD)
	{
		//Current velocity vector: green
		float lineX1, lineX2, lineY1, lineY2;
		C_Vec2<float> line = currentVelocity;
		line.Normalize();
		line *= 3;
		lineX1 = position.x;
		lineY1 = position.y;
		lineX2 = (line.x * 10 + lineX1);
		lineY2 = (line.y * 10 + lineY1);
		App->render->AddLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, true, 0, 255, 0);

		//Desired velocity vector: red
		C_Vec2<float> line1 = desiredVelocity;
		line1.Normalize();
		line1 *= 3;
		lineX1 = position.x;
		lineY1 = position.y;
		lineX2 = (line1.x * 10 + lineX1);
		lineY2 = (line1.y * 10 + lineY1);
		App->render->AddLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, true, 255, 0, 0);

		//Attack range
		App->render->AddCircle((int)position.x, (int)position.y, stats.attackRange, true, 255, 0, 0, 255);

		//Vision range
		App->render->AddCircle((int)position.x, (int)position.y, stats.visionRange, true, 0, 255, 255, 255);

		//Path
		if (!path.empty())
		{
			for (uint i = 0; i < path.size(); i++)
			{
				iPoint position = App->pathFinding->MapToWorld(path[i].x, path[i].y);
				SDL_Rect pos = { position.x, position.y, 8, 8 };
				SDL_Rect rect = { 0, 0, 64, 64 };
				if (i < (uint)currentNode)
					rect = { 0, 0, 64, 64 };
				App->render->Blit(App->sceneMap->debug_tex, &pos, true, &rect);
			}
		}
		if (stats.player == COMPUTER)
		{
			App->render->AddRect(collider, true, 255, 0, 0, 80, true);
		}
	}
}