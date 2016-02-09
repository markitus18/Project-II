#include "j1App.h"
#include "j1Textures.h"
#include "Unit.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "EntityManager.h"

EntityManager::EntityManager(bool start_enabled) : j1Module(start_enabled)
{

}

EntityManager::~EntityManager()
{

}

bool EntityManager::Start()
{
	entity_tex = App->tex->Load("textures/unit.png");
	unit_base = App->tex->Load("textures/unit_base.png");
	path_tex = App->tex->Load("textures/path.png");

	for (uint i = 0; i < unitList.count(); i++)
	{
		unitList[i]->Start();
	}

	return true;
}

bool EntityManager::Update(float dt)
{
	ManageInput();

	p2List_item<Unit*>* item = NULL;
	item = unitList.start;

	while (item)
	{
		if (selectionRect.w != 0 || selectionRect.h != 0)
		{
			if (IsUnitSelected(item))
			{
				if (item->data->selected == false)
				{
					item->data->selected = true;
					selectedUnits.add(item->data);
				}
			}

			else if (item->data->selected == true)
			{
				item->data->selected = false;
				selectedUnits.del(selectedUnits.At(selectedUnits.find(item->data)));
			}

		}
		item->data->Update(dt);
		item = item->next;
	}
	
	/*
	p2List_item<Building*>* item = NULL;
	item = buildingList.start;

	while (item)
	{
	item->data->Update(dt);
	App->render->Blit(entity_tex, item->data->getPosition().x, item->data->getPosition().y, new SDL_Rect{ 0, 0, 20, 20 }, 1.0f, item->data->GetDirection());

	item = item->next;
	}
	*/

	ManageInput();
	return true;
}

bool EntityManager::PostUpdate(float dt)
{
	if (selectionRect.w != 0 || selectionRect.h != 0)
		App->render->DrawQuad(selectionRect, 255, 255, 255, 255, false, false);
	return true;
}
bool EntityManager::CleanUp()
{
	p2List_item<Unit*>* item = NULL;
	item = unitList.start;
	while (item)
	{
		delete item->data;
		item = item->next;
	}
	return true;
}

void EntityManager::ManageInput()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_UP)
	{
		for (uint i = 0; i < unitList.count(); i++)
		{
			if (unitList[i]->GetLevel() < 2)
				unitList[i]->SetLevel(unitList[i]->GetLevel() + 1);
		}
	}
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
		selectionRect.w = selectionRect.h = 0;
	}
}

Unit* EntityManager::CreateUnit(int x, int y, UnitType type)
{
	Unit* unit = new Unit(x, y);
	unit->SetType(type);

	unitList.add(unit);
	return unit;
}

bool deleteUnit()
{
	return true;
}


bool EntityManager::IsUnitSelected(p2List_item<Unit*>* unit)
{
	iPoint itemPos = unit->data->GetPosition();
	itemPos.x += App->render->camera.x;
	itemPos.y += App->render->camera.y;
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
	if (itemPos.x > rect.x && itemPos.x < rect.x + rect.w && itemPos.y > rect.y && itemPos.y < rect.y + rect.h)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void EntityManager::SendNewPath(int x, int y)
{
	for (uint i = 0; i < selectedUnits.count(); i++)
	{
		p2DynArray<PathNode> newPath;
		iPoint unitPos = selectedUnits[i]->GetPosition();
		iPoint unitTile = App->map->WorldToMap(unitPos.x, unitPos.y);
		iPoint dstTile = App->map->WorldToMap(x, y);
		if (App->pathFinding->GetNewPath(unitTile, dstTile, newPath))
		{
			newPath[newPath.Count() - 1].point = { x, y };
			newPath[newPath.Count() - 1].converted = true;
			selectedUnits[i]->SetNewPath(newPath);
		}


		//Change color
		if (selectedUnits[i]->GetType() == BLUE)
			selectedUnits[i]->SetType(RED);
		else if (selectedUnits[i]->GetType() == RED)
			selectedUnits[i]->SetType(YELLOW);
		else if (selectedUnits[i]->GetType() == YELLOW)
			selectedUnits[i]->SetType(GREEN);
		else if (selectedUnits[i]->GetType() == GREEN)
			selectedUnits[i]->SetType(BLUE);
	}
}