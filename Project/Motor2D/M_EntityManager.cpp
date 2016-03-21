#include "M_EntityManager.h"

#include "j1App.h"
#include "M_Textures.h"
#include "Unit.h"
#include "Building.h"
#include "M_Render.h"
#include "M_Input.h"
#include "M_Map.h"
#include "M_PathFinding.h"
#include "S_SceneMap.h"
#include "M_CollisionController.h"

M_EntityManager::M_EntityManager(bool start_enabled) : j1Module(start_enabled)
{

}

M_EntityManager::~M_EntityManager()
{

}
bool M_EntityManager::PreStart(pugi::xml_node& node)
{
	entity_tex = App->tex->Load("graphics/protoss/units/dark templar.png");
	unit_base = App->tex->Load("graphics/ui/o048.png");
	path_tex = App->tex->Load("textures/path.png");
	hpBar_empty = App->tex->Load("graphics/ui/hpbarempt.png");
	hpBar_filled = App->tex->Load("graphics/ui/hpbarfull.png");
	hpBar_mid = App->tex->Load("graphics/ui/hpbarmid.png");;
	hpBar_low = App->tex->Load("graphics/ui/hpbarlow.png");;
	building_base = App->tex->Load("graphics/protoss/units/pylon.png");
	return true;
}

bool M_EntityManager::Start()
{
	return true;
}

bool M_EntityManager::Update(float dt)
{
	ManageInput();

	DoUnitLoop(dt);
	UpdateSelectionRect();
	/*
	C_List_item<Building*>* item = NULL;
	item = buildingList.start;

	while (item)
	{
	item->data->Update(dt);
	App->render->Blit(entity_tex, item->data->getPosition().x, item->data->getPosition().y, new SDL_Rect{ 0, 0, 20, 20 }, 1.0f, item->data->GetDirection());

	item = item->next;
	}
	*/

	if (App->sceneMap->renderForces)
		DrawDebug();

	return true;
}

bool M_EntityManager::PostUpdate(float dt)
{
	if (selectionRect.w != 0 || selectionRect.h != 0)
		App->render->DrawQuad(selectionRect, false, 0, 255, 0, 255, false);

	if (!unitsToDelete.empty())
	{
		std::list<Unit*>::iterator it = unitsToDelete.begin();
		while (it != unitsToDelete.end())
		{
			deleteUnit(it);
			it++;
		}
		unitsToDelete.clear();
	}
	return true;
}

bool M_EntityManager::CleanUp()
{
	std::list<Unit*>::iterator it = unitList.begin();
	while (it != unitList.end())
	{
		RELEASE (*it);
		it++;
	}
	unitList.clear();
	selectedUnits.clear();
	unitsToDelete.clear();

	return true;
}

void M_EntityManager::DoUnitLoop(float dt)
{
	std::list<Unit*>::iterator it = unitList.begin();
	while (it != unitList.end())
	{
		if (selectUnits)
		{
		//	if (selectionRect.w != 0 || selectionRect.h != 0)
		//	{
				//Selecting units
				if (IsUnitSelected(it))
				{
					if ((*it)->selected == false)
					{
						SelectUnit(it);
					}
				}
				else if ((*it)->selected == true)
				{
					UnselectUnit(it);
				}
		//	}
		//	else if (selectionRect.w == selectionRect.h == 0)
		//	{

		//	}
		}

		//Unit update
		if (!(*it)->Update(dt))
		{
			unitsToDelete.push_back(*it);
		}
		it++;
	}

	if (selectUnits)
	{
		selectUnits = false;
		selectionRect.w = selectionRect.h = 0;
	}
}

void M_EntityManager::UpdateSelectionRect()
{
	std::list<Unit*>::iterator it = selectedUnits.begin();

	int minX = 100000, minY = 100000, maxX = 0, maxY = 0;
	while (it != selectedUnits.end())
	{
		if ((*it)->GetPosition().x < minX)
			minX = (*it)->GetPosition().x;
		if ((*it)->GetPosition().y < minY)
			minY = (*it)->GetPosition().y;
		if ((*it)->GetPosition().x > maxX)
			maxX = (*it)->GetPosition().x;
		if ((*it)->GetPosition().y > maxY)
			maxY = (*it)->GetPosition().y;

		it++;
	}

	groupRect = { minX, minY, maxX - minX, maxY - minY };
}

void M_EntityManager::ManageInput()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		App->input->GetMousePosition(selectionRect.x, selectionRect.y);
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		selectionRect.w = x - selectionRect.x;
		selectionRect.h = y - selectionRect.y;
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		selectUnits = true;
	}
}

Unit* M_EntityManager::CreateUnit(int x, int y, Unit_Type type)
{
	iPoint tile = App->map->WorldToMap(x, y);
	bool isWalkable = App->pathFinding->IsWalkable(tile.x, tile.y);
	if (isWalkable)
	{
		Unit* unit = new Unit(x, y);
		unit->SetType(type);

		switch (type)
		{
		case (ARBITER):
			{
			unit->SetMovementType(FLYING);
			unit->SetCollider({ 0, 0, 5 * 8, 5 * 8 });
			}
		}
		unit->SetPriority(currentPriority++);
		unit->Start();

		AddUnit(unit);
		return unit;
	}
	return NULL;

}

bool M_EntityManager::deleteUnit(std::list<Unit*>::iterator it)
{
	(*it)->Destroy();
	if ((*it)->selected)
	{
		selectedUnits.remove(*it);
	}
	unitList.remove(*it);
	RELEASE (*it);


	return true;
}


bool M_EntityManager::IsUnitSelected(std::list<Unit*>::const_iterator it) const
{
	SDL_Rect itemRect = (*it)->GetCollider();
	itemRect.x += App->render->camera.x;
	itemRect.y += App->render->camera.y;
	SDL_Rect rect = selectionRect;

	//Fixing negative values
	if (rect.h < 0)
	{
		rect.y += rect.h;
		rect.h *= -1;
	}
	if (rect.w < 0)
	{
		rect.x += rect.w;
		rect.w *= -1;
	}
	if (rect.w == 0 && rect.h == 0)
	{
		return (rect.x > itemRect.x && rect.y > itemRect.y && rect.x < itemRect.x + itemRect.w && rect.y < itemRect.y + itemRect.h);
	}
	else
	{
		return (SDL_HasIntersection(&rect, &itemRect));
	}

}

void M_EntityManager::SendNewPath(int x, int y)
{
	if (App->pathFinding->IsWalkable(x, y))
	{
		//Moving group rectangle to the destination point
		iPoint Rcenter = App->map->MapToWorld(x, y);
		destinationRect = { Rcenter.x - groupRect.w / 2, Rcenter.y - groupRect.h / 2, groupRect.w, groupRect.h };
	
		//Iteration through all selected units
		std::list<Unit*>::iterator it = selectedUnits.begin();

		while (it != selectedUnits.end())
		{
			std::vector<iPoint> newPath;

			//Distance from rectangle position to unit position
			iPoint posFromRect;
			posFromRect.x = (*it)->GetPosition().x - groupRect.x;
			posFromRect.y = (*it)->GetPosition().y - groupRect.y;

			//Destination tile: destination rectangle + previous distance
			iPoint dstTile = App->map->WorldToMap(destinationRect.x + posFromRect.x, destinationRect.y + posFromRect.y);

			//Unit tile position
			fPoint unitPos = (*it)->GetPosition();
			iPoint unitTile = App->map->WorldToMap(round(unitPos.x), round(unitPos.y));

			//If destination is not walkable, use the player's clicked tile
			if (!App->pathFinding->IsWalkable(dstTile.x, dstTile.y))
				dstTile = { x, y };

			//If a path is found, send it to the unit
			App->pathFinding->GetNewPath(unitTile, dstTile, newPath);
			(*it)->SetNewPath(newPath);

			it++;
		}
	}
}

SDL_Texture* M_EntityManager::GetTexture(Unit_Type type)
{
	switch (type)
	{
	case (ARBITER):
		return entity_tex;
		break;
	default:
		return NULL;
		break;
	}
}

SDL_Texture* M_EntityManager::GetTexture(Building_Type type)
{
	switch (type)
	{
	case (PYLON) :
		return entity_tex;
		break;
	default:
		return NULL;
		break;
	}
}

void M_EntityManager::AddUnit(Unit* unit)
{
	unitList.push_back(unit);
}

void M_EntityManager::SelectUnit(std::list<Unit*>::iterator it)
{
	(*it)->selected = true;
	(*it)->UpdateBarState();
	selectedUnits.push_back(*it);
}

void M_EntityManager::UnselectUnit(std::list<Unit*>::iterator it)
{
	(*it)->selected = false;
	(*it)->UpdateBarState();
	selectedUnits.remove(*it);
}

void M_EntityManager::DrawDebug()
{
	if (!selectedUnits.empty())
		App->render->DrawQuad(groupRect, true, 255, 0, 0, 255, false);

	App->render->DrawQuad(destinationRect, true, 255, 255, 0, 255, false);
}