#include "M_CollisionController.h"

#include "j1App.h"

#include "M_Input.h"
#include "M_Textures.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_Map.h"
#include "M_PathFinding.h"
#include "Unit.h"
#include "Entity.h"
#include "M_EntityManager.h"
#include "UIElements.h"

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
	for (int i = 0; i < App->entityManager->unitList.count(); i++)
	{
		Unit* unit = App->entityManager->unitList[i];
		if (!App->entityManager->unitList[i]->targetReached)
		{
			if (mapChanged)
			{
				bool stop = false;
				for (int n = App->entityManager->unitList[i]->currentNode; n < App->entityManager->unitList[i]->path.Count(); n++)
				{
					if (!App->pathFinding->IsWalkable(unit->path[n].x, unit->path[n].y))
					{
						stop = true;
						C_DynArray<iPoint> newPath;
						iPoint unitPos = App->map->WorldToMap(unit->GetPosition().x, unit->GetPosition().y);
						App->pathFinding->GetNewPath(unitPos, unit->path[unit->path.Count() - 1], newPath);
						unit->SetNewPath(newPath);
					}
				}
			}
		}
		else
		{
			iPoint unitPos = App->map->WorldToMap(unit->GetPosition().x, unit->GetPosition().y);
			if (!App->pathFinding->IsWalkable(unitPos.x, unitPos.y))
			{
				LOG("Unit in no-walkable tile");
			}
			else
			{
				for (int n = 0; n < App->entityManager->unitList.count(); n++)
				{
					if (i != n)
					{
						Unit* unit2 = App->entityManager->unitList[n];
						if (SDL_HasIntersection(&unit->GetCollider(), &unit2->GetCollider()))
						{
							LOG("Units overlapping");
						}
					}
				}
			}
		}
	}
	if (mapChanged)
		mapChanged = false;
}