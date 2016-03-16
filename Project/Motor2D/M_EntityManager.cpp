#include "M_EntityManager.h"

#include "j1App.h"
#include "M_Textures.h"
#include "Unit.h"
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
	entity_tex = App->tex->Load("graphics/protoss/units/arbiter.png");
	unit_base = App->tex->Load("graphics/ui/o062.png");
	path_tex = App->tex->Load("textures/path.png");
	hpBar_empty = App->tex->Load("graphics/ui/hpbarempt.png");
	hpBar_filled = App->tex->Load("graphics/ui/hpbarfull.png");
	hpBar_mid = App->tex->Load("graphics/ui/hpbarmid.png");;
	hpBar_low = App->tex->Load("graphics/ui/hpbarlow.png");;
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

	if (unitsToDelete.count() > 0)
	{
		C_List_item<Unit*>* item;
		C_List_item<Unit*>* item2;
		for (item = unitsToDelete.start; item; item = item2)
		{
			item2 = item->next;
			deleteUnit(item);

		}
	}
	return true;
}

bool M_EntityManager::CleanUp()
{
	C_List_item<Unit*>* item = NULL;
	item = unitList.start;
	while (item)
	{
		delete item->data;
		item = item->next;
	}
	return true;
}

void M_EntityManager::DoUnitLoop(float dt)
{
	C_List_item<Unit*>* item = NULL;
	item = unitList.start;


	while (item)
	{
		if (selectUnits)
		{
			if (selectionRect.w != 0 || selectionRect.h != 0)
			{
				//Selecting units
				if (IsUnitSelected(item))
				{
					if (item->data->selected == false)
					{
						item->data->selected = true;
						item->data->UpdateBarState();
						selectedUnits.add(item->data);
					}
				}
				else if (item->data->selected == true)
				{
					item->data->selected = false;
					item->data->UpdateBarState();
					selectedUnits.del(selectedUnits.At(selectedUnits.find(item->data)));
				}
			}
		}

		//Unit update
		if (!item->data->Update(dt))
		{
			unitsToDelete.add(item->data);
		}
		item = item->next;
	}
	if (selectUnits)
	{

		selectionRect.w = selectionRect.h = 0;
		selectUnits = false;
	}
}

void M_EntityManager::UpdateSelectionRect()
{
	C_List_item<Unit*>* item = NULL;
	item = selectedUnits.start;

	int minX = 100000, minY = 100000, maxX = 0, maxY = 0;
	while (item)
	{
		if (item->data->GetPosition().x < minX)
			minX = item->data->GetPosition().x;
		if (item->data->GetPosition().y < minY)
			minY = item->data->GetPosition().y;
		if (item->data->GetPosition().x > maxX)
			maxX = item->data->GetPosition().x;
		if (item->data->GetPosition().y > maxY)
			maxY = item->data->GetPosition().y;

		item = item->next;
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

bool M_EntityManager::deleteUnit(C_List_item<Unit*>* item)
{
	item->data->Destroy();
	if (item->data->selected)
	{
		selectedUnits.del(selectedUnits.At(selectedUnits.find(item->data)));
	}
	unitList.del(unitList.At(unitList.find(item->data)));
	unitsToDelete.del(item);
	return true;
}


bool M_EntityManager::IsUnitSelected(C_List_item<Unit*>* unit)
{
	SDL_Rect itemRect = unit->data->GetCollider();
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
	if (SDL_HasIntersection(&rect, &itemRect))
	{
		return true;
	}
	else
	{
		return false;
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
		for (uint i = 0; i < selectedUnits.count(); i++)
		{
			C_DynArray<iPoint> newPath;

			//Distance from rectangle position to unit position
			iPoint posFromRect;
			posFromRect.x = selectedUnits[i]->GetPosition().x - groupRect.x;
			posFromRect.y = selectedUnits[i]->GetPosition().y - groupRect.y;

			//Destination tile: destination rectangle + previous distance
			iPoint dstTile = App->map->WorldToMap(destinationRect.x + posFromRect.x, destinationRect.y + posFromRect.y);

			//Unit tile position
			fPoint unitPos = selectedUnits[i]->GetPosition();
			iPoint unitTile = App->map->WorldToMap(round(unitPos.x), round(unitPos.y));

			//If destination is not walkable, use the player's clicked tile
			if (!App->pathFinding->IsWalkable(dstTile.x, dstTile.y))
				dstTile = { x, y };

			//If a path is found, send it to the unit
			App->pathFinding->GetNewPath(unitTile, dstTile, newPath);
				selectedUnits[i]->SetNewPath(newPath);
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

void M_EntityManager::AddUnit(Unit* unit)
{
	C_List_item<Unit*>* item = NULL;
	C_List_item<Unit*>* unitItem = new C_List_item<Unit*>(unit);

	bool keepGoing = true;
	for (item = unitList.end; item && keepGoing; item = item->prev)
	{
		if (item->data->GetPosition().y < unit->GetPosition().y)
		{
			unitList.Insert(item, unitItem);
			keepGoing = false;
		}
	}
	if (keepGoing)
		unitList.Insert(NULL, unitItem);
}

void M_EntityManager::DrawDebug()
{
	if (selectedUnits.count() > 0)
		App->render->DrawQuad(groupRect, true, 255, 0, 0, 255, false);

	App->render->DrawQuad(destinationRect, true, 255, 255, 0, 255, false);
}