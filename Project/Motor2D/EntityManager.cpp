#include "EntityManager.h"

#include "j1App.h"
#include "M_Textures.h"
#include "Unit.h"
#include "M_Render.h"
#include "M_Input.h"
#include "M_Map.h"
#include "M_PathFinding.h"


EntityManager::EntityManager(bool start_enabled) : j1Module(start_enabled)
{

}

EntityManager::~EntityManager()
{

}
bool EntityManager::PreStart(pugi::xml_node& node)
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

bool EntityManager::Start()
{
	return true;
}

bool EntityManager::Update(float dt)
{
	ManageInput();

	C_List_item<Unit*>* item = NULL;
	item = unitList.start;

	while (item)
	{
		if (selectUnits)
		{
			if (selectionRect.w != 0 || selectionRect.h != 0)
			{
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

	ManageInput();
	return true;
}

bool EntityManager::PostUpdate(float dt)
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

bool EntityManager::CleanUp()
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

void EntityManager::ManageInput()
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

Unit* EntityManager::CreateUnit(int x, int y, Unit_Type type)
{
	Unit* unit = new Unit(x, y);
	unit->SetType(type);


	switch (type)
	{
	case (ARBITER):
		{
		unit->SetMovementType(FLYING);
		unit->SetCollider({ 0, 0, 4 * 8, 3 * 8 });
		unit->SetSoftCollider({ 0, 0, 6 * 8, 5 * 8 });
		}
	}
	unit->SetPriority(currentPriority++);
	unit->Start();

	AddUnit(unit);
	return unit;
}

bool EntityManager::deleteUnit(C_List_item<Unit*>* item)
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


bool EntityManager::IsUnitSelected(C_List_item<Unit*>* unit)
{
	fPoint itemPos = unit->data->GetPosition();
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
		C_DynArray<PathNode> newPath;
		fPoint unitPos = selectedUnits[i]->GetPosition();
		iPoint unitTile = App->map->WorldToMap(round(unitPos.x), round(unitPos.y));
		iPoint dstTile = App->map->WorldToMap(x, y);
		if (App->pathFinding->GetNewPath(unitTile, dstTile, newPath))
		{
			
			newPath[newPath.Count() - 1].point = { x, y };
			newPath[newPath.Count() - 1].converted = true;
			
			selectedUnits[i]->SetNewPath(newPath);
		}
	}
}

SDL_Texture* EntityManager::GetTexture(Unit_Type type)
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

void EntityManager::AddUnit(Unit* unit)
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