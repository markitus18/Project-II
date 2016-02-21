#include <stdlib.h>

#include "Entity.h"
#include "Unit.h"

#include "j1Render.h"
#include "j1App.h"
#include "j1Map.h"

#include "EntityManager.h"

//Scene Unit shouldnt be necessary to include after removing draw condition
#include "j1SceneUnit.h"

#include "j1Gui.h"
#include "UIElements.h"
#include "j1Pathfinding.h"
#include "p2Vec2.h"

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
	CAP(maxForce, 0, 1);

	HPBar->Center(position);
	HPBar->SetLocalPosition(HPBar->GetLocalPosition().x, HPBar->GetLocalPosition().y - 60);

	return true;
}

bool Unit::Update(float dt)
{
	if (targetChange)
	{
		if (UpdateVelocity(dt))
		{
			if (!Move())
				targetChange = false;
		}

		HPBar->Center(position);
		HPBar->SetLocalPosition(HPBar->GetLocalPosition().x, HPBar->GetLocalPosition().y - 60);
	}
	if (!targetChange)
	{
		GetNewTarget();
	}

	Draw();
	return true;
}

bool Unit::UpdateVelocity(float dt)
{
	bool ret = true;
	bool smooth = false;
	GetDesiredVelocity(desiredVelocity);
	if (smooth)
	{
		float diffVel = abs(desiredVelocity.GetAngle() - currentVelocity.GetAngle());
		if (diffVel > 3.5f)
		{
			steeringVelocity = GetSteeringVelocity();
			currentVelocity = GetcurrentVelocity(dt, true);
			ret = false;
		}
		else
		{
			currentVelocity = GetcurrentVelocity(dt, false);
			ret = true;
		}
	}
	else
	{
		currentVelocity = GetcurrentVelocity(dt, false);
		ret = true;
	}
	return ret;
}

//Get the desired velocity: target position - current position
bool Unit::GetDesiredVelocity(p2Vec2<float>& newDesiredVelocity)
{
	bool ret = true;
	p2Vec2<float> velocity;
	velocity.position.x = (float)position.x;
	velocity.position.y = (float)position.y;
	velocity.x = (float)(target.x - position.x);
	velocity.y = (float)(target.y - position.y);

	velocity.Normalize();
	velocity *= maxSpeed;
	newDesiredVelocity = velocity;
	LOG("Vel angle: %f", velocity.GetAngle());

	return ret;
}

//Get the steering velocity: current velocity - desired velocity
p2Vec2<float> Unit::GetSteeringVelocity()
{
p2Vec2<float> velocity;
velocity = desiredVelocity - currentVelocity;
if (desiredVelocity.IsOpposite(currentVelocity))
{
	currentVelocity.x += 0.5f;
	currentVelocity.y += 0.5f;
}
velocity *= maxForce;

return velocity;
}

//Get the current velocity: if we have a steering velocity, we add it, otherwise
//its the desired velocity. We normalize the resulting velocity later and
//multiply by max speed
p2Vec2<float> Unit::GetcurrentVelocity(float dt, bool isRotating)
{
	p2Vec2<float> velocity;
	if (isRotating)
	{
		velocity = currentVelocity + steeringVelocity;
	}
	else
	{
		velocity = desiredVelocity;
	}

	velocity.position.x = (float)position.x;
	velocity.position.y = (float)position.y;

	velocity.Normalize();
	velocity *= maxSpeed * dt;

	return velocity;
}

bool Unit::Move()
{
	bool ret = true;
	bool continuous = true;

	if (continuous)
	{
		float module = currentVelocity.GetModule();
		int steps = currentVelocity.GetModule() / (slowingRadius / 2);
		p2Vec2<float> velStep = (currentVelocity.GetNormal() * (slowingRadius / 2));
		p2Vec2<float> rest = currentVelocity - currentVelocity.GetNormal() * slowingRadius / 2 * steps;

		for (int i = 0; i < steps && ret; i++)
		{
			position.x += round(velStep.x);
			position.y += round(velStep.y);
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
	else
	{
		position.x += round(currentVelocity.x);
		position.y += round(currentVelocity.y);
		if (isTargetReached())
			ret = false;
	}

	return ret;
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

bool Unit::isTargetReached()
{
	p2Vec2<int> vec;
	vec.x = target.x - position.x;
	vec.y = target.y - position.y;
	float distance = vec.GetModule();
	if (distance < slowingRadius)
	{
		position = target;
		LOG("position: x - %i, y - %i", position.x, position.y);
		return true;
	}
	return false;
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
	SDL_Rect rect = {64 * GetDirection(), 70 * type, 65, 70 };
	if (App->sceneUnit->renderUnits)
	{
		App->render->Blit(App->entityManager->unit_base, position.x - 32, position.y - 16, true);
		App->render->Blit(App->entityManager->entity_tex, position.x - 32, position.y - 55, true, &rect);
	}

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
	//Should be independent from scene
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
	App->render->DrawLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, true, 0, 255, 0);
	
	p2Vec2<float> line1 = desiredVelocity;
	line1.Normalize();
	line1 *= 3;
	lineX1 = line1.position.x;
	lineY1 = line1.position.y;
	lineX2 = (line1.x * 30 + lineX1);
	lineY2 = (line1.y * 30 + lineY1);
	App->render->DrawLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, true, 255, 0, 0);
	
	//Target position
	App->render->DrawCircle((int)target.x, (int)target.y, 10, true, 255, 255, 255);
	//Unit position
	App->render->DrawCircle(position.x, position.y, 10, true, 255, 255, 255, 255);

	//Path
	if (path.Count() > 0)
	{
		for (uint i = 0; i < path.Count(); i++)
		{
			iPoint position = App->map->MapToWorld(path[i].point.x, path[i].point.y);
			SDL_Rect rect = { 0, 0, 64, 32 };
			if (i < (uint)currentNode)
				rect = { 0, 64, 64, 32 };
			App->render->Blit(App->entityManager->path_tex, position.x - 32, position.y - 16, true, &rect);
		}
	}
}

void Unit::CreateBar()
{
	UIRect* rect1 = App->gui->CreateRect("testRect1", { 0, 0, 150, 20 }, 0, 0, 0);
	UIRect* rect2 = App->gui->CreateRect("testRect1", { 5, 5, 140, 10 }, 255, 0, 0);
	HPBar = App->gui->CreateBar("HP Bar", (UIElement*)rect1, (UIElement*)rect2, &maxHP, &HP);
	HPBar->SetCamera(true);
}