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
	case (MOVEMENT_DIE):
	{		
		ret = UpdateDeath(dt);
		break;
	}
	}
	CheckMouseHover();
	Draw(dt);

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
			App->entityManager->UpdateCurrentFrame(this);
			actionTimer.Start();
		}
		else
		{
			iPoint dst = App->pathFinding->WorldToMap(attackingUnit->GetPosition().x, attackingUnit->GetPosition().y);
			SetNewPath(dst);
			logicTimer.Start();
		}
	}
	else if (attackingBuilding)
	{
		if (IsInRange(attackingBuilding))
		{
			movement_state = MOVEMENT_ATTACK;
			App->entityManager->UpdateCurrentFrame(this);
			actionTimer.Start();
		}
		else
		{
			iPoint dst = App->entityManager->GetClosestCorner(this, attackingBuilding);
			SetNewPath(dst);
			logicTimer.Start();
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
	if (time < ((float)stats.attackSpeed * 3.0f / 4.0f))
	{
		if (attackingUnit && attackingUnit->GetState() != STATE_DIE)
		{
			if (!IsInRange(attackingUnit))
			{
				LOG("Unit out of range!");
				movement_state = MOVEMENT_WAIT;
			}
		}
		else if (attackingBuilding)
		{
			if (!IsInRange(attackingBuilding))
			{
				LOG("Building out of range!");
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
			LOG("Hitting unit");
			if (stats.type == DRAGOON)
			{
				if (attackingUnit->GetHP() <= 0)
				{
					movement_state = MOVEMENT_IDLE;
					state = STATE_STAND;
					App->entityManager->UpdateCurrentFrame(this);
				}
				else
				{
					App->missiles->AddMissil(position, attackingUnit);
					App->entityManager->UpdateCurrentFrame(this);
				}
			}
			else if (!attackingUnit->Hit(stats.attackDmg))
			{
				movement_state = MOVEMENT_IDLE;
				state = STATE_STAND;
				App->entityManager->UpdateCurrentFrame(this);
			}

			movement_state = MOVEMENT_WAIT;
		}
		else if (attackingBuilding)
		{
			LOG("Hitting building");
			if (stats.type == DRAGOON)
			{
				if (attackingBuilding->GetHP() <= 0)
				{
					movement_state = MOVEMENT_IDLE;
					state = STATE_STAND;
					App->entityManager->UpdateCurrentFrame(this);
				}
				else
				{
					App->missiles->AddMissil(position, attackingBuilding);
					App->entityManager->UpdateCurrentFrame(this);
				}
			}
			else if (!attackingBuilding->Hit(stats.attackDmg))
			{
				movement_state = MOVEMENT_IDLE;
				state = STATE_STAND;
				App->entityManager->UpdateCurrentFrame(this);
			}

			movement_state = MOVEMENT_WAIT;
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
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
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
		iPoint endPos = App->entityManager->GetClosestCorner(this, gatheringNexus);
		if (SetNewPath(endPos))
		{
			state = STATE_GATHER_RETURN;
		}
	}
	else
	{
		Stop();
	}
}

void Unit::SetAttack(Unit* unit)
{
	if (unit->GetState() != STATE_DIE)
	{
		attackingUnit = unit;
		attackingBuilding = NULL;
		actionTimer.Start();
		state = STATE_ATTACK;
		movement_state = MOVEMENT_ATTACK;
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
	movement_state = MOVEMENT_ATTACK;
	attackState = ATTACK_STAND;
	App->entityManager->UpdateCurrentFrame(this);
}

bool Unit::Hit(int amount)
{
	//App->render->AddRect(collider, true, 255, 255, 255);
	currHP -= amount;
	if (state != STATE_DIE)
	{
		UpdateBarTexture();
	}
	if (currHP <= 0)
	{
		movement_state = MOVEMENT_DIE;
		state = STATE_DIE;
		App->entityManager->UpdateCurrentFrame(this);
		return false;
	}

	return true;
}

bool Unit::IsInRange(Unit* unit)
{
	iPoint unitPos = { (int)unit->GetPosition().x, (int)unit->GetPosition().y };

	return I_Point_Cicle(unitPos, position.x, position.y, stats.attackRange);
}

bool Unit::IsInRange(Building* building)
{
	iPoint buildingPos = App->pathFinding->MapToWorld(building->GetPosition().x, building->GetPosition().y);
	SDL_Rect buildingRect = { buildingPos.x, buildingPos.y, building->GetCollider().w, building->GetCollider().h };

	return I_Rect_Circle(buildingRect, position.x, position.y, stats.attackRange);
}

bool Unit::HasVision(Unit* unit)
{
	iPoint unitPos = { (int)unit->GetPosition().x, (int)unit->GetPosition().y };

	return I_Point_Cicle(unitPos, position.x, position.y, stats.visionRange);
}

bool Unit::HasVision(Building* building)
{
	iPoint buildingPos = App->pathFinding->MapToWorld(building->GetPosition().x, building->GetPosition().y);
	SDL_Rect buildingRect = { buildingPos.x, buildingPos.y, building->GetCollider().w, building->GetCollider().h };

	return I_Rect_Circle(buildingRect, position.x, position.y, stats.visionRange);
}

void Unit::Stop()
{
	state = STATE_STAND;
	movement_state = MOVEMENT_IDLE;
	attackState = ATTACK_ATTACK;
	path.clear();
	App->entityManager->UpdateCurrentFrame(this);
}

void Unit::UpdateCollider()
{
	collider.x = round(position.x - collider.w / 2);
	collider.y = round(position.y - collider.h / 2);
	int size = App->entityManager->GetUnitSprite(stats.type)->size;
	sprite.position = { (int)round(position.x - size / 2), (int)round(position.y - size / 2) };
	sprite.y_ref = position.y;

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

	//if (movementType == FLYING)
	//{
	//	HPBar_Empty->localPosition.y -= 20;
	//	HPBar_Filled->localPosition.y -= 20;
	//}

	HPBar_Empty->UpdateSprite();
	HPBar_Filled->UpdateSprite();
}

bool Unit::UpdateDeath(float dt)
{
	if (logicTimer.ReadSec() > 3)
	{
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

	//Loading all sprites data
	const UnitSpriteData* spriteData = App->entityManager->GetUnitSprite(stats.type);
	sprite.texture = spriteData->texture;
	App->entityManager->UpdateSpriteRect(this, sprite, 1);
	sprite.y_ref = position.y;
	sprite.useCamera = true;

	//Shadow

	//HP Bar
	HPBar_type = spriteData->HPBar_type;
	const HPBarData* HPBar = App->entityManager->GetHPBarSprite(HPBar_type - 1);
	HPBar_Empty = App->gui->CreateUI_Image({ position.x + collider.w / 2 - HPBar->size_x / 2, position.y + collider.h + 10, 0, 0 }, HPBar->empty, { 0, 0, HPBar->size_x, HPBar->size_y });
	HPBar_Filled = App->gui->CreateUI_ProgressBar({ position.x + collider.w / 2 - HPBar->size_x / 2, position.y + collider.h + 10, 0, 0 }, HPBar->fill, &maxHP, &currHP, { 0, 0, HPBar->size_x, HPBar->size_y });
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
	HPBar_Empty->sprite.useCamera = HPBar_Filled->sprite.useCamera = true;

	//Base data
	base.texture = spriteData->base.texture;
	base.section = { 0, 0, spriteData->base.size_x, spriteData->base.size_y };
	base.position = { position.x - spriteData->base.offset_x, position.y - spriteData->base.offset_y, 0, 0 };
	base_offset_x = spriteData->base.offset_x;
	base_offset_y = spriteData->base.offset_y;
	base.useCamera = true;
	base.y_ref = position.y - 2;
	base.tint = { 0, 200, 0, 255 };
}

void Unit::Draw(float dt)
{
	if (App->entityManager->render)
	{
		if (selected)
		{
			App->render->AddSprite(&base, SCENE);
		}			
		App->render->Blit(App->entityManager->unit_base, (int)round(position.x - 32), (int)round(position.y) - 32, true, NULL);
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