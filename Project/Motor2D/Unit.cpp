#include <stdlib.h>

#include "Unit.h"
#include "Entity.h"
#include "Controlled.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_EntityManager.h"

#include "M_PathFinding.h"

#include "Resource.h"
#include "Building.h"

#include "M_GUI.h"
#include "UI_Element.h"

#include "M_InputManager.h"
#include "Intersections.h"

#include "M_FogOfWar.h"
#include "M_Explosion.h"
#include "M_Player.h"

#include "M_Particles.h"
#include "M_Minimap.h"

Unit::Unit() :Controlled()
{
	LoadLibraryData();
}

Unit::Unit(float x, float y, Unit_Type _type, Player_Type playerType) : Controlled()
{
	position = { x, y };
	stats.type = _type;
	stats.player = playerType;

}
Unit::Unit(fPoint pos) : Controlled()
{
	position = pos;
	LoadLibraryData();
}
Unit::Unit(Unit& toCopy) : Controlled()
{
	position = { toCopy.position.x, toCopy.position.y };
	stats.type = toCopy.stats.type;
	stats.player = toCopy.stats.player;
}

Unit::~Unit()
{

}

bool Unit::Start()
{
	dead = false;

	LoadLibraryData();

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
	attackTimer.Stop();
	//UpdateSpriteState();

	return true;
}

bool Unit::Update(float dt)
{
	bool ret = true;
	bool collided = false;

	if (waitingForPath && state != STATE_DIE)
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

	if (state != STATE_DIE)
	{
		RegenShield();
		CheckMouseHover();
	}

	Draw(dt);

	if (!ret)
	{
		dead = true;
	}

	return ret;
}

void Unit::UpdateMovement(float dt)
{
	//Drawing high templar trail
	if (stats.type == HIGH_TEMPLAR)
	{
		C_Sprite sprite = App->entityManager->highTemplarTrail;
		int direction = 0;
		float angle = GetVelocity().GetAngle() - 90;
		if (angle < 0)
			angle = 360 + angle;
		angle = 360 - angle;
		direction = angle / (360 / 16);

		if (direction > 8)
		{
			sprite.flip = SDL_FLIP_HORIZONTAL;
			direction -= 8;
			sprite.section.x = 8 * sprite.section.w - direction * sprite.section.w;
		}
		else
		{
			sprite.flip = SDL_FLIP_NONE;
			sprite.section.x = direction * sprite.section.w;
		}
		sprite.position = { position.x - sprite.section.w / 2, position.y - sprite.section.h / 2 };
		App->particles->AddParticle(sprite, 4, 0.05f)->type = DECAL;
	}
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
		UpdateSpriteState();
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
				UpdateSpriteState();
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
				UpdateSpriteState();
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
		App->player->AddMineral(gatheredAmount);
		gatheredAmount = 0;
		SetGathering(gatheringResource);
	}
	else if (gatheringBuilding)
	{
		App->player->AddGas(gatheredAmount);
		gatheredAmount = 0;
		SetGathering(gatheringBuilding);
	}
	else
	{
		state = STATE_STAND;
		movement_state = MOVEMENT_IDLE;
		attackState = ATTACK_ATTACK;
		UpdateSpriteState();
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
	//App->entityManager->UpdateSpriteRect(this, gatherSprite, 0);
	//UpdateSprite(0);
	gatherShadow.section = gatherSprite.section;
	gatherShadow.flip = gatherSprite.flip;
	if (gatheredAmount == 2)
	{
		gatherSprite.section.y = 32;
		gatherShadow.section.y = 32;
	}
	gatherShadow.tint = { 0, 0, 0, 130 };
}
void Unit::UpdateGatherSpark(float dt)
{
	C_Vec2<float> vec = currentVelocity;
	vec.Normalize();
	vec *= 15;
	gatherFrame += 10 * dt;
	if (gatherFrame > 13)
		gatherFrame = 0;
	gatherSpark.section.y = (int)gatherFrame * 32;

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
	if ((attackingUnit && attackingUnit->GetState() != STATE_DIE) || (attackingBuilding && attackingBuilding->state != BS_DEAD))
	{
		if (attackTimer.IsStopped() || (attackTimer.ReadSec() > stats.attackSpeed))
		{
			if (attackingUnit)
			{
				if (IsInRange(attackingUnit))
				{
					Attack();
				}
				else
				{
					if (waitingForPath)
					{
						App->pathFinding->RemovePath(pathIndex);
						waitingForPath = false;
					}
					iPoint dst = App->pathFinding->WorldToMap(attackingUnit->GetPosition().x, attackingUnit->GetPosition().y);
					SetNewPath(dst, PRIORITY_HIGH);
					logicTimer.Start();
				}
			}
			else if (attackingBuilding)
			{
				if (IsInRange(attackingBuilding))
				{
					Attack();
				}
				else
				{
					if (waitingForPath)
					{
						App->pathFinding->RemovePath(pathIndex);
						waitingForPath = false;
					}
					iPoint dst = App->entityManager->GetClosestCorner(this, attackingBuilding);
					SetNewPath(dst, PRIORITY_HIGH);
					logicTimer.Start();
				}
			}
			else
			{
				Stop();
			}
		}
		else
		{
			movement_state = MOVEMENT_ATTACK_IDLE;
		}
	}
	else
	{
		Stop();
	}
}

void Unit::UpdateAttack(float dt)
{
	float time = attackTimer.ReadSec();
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
		movement_state = MOVEMENT_WAIT;
	}
}
void Unit::Attack()
{
	if ((attackingUnit && attackingUnit->state != STATE_DIE) || attackingBuilding && attackingBuilding->state != BS_DEAD)
	{
		in_combatTimer.Start();
		shieldTimer.Start();
		attackTimer.Start();
		movement_state = MOVEMENT_ATTACK_ATTACK;
		if (App->entityManager->debug)
		{
			LOG("Hitting unit");
		}
		if (stats.type == DRAGOON || stats.type == HYDRALISK || stats.type == MUTALISK || stats.type == REAVER || stats.type == SCOUT || stats.type == HIGH_TEMPLAR || stats.type == SCOUT_CIN)
		{
			App->entityManager->PlayUnitSound(stats.type, sound_attack, position);
			if (stats.type == HIGH_TEMPLAR)
			{
				if (attackingUnit)
					App->explosion->AddExplosion({ (int)attackingUnit->position.x, (int)attackingUnit->position.y }, 85, 14, 1.0f, 7, COMPUTER, EXPLOSION_PSIONIC_STORM, false);
				else
					App->explosion->AddExplosion({ (int)attackingBuilding->GetCollider().x, (int)attackingBuilding->GetCollider().y }, 85, 14, 1.0f, 7, COMPUTER, EXPLOSION_PSIONIC_STORM, false);
			}
			else if (stats.type == MUTALISK)
			{
				if (attackingUnit)
					App->missiles->AddMissil(position, attackingUnit, stats.attackDmg, MUTALISK_MISSILE);
				else
					App->missiles->AddMissil(position, attackingBuilding, stats.attackDmg, MUTALISK_MISSILE, true);

			}
			else if (stats.type == HYDRALISK)
			{
				if (attackingUnit)
					App->missiles->AddMissil(position, attackingUnit, stats.attackDmg, HYDRALISK_MISSILE);
				else
					App->missiles->AddMissil(position, attackingBuilding, stats.attackDmg, HYDRALISK_MISSILE, true);
			}
			else
			{
				if (attackingUnit)
					App->missiles->AddMissil(position, attackingUnit, stats.attackDmg, DRAGOON_MISSILE);
				else
					App->missiles->AddMissil(position, attackingBuilding, stats.attackDmg, DRAGOON_MISSILE, true);
			}
		}
		else if (stats.type == INFESTED_TERRAN)
		{
			StartDeath();
		}
		else
		{
			App->entityManager->PlayUnitSound(stats.type, sound_attack, position);
			attackingUnit ? attackingUnit->Hit(stats.attackDmg) : attackingBuilding->Hit(stats.attackDmg);
		}
		UpdateSpriteState();
	}
}

void Unit::SetTarget(int x, int y)
{
	if (position.x == x && position.y == y)
	{
		movement_state = MOVEMENT_WAIT;
		UpdateSpriteState();
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
			UpdateSpriteState();
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
	App->entityManager->PlayUnitSound(stats.type, sound_death, position);
	Stop();
	if (stats.type == INFESTED_TERRAN)
	{
		App->explosion->AddExplosion({ (int)position.x, (int)position.y }, 65, stats.attackDmg, 0.1f, 1, PLAYER, EXPLOSION_TERRAN, false);
	}
	if (selected)
	{
		App->entityManager->createBuilding = false;
		App->entityManager->UnselectUnit(this);
	}
	if (App->entityManager->hoveringUnit == this)
	{
		App->entityManager->hoveringUnit = NULL;
	}
	if (stats.player == PLAYER)
	{
		App->player->SubstractPsi(psi);
	}
	movement_state = MOVEMENT_DIE;
	state = STATE_DIE;
	waitingForPath = false;
	HPBar->SetActive(false);
	logicTimer.Start();
	actionTimer.Start();
	UpdateSpriteState();
}

void Unit::Destroy()
{
	LOG("Unit destroyed");
	HPBar->SetActive(false);
}

void Unit::CheckMouseHover()
{
	if (App->events->hoveringUI == false)
	{
		iPoint mousePos = App->events->GetMouseOnWorld();

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
		pathIndex = App->pathFinding->GetNewPath(start, dst, &path, priority);
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

	UpdateSpriteState();

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
		state = STATE_ATTACK;
		movement_state = MOVEMENT_WAIT;
		attackState = ATTACK_STAND;
		UpdateSpriteState();
	}
	else 
	{
		Stop();
	}
}

void Unit::SetAttack(Building* building)
{
	if (building->state != BS_DEAD)
	{
		attackingBuilding = building;
		attackingUnit = NULL;
		actionTimer.Start();
		state = STATE_ATTACK;
		movement_state = MOVEMENT_WAIT;
		attackState = ATTACK_STAND;
		UpdateSpriteState();
	}
	else
	{
		Stop();
	}
}

bool Unit::Hit(int amount)
{
	in_combatTimer.Start();
	if (stats.player == PLAYER)
	{
		shieldTimer.Start();
		App->minimap->PingOnWorld(position.x, position.y);
	}

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
			if (stats.player == COMPUTER && !RegenHP())
			{
				shieldTimer.Start();
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
	//Protoss shield regeneration
	if (stats.player == PLAYER)
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
	//Zerg hp regeneration
	if (stats.player == COMPUTER)
	{
		if (shieldTimer.ReadSec() >= 3.7)
		{
			if (RegenHP())
			{
				shieldTimer.Stop();
			}
			else
			{
				shieldTimer.Start();
			}
		}
	}
}

bool Unit::RegenHP()
{
	currHP += 1;
	if (currHP > maxHP)
	{
		currHP = maxHP;
		return true;
	}
	return false;
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
	UpdateSpriteState();
}

void Unit::UpdateCollider()
{
	collider.x = round(position.x - collider.w / 2);
	collider.y = round(position.y - collider.h / 2);
	int size = App->entityManager->GetUnitSprite(stats.type)->size;
	animation.sprite.position = { (int)round(position.x - size / 2), (int)round(position.y - size / 2) };
	animation.sprite.y_ref = position.y;
	shadow.sprite.position = animation.sprite.position;
	shadow.sprite.position.x += shadow_offset_x;
	shadow.sprite.position.y += shadow_offset_y;

	base.position = { (int)round(position.x - base_offset_x), (int)round(position.y - base_offset_y) };
	base.y_ref = position.y - 2;

}

void Unit::UpdateBarPosition()
{
	const HPBarData* HPBar_data = App->entityManager->GetHPBarSprite(HPBar_type - 1);

	HPBar->localPosition.x = collider.x + collider.w / 2 - HPBar_data->size_x / 2;
	HPBar->localPosition.y = collider.y + collider.h + 10;


	if (movementType == FLYING)
	{
		HPBar->localPosition.y += 10;
	}
}
void Unit::UpdateSprite(float dt)
{
	//Rectangle definition variables
	int direction, size, rectX = 0, rectY = 0;

	if (dt)
	{
		animation.Update(dt);
		shadow.sprite.section = animation.sprite.section;
		shadow.sprite.position = animation.sprite.position;
		shadow.sprite.position.y += shadow_offset_y;
		shadow.sprite.position.x += shadow_offset_x;

		if (GetMovementState() != MOVEMENT_DIE && GetMovementState() != MOVEMENT_DEAD)
		{
			if (GetMovementState() == MOVEMENT_ATTACK_ATTACK && animation.loopEnd)
			{
				movement_state = MOVEMENT_ATTACK_IDLE;
				UpdateSpriteState();
				UpdateSprite(dt);
			}
			if (GetMovementType() == FLYING && GetType() != MUTALISK)
			{
				if ((int)currentFrame == 2 || (int)currentFrame == 0)
					flyingOffset = 0;
				else if ((int)currentFrame == 1)
					flyingOffset = -2;
				else if ((int)currentFrame == 3)
					flyingOffset = 2;
				rectY = 0;
			}

			if (GetMovementType() == FLYING)
			{
				animation.sprite.position.y = (int)round(GetPosition().y - spriteData->size / 2) + flyingOffset;
			}

			//Getting unit movement direction----
			float angle = currentVelocity.GetAngle() - 90;
			if (angle < 0)
				angle = 360 + angle;
			angle = 360 - angle;
			direction = angle / (360 / 32);

			if (direction > 16)
			{
				gatherSprite.flip = animation.sprite.flip = SDL_FLIP_HORIZONTAL;
				direction -= 16;
				rectX = 16 * spriteData->size - direction * spriteData->size;
			}
			else
			{
				gatherSprite.flip = animation.sprite.flip = SDL_FLIP_NONE;
				rectX = direction * spriteData->size;
			}
			animation.sprite.section.x = rectX;
		}

		if (stats.type == PROBE)
		{
			if (movement_state == MOVEMENT_GATHER || movement_state == MOVEMENT_ATTACK_ATTACK || movement_state == MOVEMENT_ATTACK_IDLE)
			{
				UpdateGatherSpark(dt);
			}
			if (gatheredAmount)
			{
				UpdateGatherSprite();
			}
		}
	}
}

void Unit::UpdateSpriteState()
{
	switch (movement_state)
	{
	case(MOVEMENT_IDLE) :
	{
		animation.currentRect = currentFrame = spriteData->idle_line_start;
		animation.firstRect = spriteData->idle_line_start;
		animation.lastRect = spriteData->idle_line_end;
		animation.loopable = true;
		animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_ATTACK_IDLE) :
	{
		animation.currentRect = currentFrame = spriteData->idle_line_start;
		animation.firstRect = spriteData->idle_line_start;
		animation.lastRect = spriteData->idle_line_end;
		animation.loopable = true;
		animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_ATTACK_ATTACK) :
	{
		animation.currentRect = currentFrame = spriteData->attack_line_start;
		animation.firstRect = spriteData->attack_line_start;
		animation.lastRect = spriteData->attack_line_end;
		animation.loopable = false;
		animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_GATHER) :
	{
		animation.currentRect = currentFrame = spriteData->idle_line_start;
		animation.firstRect = spriteData->idle_line_start;
		animation.lastRect = spriteData->idle_line_end;
		animation.loopable = true;
		animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_MOVE) :
	{
		animation.currentRect = currentFrame = spriteData->run_line_start;
		animation.firstRect = spriteData->run_line_start;
		animation.lastRect = spriteData->run_line_end;
		animation.loopable = true;
		animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_DIE) :
	{
		animation.currentRect = currentFrame = spriteData->death_column_start;
		animation.sprite.section.y = spriteData->size * spriteData->death_line;
		animation.sprite.section.x = 0;
		animation.firstRect = spriteData->death_column_start;
		animation.lastRect = spriteData->death_column_end;
		animation.type = A_RIGHT;
		animation.loopable = false;
		animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_DEAD) :
	{
		animation.sprite.texture = spriteData->corpse;
		animation.rect_size_x = animation.sprite.section.w = spriteData->deathSize.x;
		animation.rect_size_y = animation.sprite.section.h = spriteData->deathSize.y;
		animation.sprite.position.x = round(position.x) - spriteData->deathSize.x / 2;
		animation.sprite.position.y = round(position.y) - spriteData->deathSize.y / 2;
		animation.sprite.section.x = animation.sprite.section.y = 0;
		animation.currentRect = 0;
		animation.firstRect = 0;
		animation.lastRect = spriteData->deathNFrames;
		animation.animSpeed = 1 / (spriteData->deathDuration / spriteData->deathNFrames);
		animation.type = A_DOWN;
		animation.loopable = false;
		animation.loopEnd = false;
		break;
	}
	}
}

void Unit::UpdateDeath()
{
	if (animation.loopEnd)
	{
		movement_state = MOVEMENT_DEAD;
		logicTimer.Start();
		UpdateSpriteState();
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
	name = statsData->name;
	race = statsData->race;
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
	spriteData = App->entityManager->GetUnitSprite(stats.type);
	animation.sprite.texture = spriteData->texture;
	animation.sprite.section.w = animation.rect_size_x = spriteData->size;
	animation.sprite.section.h = animation.rect_size_y = spriteData->size;
	animation.firstRect = spriteData->idle_line_start;
	animation.lastRect = spriteData->idle_line_end;
	animation.sprite.y_ref = position.y;
	animation.animSpeed = spriteData->animationSpeed;
	animation.type = A_DOWN;

	//Shadow
	shadow.sprite.texture = spriteData->shadow.texture;
	shadow.sprite.tint = { 0, 0, 0, 130 };
	shadow_offset_x = spriteData->shadow.offset_x;
	shadow_offset_y = spriteData->shadow.offset_y;

	//HP Bar
	HPBar_type = spriteData->HPBar_type;
	const HPBarData* HPBar_data = App->entityManager->GetHPBarSprite(HPBar_type - 1);
	HPBar = App->gui->CreateUI_HPBar({ position.x + collider.w / 2 - HPBar_data->size_x / 2, position.y + collider.h + 10, HPBar_data->size_x, HPBar_data->size_y }, HPBar_data->fill, HPBar_data->shield, HPBar_data->empty, &maxHP, &currHP, &stats.maxShield, &stats.shield);
	HPBar->SetActive(false);
	HPBar->sprite.useCamera = true;

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
		gatherSprite.position = { 0, 0, 0, 0 };
		gatherSprite.useCamera = true;
		gatherSpark.texture = App->entityManager->probe_spark_tex;
		gatherSpark.section = { 0, 0, 40, 32 };
		gatherSpark.position = { 0, 0, 0, 0 };
		gatherSpark.useCamera = true;
	}
}

void Unit::Draw(float dt)
{
	UpdateSprite(dt);
	if (App->entityManager->render)
	{
		if (movement_state == MOVEMENT_DEAD && stats.type != DRAGOON)
		{
			if (animation.sprite.texture)
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

			if (stats.type == PROBE && state != STATE_DIE)
			{
				if (gatheredAmount)
					App->render->AddSprite(&gatherSprite, SCENE);
				if (movement_state == MOVEMENT_GATHER || movement_state == MOVEMENT_ATTACK_ATTACK || movement_state == MOVEMENT_ATTACK_IDLE)
					App->render->AddSprite(&gatherSpark, SCENE);
			}
			if (App->entityManager->shadows && state != STATE_DIE)
			{
				if (shadow.sprite.texture)
				{
					if (stats.type == PROBE && gatheredAmount)
					{
						App->render->AddSprite(&gatherShadow, SCENE);
					}
					shadow.sprite.flip = animation.sprite.flip;
					App->render->AddSprite(&shadow.sprite, DECAL);
				}
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

		if (stats.player == COMPUTER)
		{
			App->render->AddRect(collider, true, 255, 0, 0, 80, true);
		}
	}
}