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
	if (mapChanged)
	{
		mapChanged = false;
		C_List<Unit*> unitList = App->entityManager->unitList;
		for (int i = 0; i < App->entityManager->unitList.count(); i++)
		{
			if (!unitList[i]->targetReached)
			{
				bool stop = false;
				for (int n = unitList[i]->currentNode; n < unitList[i]->path.Count(); n++)
				{
					if (!App->pathFinding->IsWalkable(unitList[i]->path[n].x, unitList[i]->path[n].y))
					{
						stop = true;
						C_DynArray<iPoint> newPath;
						iPoint unitPos = App->map->WorldToMap(unitList[i]->GetPosition().x, unitList[i]->GetPosition().y);
						App->pathFinding->GetNewPath(unitPos, unitList[i]->path[unitList[i]->path.Count()], newPath);
						unitList[i]->SetNewPath(newPath);
					}

				}
			}
		}
	}
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