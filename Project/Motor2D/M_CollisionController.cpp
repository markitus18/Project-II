#include "M_CollisionController.h"

#include "j1App.h"

#include "M_Input.h"
#include "M_Textures.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_PathFinding.h"
#include "Unit.h"
#include "Entity.h"
#include "M_EntityManager.h"

M_CollisionController::M_CollisionController(bool start_enabled) : j1Module(start_enabled)
{
	name.create("collision_controller");
}

// Destructor
M_CollisionController::~M_CollisionController()
{}

// Called before render is available
bool M_CollisionController::Awake(pugi::xml_node& node)
{

	return true;
}

// Called before the first frame
bool M_CollisionController::Start()
{
	return true;
}

// Called each loop iteration
bool M_CollisionController::PreUpdate()
{

	return true;
}

// Called each loop iteration
bool M_CollisionController::Update(float dt)
{
	DoUnitLoop();
	return true;
}

// Called each loop iteration
bool M_CollisionController::PostUpdate(float dt)
{
	bool ret = true;
	
	return ret;
}

bool M_CollisionController::CleanUp()
{
	return true;
}

void M_CollisionController::ManageInput(float dt)
{

}

void M_CollisionController::DoUnitLoop()
{
	std::list<Unit*>::iterator it = App->entityManager->unitList.begin();
	while (it != App->entityManager->unitList.end())
	{
		if ((*it)->GetState() == MOVEMENT_MOVE)
		{
			if (mapChanged)
			{
				bool stop = false;
				for (int n = (*it)->currentNode; n < (*it)->path.size(); n++)
				{
					if (!App->pathFinding->IsWalkable((*it)->path[n].x, (*it)->path[n].y))
					{
						stop = true;
						std::vector<iPoint> newPath;
						iPoint unitPos = App->pathFinding->WorldToMap((*it)->GetPosition().x, (*it)->GetPosition().y);
						App->pathFinding->GetNewPath(unitPos, (*it)->path[(*it)->path.size() - 1], &newPath);
						(*it)->Move((*it)->path[(*it)->path.size() - 1]);
					}
				}
			}
		}
		else
		{
			iPoint unitPos = App->pathFinding->WorldToMap((*it)->GetPosition().x, (*it)->GetPosition().y);
			if (!App->pathFinding->IsWalkable(unitPos.x, unitPos.y))
			{
				LOG("Unit in no-walkable tile");
				iPoint tile = FindClosestWalkable(unitPos.x, unitPos.y);
				iPoint dst = App->pathFinding->MapToWorld(tile.x, tile.y);
				(*it)->SetTarget(dst.x, dst.y);
				(*it)->path.clear();
			}
			else if((*it)->GetState() == MOVEMENT_IDLE)
			{
				bool stop = false;
				std::list<Unit*>::iterator it2 = App->entityManager->unitList.begin();
				while (it2 != App->entityManager->unitList.end())
				{
					if (*it != *it2 && (*it2)->targetReached)
					{
						if (DoUnitsIntersect(*it, *it2))
						{
							if ((*it)->priority > (*it2)->priority)
								SplitUnits(*it, *it2);
							else
							{
								SplitUnits(*it2, *it);
								stop = true;
							}
							LOG("Units overlapping");
						}
					}
					it2++;
				}
			}
		}
		it++;
	}
	if (mapChanged)
		mapChanged = false;
}

iPoint M_CollisionController::FindClosestWalkable(int x, int y)
{
	bool found = false;
	int lenght = 3;

	iPoint tile = { x + lenght / 2, y - lenght / 2 - 1 };

	while (!found && lenght < 20)
	{
		while (tile.y < y + lenght / 2 && !found)
		{
			tile.y++;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;
		}

		while (tile.x > x - lenght / 2 && !found)
		{
			tile.x--;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;
		}

		while (tile.y > y - lenght / 2 && !found)
		{
			tile.y--;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;

		}

		while (tile.x < x + lenght / 2 && !found)
		{
				tile.x++;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;
		}
		lenght+=2;
	}
	return tile;
}

bool M_CollisionController::DoUnitsIntersect(Unit* unit1, Unit* unit2)
{
	C_Vec2<float> distance = { unit1->GetPosition().x - unit2->GetPosition().x, unit1->GetPosition().y - unit2->GetPosition().y };
	return (distance.GetModule() < unit1->colRadius + unit2->colRadius);
}

//Higher priority unit is unit1, we will move unit2
void M_CollisionController::SplitUnits(Unit* unit1, Unit* unit2)
{
	C_Vec2<float> vec = { unit2->GetPosition().x - unit1->GetPosition().x, unit2->GetPosition().y - unit1->GetPosition().y };
	vec.position = unit1->GetPosition();
	vec.Normalize();
	vec *= unit1->colRadius + unit2->colRadius + 1;
	if (vec.GetModule() == 0)
	{
		vec.x = unit1->colRadius + unit2->colRadius + 1;
		vec.SetAngle(rand() % 360);
	}

	fPoint pos = vec.position + fPoint{ vec.x, vec.y };
	iPoint tile = App->pathFinding->WorldToMap(pos.x, pos.y);
	iPoint dst = App->pathFinding->MapToWorld(tile.x, tile.y);

	int loops = 0;
	while (!App->pathFinding->IsWalkable(tile.x, tile.y) && loops < 24)
	{
		vec.SetAngle(vec.GetAngle() + 15);
		pos = vec.position + fPoint{ vec.x, vec.y };
		tile = App->pathFinding->WorldToMap(pos.x, pos.y);
		dst = App->pathFinding->MapToWorld(tile.x, tile.y);
		loops++;
	}

	unit2->SetTarget(pos.x, pos.y);
	unit2->path.clear();
}