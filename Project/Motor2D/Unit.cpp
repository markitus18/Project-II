#include <stdlib.h>

#include "Entity.h"
#include "Controlled.h"
#include "Unit.h"

#include "M_Render.h"
#include "j1App.h"
#include "M_Map.h"

#include "EntityManager.h"
#include "S_SceneMap.h"
#include "UI_Element.h"
#include "M_Gui.h"
//Scene Unit shouldnt be necessary to include after removing draw condition
#include "S_SceneUnit.h"

#include "UIElements.h"
#include "M_PathFinding.h"
#include "C_Vec2.h"

Unit::Unit() :Controlled()
{

}

Unit::Unit(float x, float y) : Controlled(x, y)
{

}
Unit::Unit(fPoint pos) : Controlled(pos)
{

}
Unit::~Unit()
{}

bool Unit::Start()
{
	currentVelocity.x = position.x;
	currentVelocity.y = position.y;
	currentVelocity.y = currentVelocity.x = 1;

	currentVelocity.SetAngle(30);

	currentVelocity.Normalize();
	currentVelocity *= maxSpeed;

	iPoint pos;
	pos.x = (int)position.x;
	pos.y = (int)position.y;

	HPBar_Empty->localPosition.x = position.x;
	HPBar_Empty->localPosition.y = position.y - 60;

	texture = App->entityManager->GetTexture(type);
	return true;
}

bool Unit::Update(float dt)
{
	bool ret = true;

	if (!targetReached)
	{
		if (UpdateVelocity(dt))
		{
			if (!Move(dt))
				targetReached = true;
		}

		iPoint pos;
		pos.x = (int)position.x;
		pos.y = (int)position.y;
	}
	if (targetReached)
	{
		GetNewTarget();
	}

	currentVelocity.position = position;
	desiredVelocity.position = position;

	UpdateBarPosition();
	UpdateBarTexture();

	Draw();

	if (currHP <= 0)
	{
		ret = false;
	}
	return ret;
}

bool Unit::UpdateVelocity(float dt)
{
	bool ret = true;
	GetDesiredVelocity(desiredVelocity);
	if (App->entityManager->smooth)
	{
		if (!isAngleReached())
		{
			Rotate(dt);
			ret = false;
		}
	}
	else
	{
		currentVelocity = GetcurrentVelocity();
	}
	return ret;
}

//Get the desired velocity: target position - current position
bool Unit::GetDesiredVelocity(C_Vec2<float>& newDesiredVelocity)
{
	bool ret = true;
	C_Vec2<float> velocity;

	velocity.x = (target.x - position.x);
	velocity.y = (target.y - position.y);

	velocity.Normalize();
	velocity *= maxSpeed;
	newDesiredVelocity = velocity;

	return ret;
}

//Get the current velocity
C_Vec2<float> Unit::GetcurrentVelocity()
{
	C_Vec2<float> velocity;

	velocity = desiredVelocity;

	velocity.position = position;

	velocity.Normalize();
	velocity *= maxSpeed;

	return velocity;
}

bool Unit::Move(float dt)
{
	bool ret = true;
	C_Vec2<float> vel = currentVelocity * dt;
	//Continuous evaluation
	if (App->entityManager->continuous)
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
			if (isTargetReached())
				ret = false;
		}
		if (ret)
		{
			position.x += rest.x;
			position.y += rest.y;
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

void Unit::Rotate(float dt)
{
	bool ret = true;
	int positive = 1;

	float currentAngle = currentVelocity.GetAngle();
	float desiredAngle = desiredVelocity.GetAngle();
	float diffVel = abs(currentAngle - desiredAngle);

	//Getting the direction of the rotation
	bool currBigger = (currentAngle > desiredAngle);
	bool difBigger = (diffVel > 180);
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
	isAngleReached();
}

bool Unit::GetNewTarget()
{
	bool ret = false;
	if ((uint)currentNode + 1 < path.Count())
	{
		currentNode++;
		iPoint newPos;
		if (!path[currentNode].converted)
		{
			newPos = App->map->MapToWorld(path[currentNode].point.x, path[currentNode].point.y);
			newPos += iPoint{ 4, 4 }; //If we use big tiles offset should be corrected
		}
		else
		{
			newPos = { path[currentNode].point.x, path[currentNode].point.y };
		}

		SetTarget(newPos.x, newPos.y);
		ret = true;
	}
	return ret;
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
		rectX = 17 * 76 - direction * 76;
	}
	else
	{
		flip = SDL_FLIP_NONE;
		rectX = direction * 76;
	}
	rect = { rectX, 0, 76, 76 };
}

Unit_Type Unit::GetType()
{
	return type;
}

void Unit::Destroy()
{
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
}

void Unit::SetNewPath(C_DynArray<PathNode>& newPath)
{
	path.Clear();
	path += newPath;
	targetReached = false;
	currentNode = -1;
	GetNewTarget();
}

void Unit::Draw()
{
	SDL_Rect rect = {0, 0, 76, 76 };
	SDL_RendererFlip flip = SDL_FLIP_NONE;

	if (App->sceneMap->renderUnits)
	{
		if (selected)
			App->render->Blit(App->entityManager->unit_base, (int)round(position.x - 32), (int)round(position.y) - 32, true, NULL);
		GetTextureRect(rect, flip);
		int positionY = (int)round(position.y - 38);
//		if (movementType == FLYING)
//			positionY -= 30;
	
		App->render->Blit(texture, (int)round(position.x - 38), positionY, true, &rect, flip);
	}

	//Should be independent from scene
	if (App->sceneMap->renderForces)
		DrawDebug();
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
	App->render->DrawLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, true, 0, 255, 0);
	
	//Desired velocity vector: red
	C_Vec2<float> line1 = desiredVelocity;
	line1.Normalize();
	line1 *= 3;
	lineX1 = line1.position.x;
	lineY1 = line1.position.y;
	lineX2 = (line1.x * 30 + lineX1);
	lineY2 = (line1.y * 30 + lineY1);
	App->render->DrawLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, true, 255, 0, 0);
	
	//Target position
	App->render->DrawCircle(target.x, target.y, 10, true, 255, 255, 255);
	//Unit position
	App->render->DrawCircle((int)round(position.x), (int)round(position.y), 10, true, 255, 255, 255, 255);

	//Path
	if (path.Count() > 0)
	{
		for (uint i = 0; i < path.Count(); i++)
		{
			iPoint position = App->map->MapToWorld(path[i].point.x, path[i].point.y);
			SDL_Rect pos = { position.x, position.y, 8, 8 };
			SDL_Rect rect = { 0, 0, 64, 64 };
			if (i < (uint)currentNode)
				rect = { 0, 0, 64, 64 };
			App->render->Blit(App->sceneMap->debug_tex, &pos, true, &rect);
		}
	}
}
/*
void Unit::CreateBar()
{
	UIRect* rect1 = App->gui->CreateRect("testRect1", { 0, 0, 150, 20 }, 0, 0, 0);
	UIRect* rect2 = App->gui->CreateRect("testRect1", { 5, 5, 140, 10 }, 255, 0, 0);
	HPBar = App->gui->CreateBar("HP Bar", (UIElement*)rect1, (UIElement*)rect2, &maxHP, &currHP);
	HPBar->SetCamera(true);
}
*/