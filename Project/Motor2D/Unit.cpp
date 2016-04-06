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
	currentVelocity *= maxSpeed;

	UpdateCollider();
	UpdateBarPosition();

	movement_state = MOVEMENT_IDLE;
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
			movement_state = MOVEMENT_IDLE;
			attackState = ATTACK_ATTACK;
			LOG("Attack state: attack_attack");
			App->entityManager->UpdateCurrentFrame(this);
			break;
		}
		case(STATE_MOVE) :
		{
			movement_state = MOVEMENT_IDLE;
			attackState = ATTACK_ATTACK;
			LOG("Attack state: attack_attack");
			App->entityManager->UpdateCurrentFrame(this);
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
			LOG("Updating attack state");
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
	case (MOVEMENT_GATHER) :
	{
		UpdateGather(dt);
		break;
	}
	case (MOVEMENT_ATTACK) :
	{
		UpdateAttack(dt);
		break;
	}
	}
	CheckMouseHover();
	Draw(dt);

	if (currHP <= 0)
	{
		ret = false;
	}

	return ret;
}

void Unit::UpdateMovement(float dt)
{
	if (state == STATE_ATTACK)
	{
		if (logicTimer.ReadSec() >= 0.1)
		{
			if (IsInRange(attackingUnit))
			{
				movement_state = MOVEMENT_WAIT;
			}
			logicTimer.Start();
		}
	}
	if (!targetReached)
	{
		if (UpdateVelocity(dt))
		{
			if (!UpdatePosition(dt))
			{
				targetReached = true;
			}
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
	GetDesiredVelocity();
	if (true)
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
	velocity *= maxSpeed;
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

void Unit::UpdateAttackState(float dt)
{
	if (attackingUnit)
	{
		if (IsInRange(attackingUnit))
		{
			movement_state = MOVEMENT_ATTACK;
			actionTimer.Start();
		}
		else
		{
			iPoint dst = App->pathFinding->WorldToMap(attackingUnit->GetPosition().x, attackingUnit->GetPosition().y);
			SetNewPath(dst);
			logicTimer.Start();
		}
	}
	else
	{
		state = STATE_STAND;
		movement_state = MOVEMENT_IDLE;
	}

}

void Unit::UpdateAttack(float dt)
{
	LOG("Updating attack");
	float time = actionTimer.ReadSec();
	if (time < ((float)stats.attackSpeed * 3.0f / 4.0f))
	{
		if (!IsInRange(attackingUnit))
		{
			LOG("Unit out of range!");
			movement_state = MOVEMENT_WAIT;
		}
	}

	if (movement_state != MOVEMENT_WAIT && time >= (float)stats.attackSpeed)
	{
		LOG("Hitting unit");

		if (!attackingUnit->Hit(stats.attackDmg))
		{
			movement_state = MOVEMENT_IDLE;
			state = STATE_STAND;
			App->entityManager->UpdateCurrentFrame(this);
		}

		movement_state = MOVEMENT_WAIT;
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
		movement_state = MOVEMENT_MOVE;
		App->entityManager->UpdateCurrentFrame(this);
	}
}

void Unit::SetType(Unit_Type _type)
{
	stats.type = _type;
}

void Unit::SetMaxSpeed(float speed)
{
	maxSpeed = speed;
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

void Unit::Destroy()
{
	LOG("Unit destroyed");
	App->gui->DeleteUIElement(HPBar_Empty);
	App->gui->DeleteUIElement(HPBar_Filled);
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
void Unit::Move(iPoint dst, Attack_State _attackState)
{
	if (!waitingForPath)
	{
		if (SetNewPath(dst))
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

bool Unit::SetNewPath(iPoint dst)
{
	bool ret = true;
	path.clear();
	movement_state = MOVEMENT_IDLE;
	iPoint start = App->pathFinding->WorldToMap(position.x, position.y);
	App->pathFinding->GetNewPath(start, dst, &path);
	waitingForPath = true;
	currentNode = -1;

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
		else
		{
			iPoint startPos = App->pathFinding->WorldToMap(position.x, position.y);
			iPoint endPos = App->entityManager->GetClosestCorner(this, resource);
			if (SetNewPath(endPos))
			{
				state = STATE_GATHER;
			}
		}
	}
	else
	{
		state = STATE_STAND;
		movement_state = MOVEMENT_IDLE;
		attackState = ATTACK_ATTACK;
		App->entityManager->UpdateCurrentFrame(this);
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
		else
		{
			iPoint startPos = App->pathFinding->WorldToMap(position.x, position.y);
			iPoint endPos = App->entityManager->GetClosestCorner(this, building);
			if (SetNewPath(endPos))
			{
				state = STATE_GATHER;
			}
		}
	}
	else
	{
		state = STATE_STAND;
		movement_state = MOVEMENT_IDLE;
		attackState = ATTACK_ATTACK;
		App->entityManager->UpdateCurrentFrame(this);
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
		iPoint endPos = App->entityManager->GetClosestCorner(this, gatheringNexus);
		if (SetNewPath(endPos))
		{
			state = STATE_GATHER_RETURN;
		}
	}
	else
	{
		state = STATE_STAND;
		movement_state = MOVEMENT_IDLE;
		App->entityManager->UpdateCurrentFrame(this);
	}
}

void Unit::SetAttack(Unit* unit)
{
	attackingUnit = unit;
	actionTimer.Start();
	state = STATE_ATTACK;
	movement_state = MOVEMENT_ATTACK;
	attackState = ATTACK_ATTACK;
	LOG("Attack state: attack_attack");
	App->entityManager->UpdateCurrentFrame(this);
}

bool Unit::Hit(int amount)
{
	App->render->AddRect(collider, true, 255, 255, 255);
	currHP -= amount;
	UpdateBarTexture();
	if (currHP <= 0)
		return false;
	return true;
}

bool Unit::IsInRange(Unit* unit)
{
	int dstX = abs(position.x - unit->GetPosition().x);
	int dstY = abs(position.y - unit->GetPosition().y);
	float dst = sqrt(dstX * dstX + dstY * dstY);
	if (dst < stats.attackRange)
	{
		return true;
	}
	return false;
}

bool Unit::HasVision(Unit* unit)
{
	int dstX = abs(position.x - unit->GetPosition().x);
	int dstY = abs(position.y - unit->GetPosition().y);
	float dst = sqrt(dstX * dstX + dstY * dstY);
	if (dst < stats.visionRange)
	{
		return true;
	}
	return false;
}

void Unit::Stop()
{
	state = STATE_STAND;
	movement_state = MOVEMENT_IDLE;
	path.clear();
	App->entityManager->UpdateCurrentFrame(this);
}

void Unit::UpdateCollider()
{
	collider.x = round(position.x - collider.w / 2);
	collider.y = round(position.y - collider.h / 2);
	int size = App->entityManager->GetUnitSprite(stats.type)->size / 2;
	sprite.position = { (int)round(position.x - size), (int)round(position.y - size) };
	sprite.y_ref = position.y;
}

void Unit::UpdateBarPosition()
{
	HPBar_Empty->localPosition.x = position.x - 17;
	HPBar_Empty->localPosition.y = position.y + 20;
	HPBar_Filled->localPosition.x = position.x - 17;
	HPBar_Filled->localPosition.y = position.y + 20;

	if (movementType == FLYING)
	{
		HPBar_Empty->localPosition.y -= 20;
		HPBar_Filled->localPosition.y -= 20;
	}
	HPBar_Empty->UpdateSprite();
	HPBar_Filled->UpdateSprite();
}

void Unit::LoadLibraryData()
{
	iPoint pos = App->pathFinding->MapToWorld(position.x, position.y);

	//Loading all stats data
	const UnitStatsData* statsData = App->entityManager->GetUnitStats(stats.type);
	maxHP = currHP = statsData->HP;
	psi = statsData->psi;
	movementType = statsData->movementType;

	//Loading all sprites data
	const UnitSpriteData* spriteData = App->entityManager->GetUnitSprite(stats.type);
	sprite.texture = spriteData->texture;
	App->entityManager->UpdateSpriteRect(this, sprite, 1);
	sprite.y_ref = position.y;
	sprite.useCamera = true;

	//Base data
	//base.texture = App->tex->Load("graphics/ui/o072.png");
	base.section = { 0, 0, 80, 80 };
	base.position = { pos.x - 8, pos.y + 8, 0, 0 };
	base.useCamera = true;
	base.y_ref = position.y - 2;
	base.tint = { 0, 200, 0, 255 };
}

void Unit::Draw(float dt)
{
	if (App->entityManager->render)
	{
		//if (selected)
		//	App->render->Blit(App->entityManager->unit_base, (int)round(position.x - 32), (int)round(position.y) - 32, true, NULL);
		App->entityManager->UpdateSpriteRect(this, sprite, dt);
		App->render->AddSprite(&sprite, SCENE);
	}
	if (App->entityManager->shadows)
	{
		//App->render->AddSprite(&shadow, SCENE);
	}
	//Should be independent from scene
	if (App->entityManager->debug)
	{
		DrawDebug();
	}

}

void Unit::DrawDebug()
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

	SDL_Rect rect = collider;
	App->render->AddRect(rect, true, 0, 255, 0, 255, false);
	App->render->AddRect(rect, true, 0, 255, 0, 255, false);

	//Attack range
	App->render->AddCircle((int)position.x, (int)position.y, stats.attackRange, true, 255, 0, 0, 255);

	//Vision range
	App->render->AddCircle((int)position.x, (int)position.y, stats.visionRange, true, 0, 255, 255, 255);

	//Path
	if (path.size() > 0)
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
}