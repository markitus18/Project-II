#include <stdlib.h>

#include "Unit.h"

#include "j1Render.h"
#include "j1App.h"
#include "j1Map.h"
#include "Entity.h"
#include "j1Textures.h"
#include "EntityManager.h"
#include "j1SceneUnit.h"
#include "j1Gui.h"
#include "UIElements.h"
#include "j1Pathfinding.h"

Unit::Unit()
{
	position.x = position.y = 0;
	CreateBar();
}
Unit::Unit(int x, int y)
{
	position.x = x;
	position.y = y;
	CreateBar();
}
Unit::~Unit()
{}

bool Unit::Start()
{
	HPBar->Center(position);
	HPBar->SetLocalPosition(HPBar->GetLocalPosition().x, HPBar->GetLocalPosition().y - 60);

	return true;
}

bool Unit::Update(float dt)
{
	if (targetChange)
	{
		UpdateVelocity(dt);
		position.x += (int)currentVelocity.x;
		position.y += (int)currentVelocity.y;

		HPBar->Center(position);
		HPBar->SetLocalPosition(HPBar->GetLocalPosition().x, HPBar->GetLocalPosition().y - 60);
	}
	else
	{
		GetNewTarget();
	}

	Draw();
	return true;
}

void Unit::UpdateVelocity(float dt)
{
	if (GetDesiredVelocity(desiredVelocity))
	//	steeringVelocity = GetSteeringVelocity();
	currentVelocity = GetcurrentVelocity(dt);
}

//Get the desired velocity: target position - entity position
bool Unit::GetDesiredVelocity(p2Vec2<float>& newDesiredVelocity)
{
	bool ret = true;
	p2Vec2<float> velocity;
	velocity.position.x = (float)position.x;
	velocity.position.y = (float)position.y;
	velocity.x = (float)(target.x - position.x);
	velocity.y = (float)(target.y - position.y);

	float distance = velocity.GetModule();

	while (distance < slowingRadius)
	{
		position = target;
		targetChange = false;
		if (GetNewTarget())
		{
			velocity.x = (float)(target.x - position.x);
			velocity.y = (float)(target.y - position.y);
			distance = velocity.GetModule();
		}
		else
		{
			velocity.SetToZero();
			ret = false;
			break;
		}
	}
	velocity.Normalize();
	velocity *= maxSpeed;

	newDesiredVelocity = velocity;
	return ret;
}

//Get the steering velocity: current velocity - desired velocity
/*
p2Vec2<float> Entity::GetSteeringVelocity()
{
p2Vec2<float> velocity;
velocity = desiredVelocity - currentVelocity;
velocity /= maxForce;
if (behaviour == RUN)
{
velocity.Negate();
}
return velocity;
}
*/
p2Vec2<float> Unit::GetcurrentVelocity(float dt)
{
	p2Vec2<float> velocity;
	velocity = desiredVelocity;//currentVelocity + steeringVelocity;
	velocity.position.x = (float)position.x;
	velocity.position.y = (float)position.y;
	velocity *= 300.0f * dt;
	/*
	if (velocity.IsOpposite(desiredVelocity))
	{
	velocity.x += 0.1f;
	velocity.y += 0.1f;
	}
	*/
	return velocity;
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

void Unit::SetTarget(int x, int y)
{
	target.x = x;
	target.y = y;
	targetChange = true;
}

void Unit::SetType(UnitType _type)
{
	type = _type;
}

void Unit::SetLevel(int _level)
{
	level = _level;
}

void Unit::SetMaxSpeed(float speed)
{
	maxSpeed = speed;
}

float Unit::GetSlowRad()
{
	return slowingRadius;
}

Entity_Directions Unit::GetDirection()
{
	Entity_Directions direction = UP;
	float angle = currentVelocity.GetAngle();

	if (angle <= 0 && angle > -90)
		direction =	UP;
	else if (angle <= -90 && angle > -180)
		direction =	LEFT;
	else if (angle > 0 && angle < 90)
		direction =	RIGHT;
	else if (angle > 90 && angle <= 180)
		direction = DOWN;
	
	return direction;
}

UnitType Unit::GetType()
{
	return type;
}

int Unit::GetLevel()
{
	return level;
}

void Unit::SetNewPath(p2DynArray<PathNode>& newPath)
{
	path.Clear();
	path += newPath;
	targetChange = true;
	currentNode = -1;
	GetNewTarget();
}

void Unit::Draw()
{
	App->render->Blit(App->entityManager->unit_base, position.x - 32, position.y - 16);
	App->render->Blit(App->entityManager->entity_tex, position.x - 32, position.y - 55, new SDL_Rect{ 256 * (level - 1)  + 64 * GetDirection(), 70 * type, 65, 70 });// , 1.0f, GetDirection());
	
	if (selected)
	{
		if (HPBar->active == false)
			HPBar->Activate();
	}
	else
	{
		if (HPBar->active == true)
			HPBar->Deactivate();
	}
	if (App->sceneUnit->renderForces)
		DrawDebug();
}

void Unit::DrawDebug()
{
	//Direction Line
	float lineX1, lineX2, lineY1, lineY2;
	p2Vec2<float> line = currentVelocity;
	line.Normalize();
	line *= 3;
	lineX1 = line.position.x;
	lineY1 = line.position.y;
	lineX2 = (line.x * 30 + lineX1);
	lineY2 = (line.y * 30 + lineY1);
	App->render->DrawLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, 0, 255, 0);
	/*
	p2Vec2<float> line1 = currentVelocity;
	line1 = desiredVelocity;
	line1.Normalize();
	line1 *= 3;
	lineX1 = line1.position.x + App->render->camera.x;
	lineY1 = line1.position.y + App->render->camera.y;
	lineX2 = (line1.x * 30 + lineX1);
	lineY2 = (line1.y * 30 + lineY1);
	App->render->DrawLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, 0, 255, 0);
	*/
	//Target position
	App->render->DrawCircle((int)target.x, (int)target.y, (int)GetSlowRad(), 255, 255, 255);
	//Unit position
	App->render->DrawCircle(position.x, position.y, 10, 255, 255, 255, 255);

	//Path
	if (path.Count() > 0)
	{
		for (uint i = 0; i < path.Count(); i++)
		{
			iPoint position = App->map->MapToWorld(path[i].point.x, path[i].point.y);
			SDL_Rect rect = { 0, 0, 64, 32 };
			if (i < (uint)currentNode)
				rect = { 0, 64, 64, 32 };
			App->render->Blit(App->entityManager->path_tex, position.x - 32, position.y - 16, &rect);
		}
	}
}

void Unit::CreateBar()
{
	UIRect* rect1 = App->gui->CreateRect("testRect1", { 0, 0, 150, 20 }, 0, 0, 0);
	UIRect* rect2 = App->gui->CreateRect("testRect1", { 5, 5, 140, 10 }, 255, 0, 0);
	HPBar = App->gui->CreateBar("HP Bar", (UIElement*)rect1, (UIElement*)rect2, &maxHP, &HP);
	HPBar->SetIgnoreCamera();
}