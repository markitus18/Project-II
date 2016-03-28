#include <stdlib.h>

#include "Unit.h"
#include "Entity.h"
#include "Controlled.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_EntityManager.h"
#include "UI_Element.h"
#include "M_PathFinding.h"

#include "S_SceneMap.h"

Unit::Unit() :Controlled()
{

}

Unit::Unit(float x, float y)
{
	position = { x, y };
}
Unit::Unit(fPoint pos)
{
	position = pos;
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
	
	sprite.texture = App->entityManager->GetUnitSprite(type)->texture;
	sprite.useCamera = true;
	GetTextureRect(sprite.section, sprite.flip);
	UpdateCollider();

	state = IDLE;
	App->entityManager->UpdateCurrentFrame(this);

	return true;
}

bool Unit::Update(float dt)
{
	bool ret = true;
	bool collided = false;

	if (!targetReached)
	{
		if (UpdateVelocity(dt))
		{
			if (!Move(dt))
			{
				targetReached = true;
			}
		}
	}
	if (targetReached)
	{
		GetNewTarget();
	}

	UpdateBarPosition();
	UpdateBarTexture();

	Draw(dt);

	if (currHP <= 0)
	{
		ret = false;
	}
	return ret;
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

bool Unit::Move(float dt)
{
	bool ret = true;

	if (state == IDLE)
	{
		state = MOVE;
		App->entityManager->UpdateCurrentFrame(this);
	}

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
		newPos += {4, 4};

		SetTarget(newPos.x, newPos.y);
		return true;
	}
	if (state == MOVE)
	{
		state = IDLE;
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
void Unit::SetTarget(int x, int y)
{
	target.x = x;
	target.y = y;
	targetReached = false;
}

void Unit::SetType(Unit_Type _type)
{
	type = _type;
}

void Unit::SetMaxSpeed(float speed)
{
	maxSpeed = speed;
}

void Unit::SetPriority(int _priority)
{
	priority = _priority;
}

void Unit::GetTextureRect(SDL_Rect& rect, SDL_RendererFlip& flip) const
{
	int rectX;

	float angle = currentVelocity.GetAngle() - 90;
	if (angle < 0)
		angle = 360 + angle;
	angle = 360 - angle;
	int direction = angle / (360 / 32);

	if (direction > 16)
	{
		flip = SDL_FLIP_HORIZONTAL;
		direction -= 16;
		rectX = 17 * 64 - direction * 64;
	}
	else
	{
		flip = SDL_FLIP_NONE;
		rectX = direction * 64;
	}

	int rectY = 0;

	rectY = (int)currentFrame * 64;



	rect = { rectX, rectY, 64, 64 };
}

C_Vec2<float> Unit::GetVelocity() const
{
	return currentVelocity;
}
Unit_Type Unit::GetType() const
{
	return type;
}

Unit_Movement_State Unit::GetState() const
{
	return state;
}

void Unit::Destroy()
{
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
}

void Unit::SetNewPath(std::vector<iPoint>& newPath)
{
	path.clear();
	if (newPath.size() > 0)
	{
		for (int n = 0; n < newPath.size(); n++)
		{
			path.push_back(newPath[n]);
		}
		targetReached = false;
		currentNode = -1;
		GetNewTarget();
	}
}

void Unit::UpdateCollider()
{
	collider.x = round(position.x - collider.w / 2);
	collider.y = round(position.y - collider.h / 2);
	int size = App->entityManager->GetUnitSprite(type)->size / 2;
	sprite.position = { (int)round(position.x - size ), (int)round(position.y - size)};
	sprite.y_ref = position.y;
}

void Unit::UpdateBarPosition()
{
	HPBar_Empty->localPosition.x = position.x - 50;
	HPBar_Empty->localPosition.y = position.y - 70;
	HPBar_Filled->localPosition.x = position.x - 48;
	HPBar_Filled->localPosition.y = position.y - 68;

	if (movementType == FLYING)
	{
		HPBar_Empty->localPosition.y -= 20;
		HPBar_Filled->localPosition.y -= 20;
	}
	HPBar_Empty->UpdateSprite();
	HPBar_Filled->UpdateSprite();
}

void Unit::Draw(float dt)
{
	if (App->entityManager->render)
	{
		//if (selected)
		//	App->render->Blit(App->entityManager->unit_base, (int)round(position.x - 32), (int)round(position.y) - 32, true, NULL);
		App->entityManager->UpdateSpriteRect(this, sprite.section, sprite.flip, dt);
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
	lineX1 = line.position.x;
	lineY1 = line.position.y;
	lineX2 = (line.x * 30 + lineX1);
	lineY2 = (line.y * 30 + lineY1);
	App->render->AddLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, true, 0, 255, 0);
	
	//Desired velocity vector: red
	C_Vec2<float> line1 = desiredVelocity;
	line1.Normalize();
	line1 *= 3;
	lineX1 = line1.position.x;
	lineY1 = line1.position.y;
	lineX2 = (line1.x * 30 + lineX1);
	lineY2 = (line1.y * 30 + lineY1);
	App->render->AddLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, true, 255, 0, 0);
	
	SDL_Rect rect = collider;
	App->render->AddRect(rect, true, 0, 255, 0, 255, false);
	App->render->AddRect(rect, true, 0, 255, 0, 255, false);
	//Target position
	App->render->DrawCircle(position.x, position.y, targetRadius, true, 255, 255, 0, 255);

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