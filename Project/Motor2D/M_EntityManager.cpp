#include "M_EntityManager.h"

#include "j1App.h"
#include "M_Textures.h"
#include "Unit.h"
#include "Building.h"
#include "Resource.h"
#include "M_Render.h"
#include "M_Input.h"
#include "M_PathFinding.h"
#include "S_SceneMap.h"
#include "M_CollisionController.h"
#include "M_FileSystem.h"
#include "M_Window.h"

const UnitStats* UnitsLibrary::GetStats(Unit_Type _type) const
{
	int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i] == _type)
			break;
	}
	return &stats[i];
}

const UnitSprite* UnitsLibrary::GetSprite(Unit_Type _type) const
{
	int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i] == _type)
			break;
	}
	return &sprites[i];
}

void UnitsLibrary::GetStateLimits(Unit_Type type, Unit_Movement_State state, int& min, int& max)
{
	const UnitSprite* data = GetSprite(type);
	switch (state)
	{
	case (MOVEMENT_IDLE) :
	{
		min = data->idle_line_start;
		max = data->idle_line_end;
		break;
	}
	case (MOVEMENT_WAIT) :
	{
		min = data->idle_line_start;
		max = data->idle_line_end;
		break;
	}
	case (MOVEMENT_GATHER) :
	{
		min = data->idle_line_start;
		max = data->idle_line_end;
		break;
	}
	case (MOVEMENT_MOVE) :
	{
		min = data->run_line_start;
		max = data->run_line_end;
		break;
	}
						 /*
						 case (ATTACK) :
						 {
						 min = data->idle_line_start;
						 min = data->idle_line_end;
						 break;
						 }
						 */
	}
}

const BuildingStats* BuildingsLibrary::GetStats(Building_Type type) const
{
	int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i] == type)
			break;
	}
	return &stats[i];
}

const BuildingSprite* BuildingsLibrary::GetSprite(Building_Type type) const
{
	int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i] == type)
			break;
	}
	return &sprites[i];
}

const ResourceStats* ResourcesLibrary::GetStats(Resource_Type type) const
{
	int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i] == type)
			break;
	}
	return &stats[i];
}

const ResourceSprite* ResourcesLibrary::GetSprite(Resource_Type type) const
{
	int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i] == type)
			break;
	}
	return &sprites[i];
}

M_EntityManager::M_EntityManager(bool start_enabled) : j1Module(start_enabled)
{

}

M_EntityManager::~M_EntityManager()
{

}

bool M_EntityManager::Start()
{
	LoadUnitsLibrary("entityManager/Unit stats data.xml", "entityManager/Unit sprite data.xml");
	LoadBuildingsLibrary("entityManager/Building stats data.xml", "entityManager/Building sprite data.xml");
	LoadResourcesLibrary("entityManager/Resource stats data.xml", "entityManager/Resource sprite data.xml");

	walkable_tile = App->tex->Load("graphics/walkable tile.png");
	nonwalkable_tile = App->tex->Load("graphics/building incorrect tile.png");

	unit_base = App->tex->Load("graphics/ui/o048.png");
	path_tex = App->tex->Load("textures/path.png");
	hpBar_empty = App->tex->Load("graphics/ui/hpbarempt.png");
	hpBar_filled = App->tex->Load("graphics/ui/hpbarfull.png");
	hpBar_mid = App->tex->Load("graphics/ui/hpbarmid.png");;
	hpBar_low = App->tex->Load("graphics/ui/hpbarlow.png");;
	building_base = App->tex->Load("graphics/ui/o110.png");

	buildingTile.texture = App->tex->Load("graphics/building correct tile.png");
	buildingTile.section = { 0, 0, 32, 32 };
	buildingTile.tint = { 255, 255, 255, 200 };
	buildingTile.useCamera = true;

	buildingTileN.texture = App->tex->Load("graphics/building incorrect tile.png");
	buildingTileN.section = { 0, 0, 32, 32 };
	buildingTileN.tint = { 255, 255, 255, 200 };
	buildingTileN.useCamera = true;

	//Create all orders
	addOrder(o_GenProbe_toss);
	addOrder(o_Set_rallyPoint);
	addOrder(o_Move);
	addOrder(o_Attack);
	addOrder(o_Gather);
	addOrder(o_Patrol);
	addOrder(o_Hold_pos);
	addOrder(o_Stop);
	return true;
}


bool M_EntityManager::Update(float dt)
{
	ManageInput();

	DoUnitLoop(dt);
	DoBuildingLoop(dt);
	DoResourceLoop(dt);

	UpdateSelectionRect();

	if (createBuilding)
	{
		UpdateCreationSprite();
	}
	if (selectEntities)
	{
		selectEntities = false;
		selectionRect.w = selectionRect.h = 0;
	}

	if (debug)
		DrawDebug();

	if (selectionRect.w != 0 || selectionRect.h != 0)
	{
		App->render->AddRect(selectionRect, false, 0, 255, 0, 255, false);
	}

	return true;
}

bool M_EntityManager::PostUpdate(float dt)
{
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

	if (!resourcesToDelete.empty())
	{
		std::list<Resource*>::iterator it = resourcesToDelete.begin();
		while (it != resourcesToDelete.end())
		{
			deleteResource(it);
			it++;
		}
		resourcesToDelete.clear();
	}
	return true;
}

bool M_EntityManager::CleanUp()
{
	std::list<Unit*>::iterator it = unitList.begin();
	while (it != unitList.end())
	{
		RELEASE(*it);
		it++;
	}
	unitList.clear();
	selectedUnits.clear();
	unitsToDelete.clear();

	std::list<Building*>::iterator it2 = buildingList.begin();
	while (it2 != buildingList.end())
	{
		RELEASE(*it2);
		it2++;
	}
	buildingList.clear();

	return true;
}

void M_EntityManager::DoUnitLoop(float dt)
{
	std::list<Unit*>::iterator it = unitList.begin();
	while (it != unitList.end())
	{
		if ((*it)->active)
		{
			if (selectEntities)
			{
				//Selecting units
				if (IsEntitySelected(*it))
				{
					if ((*it)->selected == false)
					{
						SelectUnit(*it);
					}
				}
				else if ((*it)->selected == true)
				{
					UnselectUnit(*it);
				}
			}
			//Unit update
			if (!(*it)->Update(dt))
			{
				unitsToDelete.push_back(*it);
			}
		}

		it++;
	}
}

void M_EntityManager::DoBuildingLoop(float dt)
{

	std::list<Building*>::iterator it = buildingList.begin();
	bool buildingSelected = false;
	while (it != buildingList.end())
	{
		if ((*it)->active)
		{
			if (selectEntities)
			{
				if (IsEntitySelected(*it) && !buildingSelected && selectedUnits.empty())
				{
					SelectBuilding(*it);
					buildingSelected = true;
				}
				else if ((*it)->selected)
				{
					UnselectBuilding(*it);
				}
			}
			(*it)->Update(dt);
		}
		it++;
	}
}
void M_EntityManager::DoResourceLoop(float dt)
{
	std::list<Resource*>::iterator it = resourceList.begin();
	bool resourceSelected = false;
	while (it != resourceList.end())
	{
		if ((*it)->active)
		{
			if (selectEntities)
			{
				if (IsEntitySelected(*it) && !selectedBuilding && selectedUnits.empty() && !resourceSelected)
				{
					SelectResource(*it);
					resourceSelected = true;
				}
				else if ((*it)->selected)
				{
					UnselectResource(*it);
				}
			}
			if (!(*it)->Update(dt))
			{
				resourcesToDelete.push_back(*it);
			}
		}
		it++;
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
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		if (createBuilding)
		{
			if (buildingWalkable)
			{
				CreateBuilding(logicTile.x, logicTile.y, buildingCreationType);
				createBuilding = false;
			}
		}
		else
			selectEntities = true;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (!createBuilding)
		{
			App->input->GetMousePosition(selectionRect.x, selectionRect.y);
		}
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if (!createBuilding)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			selectionRect.w = x - selectionRect.x;
			selectionRect.h = y - selectionRect.y;
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if (createBuilding)
			createBuilding = false;
		else if (!selectedUnits.empty())
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			iPoint pos = App->render->ScreenToWorld(x, y);
			iPoint tile = App->pathFinding->WorldToMap(pos.x, pos.y);

			std::list<Resource*>::iterator it_resource = resourceList.begin();
			bool resFound = false;
			while (it_resource != resourceList.end() && !resFound)
			{
				if (tile.x >= (*it_resource)->GetPosition().x && tile.x <= (*it_resource)->GetPosition().x + (*it_resource)->width_tiles * 4 &&
					tile.y >= (*it_resource)->GetPosition().y && tile.y <= (*it_resource)->GetPosition().y + (*it_resource)->height_tiles * 4 &&
					(*it_resource)->GetType() == MINERAL)
				{
					resFound = true;
				}
				else
					it_resource++;
			}
			bool buildingFound = false;
			std::list<Building*>::iterator it_building = buildingList.begin();
			if (!resFound)
			{

				while (it_building != buildingList.end() && !buildingFound)
				{
					if (tile.x >= (*it_building)->GetPosition().x && tile.x <= (*it_building)->GetPosition().x + (*it_building)->width_tiles * 4 &&
						tile.y >= (*it_building)->GetPosition().y && tile.y <= (*it_building)->GetPosition().y + (*it_building)->height_tiles * 4 &&
						(*it_building)->GetType() == ASSIMILATOR)
					{
						buildingFound = true;
					}
					else
						it_building++;
				}
			}
			if (resFound)
			{
				if ((*it_resource)->resourceAmount)
					SendToGather((*it_resource));
			}
			else if (buildingFound)
			{
				if ((*it_building)->gasResource->resourceAmount)
					SendToGather((*it_building));
			}
			else
			{
				SendNewPath(tile.x, tile.y);
			}
		}
	}

	//Enable / Disable render
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_UP)
	{
		render = !render;
	}

	//Enable / Disable debug
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
	{
		debug = !debug;
	}
	//Enable / Disable shadows
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP)
	{
		shadows = !shadows;
	}
}


Unit* M_EntityManager::CreateUnit(int x, int y, Unit_Type type)
{
	iPoint tile = App->pathFinding->WorldToMap(x, y);
	if (App->pathFinding->IsWalkable(tile.x, tile.y))
	{
		Unit* unit = new Unit(x, y);

		unit->active = true;
		unit->SetType(type);

		unit->SetMovementType(GROUND);
		unit->SetCollider({ 0, 0, 5 * 8, 5 * 8 });

		unit->SetPriority(currentPriority++);
		unit->Start();

		AddUnit(unit);
		return unit;
	}
	return NULL;

}

void M_EntityManager::StartBuildingCreation(Building_Type type)
{
	const BuildingSprite* data = GetBuildingSprite(type);
	buildingCreationSprite.texture = data->texture;
	buildingCreationSprite.section = { 0, 0, data->size_x, data->size_y };
	buildingCreationSprite.useCamera = true;
	buildingCreationSprite.layer = GUI_MAX_LAYERS;
	buildingCreationType = type;
	UpdateCreationSprite();

}

Building* M_EntityManager::CreateBuilding(int x, int y, Building_Type type)
{
	const BuildingStats* stats = GetBuildingStats(type);

	if (IsBuildingCreationWalkable(x, y, type))
	{
		Building* building = new Building(x, y, type);

		building->active = true;

		building->Start();

		AddBuilding(building);

		if (type == ASSIMILATOR)
		{
			std::list<Resource*>::iterator it = resourceList.begin();
			bool found = false;
			while (it != resourceList.end() && !found)
			{
				if ((*it)->GetPosition().x == x && (*it)->GetPosition().y == y)
				{
					building->gasResource = (*it);
					(*it)->active = false;
					found = true;

				}
			}
		}
		return building;
	}
	return NULL;
}

Resource* M_EntityManager::CreateResource(int x, int y, Resource_Type type)
{
	const ResourceStats* stats = GetResourceStats(type);

	if (IsResourceCreationWalkable(x, y, type))
	{
		Resource* resource = new Resource(x, y, type);
		resource->active = true;

		resource->Start();

		AddResource(resource);
		return resource;
	}
	return NULL;
}

void M_EntityManager::UpdateCreationSprite()
{
	const BuildingSprite* buildingSprite = GetBuildingSprite(buildingCreationType);
	logicTile.x = (App->sceneMap->currentTile_x / 4) * 4;
	logicTile.y = (App->sceneMap->currentTile_y / 4) * 4;
	iPoint p = App->pathFinding->MapToWorld(logicTile.x, logicTile.y);
	buildingCreationSprite.position.x = p.x - buildingSprite->offset_x;
	buildingCreationSprite.position.y = p.y - buildingSprite->offset_y;
	buildingTile.position.x = p.x;
	buildingTile.position.y = p.y;
	buildingTileN.position.x = p.x;
	buildingTileN.position.y = p.y;
	App->render->AddSprite(&buildingCreationSprite, SCENE);

	buildingWalkable = IsBuildingCreationWalkable(logicTile.x, logicTile.y, buildingCreationType);

	if (buildingWalkable)
	{
		App->render->AddSprite(&buildingTile, SCENE);
	}
	else
	{
		App->render->AddSprite(&buildingTileN, SCENE);
	}
	createBuilding = true;
}

bool M_EntityManager::IsBuildingCreationWalkable(int x, int y, Building_Type type) const
{
	bool ret = true;

	if (type != ASSIMILATOR)
	{
		const BuildingStats* buildingStats = GetBuildingStats(type);
		//First two loops to iterate graphic tiles
		for (int h = 0; h < buildingStats->height_tiles; h++)
		{
			for (int w = 0; w < buildingStats->width_tiles; w++)
			{
				//Now we iterate logic tiles
				for (int h2 = 0; h2 < 4; h2++)
				{
					for (int w2 = 0; w2 < 4; w2++)
					{
						if (!App->pathFinding->IsWalkable(x + w2 * w + w2, y + h2 * h + h2))
						{
							ret = false;
						}
					}
				}
			}
		}
	}
	else
	{
		ret = false;
		std::list<Resource*>::const_iterator it = resourceList.begin();
		while (it != resourceList.end() && !ret)
		{
			if ((*it)->GetPosition().x == x && (*it)->GetPosition().y == y)
			{
				ret = true;
			}
			it++;
		}
	}

	return ret;
}

bool M_EntityManager::IsResourceCreationWalkable(int x, int y, Resource_Type type) const
{
	bool ret = true;
	const ResourceStats* resourceStats = GetResourceStats(type);

	//First two loops to iterate graphic tiles
	for (int h = 0; h < resourceStats->height_tiles; h++)
	{
		for (int w = 0; w < resourceStats->width_tiles; w++)
		{
			//Now we iterate logic tiles
			for (int h2 = 0; h2 < 4; h2++)
			{
				for (int w2 = 0; w2 < 4; w2++)
				{
					if (!App->pathFinding->IsWalkable(x + w2 * w + w2, y + h2 * h + h2))
					{
						ret = false;
					}
				}
			}
		}
	}
	return ret;
}

bool M_EntityManager::deleteUnit(std::list<Unit*>::iterator it)
{
	(*it)->Destroy();
	if ((*it)->selected)
	{
		selectedUnits.remove(*it);
	}
	unitList.remove(*it);
	RELEASE(*it);


	return true;
}

bool M_EntityManager::deleteBuilding(std::list<Building*>::iterator it)
{
	(*it)->Destroy();
	if ((*it)->selected)
	{
		UnselectBuilding(*it);
	}
	buildingList.remove(*it);
	RELEASE(*it);

	return true;
}

bool M_EntityManager::deleteResource(std::list<Resource*>::iterator it)
{
	(*it)->Destroy();
	if ((*it)->selected)
	{
		UnselectResource(*it);
	}
	resourceList.remove(*it);
	RELEASE(*it);

	return true;
}
bool M_EntityManager::IsEntitySelected(Entity* entity) const
{
	SDL_Rect itemRect = entity->GetCollider();
	SDL_Rect rect = selectionRect;

	iPoint rectPos = App->render->ScreenToWorld(rect.x, rect.y);
	rect.x = rectPos.x;
	rect.y = rectPos.y;

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
		iPoint Rcenter = App->pathFinding->MapToWorld(x, y);
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
			iPoint dstTile = App->pathFinding->WorldToMap(destinationRect.x + posFromRect.x, destinationRect.y + posFromRect.y);

			//Unit tile position
			fPoint unitPos = (*it)->GetPosition();
			iPoint unitTile = App->pathFinding->WorldToMap(round(unitPos.x), round(unitPos.y));

			//If destination is not walkable, use the player's clicked tile
			if (!App->pathFinding->IsWalkable(dstTile.x, dstTile.y))
				dstTile = { x, y };

			//If a path is found, send it to the unit
			(*it)->Move(dstTile);

			it++;
		}
	}
}
void M_EntityManager::SendToGather(Resource* resource)
{
	std::list<Unit*>::iterator it = selectedUnits.begin();

	while (it != selectedUnits.end())
	{
		if ((*it)->GetType() == PROBE)
		{
			(*it)->SetGathering(resource);
		}
		it++;
	}
}

void M_EntityManager::SendToGather(Building* building)
{
	std::list<Unit*>::iterator it = selectedUnits.begin();

	while (it != selectedUnits.end())
	{
		if ((*it)->GetType() == PROBE)
		{
			(*it)->SetGathering(building);
		}
		it++;
	}
}

Building* M_EntityManager::FindClosestNexus(Unit* unit)
{
	Building* ret = NULL;
	std::list<Building*>::iterator it = buildingList.begin();
	int dst = App->pathFinding->width * App->pathFinding->tile_width + App->pathFinding->height * App->pathFinding->height;
	while (it != buildingList.end())
	{
		if ((*it)->GetType() == NEXUS)
		{
			iPoint worldPos = App->pathFinding->MapToWorld((*it)->GetPosition().x, (*it)->GetPosition().y);
			int newDst = abs(worldPos.x - unit->GetPosition().x) + abs(worldPos.y - unit->GetPosition().y);
			if (newDst < dst)
			{
				dst = newDst;
				ret = *it;
			}
		}
		it++;
	}
	return ret;
}

Resource* M_EntityManager::FindClosestResource(Unit* unit)
{
	Resource* ret = NULL;
	std::list<Resource*>::iterator it = resourceList.begin();
	int dst = App->pathFinding->width * App->pathFinding->tile_width + App->pathFinding->height * App->pathFinding->height;
	while (it != resourceList.end())
	{
		if ((*it)->GetType() == MINERAL && !(*it)->ocupied && (*it)->resourceAmount)
		{
			iPoint worldPos = App->pathFinding->MapToWorld((*it)->GetPosition().x, (*it)->GetPosition().y);
			int newDst = abs(worldPos.x - unit->GetPosition().x) + abs(worldPos.y - unit->GetPosition().y);
			if (newDst < dst)
			{
				dst = newDst;
				ret = *it;
			}
		}
		it++;
	}
	if (dst > 500)
		ret = NULL;

	return ret;
}


iPoint M_EntityManager::GetClosestCorner(Unit* unit, Building* building)
{
	iPoint unitPos = App->pathFinding->WorldToMap(unit->GetPosition().x, unit->GetPosition().y);
	fPoint buildingPos = building->GetPosition();
	fPoint buildingCenter = buildingPos;
	buildingCenter.x += (building->width_tiles / 2) * 4;
	buildingCenter.y += (building->width_tiles / 2) * 4;
	bool maxX = false, maxY = false;

	if (unitPos.x > buildingCenter.x)
		maxX = 1;
	if (unitPos.y > buildingCenter.y)
		maxY = 1;

	iPoint ret = { (int)buildingPos.x + building->width_tiles * 4 * maxX, (int)buildingPos.y + building->height_tiles * 4 * maxY };
	maxX ? ret.x += 1 : ret.x -= 1;
	maxY ? ret.y += 1 : ret.y -= 1;
	if (!App->pathFinding->IsWalkable(ret.x, ret.y))
	{
		maxX = !maxX;
		ret = { (int)buildingPos.x + building->width_tiles * 4 * maxX, (int)buildingPos.y + building->height_tiles * 4 * maxY };
		maxX ? ret.x += 1 : ret.x -= 1;
		maxY ? ret.y += 1 : ret.y -= 1;

		if (!App->pathFinding->IsWalkable(ret.x, ret.y))
		{
			maxY = !maxY;
			ret = { (int)buildingPos.x + building->width_tiles * 4 * maxX, (int)buildingPos.y + building->height_tiles * 4 * maxY };
			maxX ? ret.x += 1 : ret.x -= 1;
			maxY ? ret.y += 1 : ret.y -= 1;

			if (!App->pathFinding->IsWalkable(ret.x, ret.y))
			{
				maxX = !maxX;
				ret = { (int)buildingPos.x + building->width_tiles * 4 * maxX, (int)buildingPos.y + building->height_tiles * 4 * maxY };
				maxX ? ret.x += 1 : ret.x -= 1;
				maxY ? ret.y += 1 : ret.y -= 1;
			}
		}
	}
	return ret;
}

iPoint M_EntityManager::GetClosestCorner(Unit* unit, Resource* resource)
{
	iPoint unitPos = App->pathFinding->WorldToMap(unit->GetPosition().x, unit->GetPosition().y);
	fPoint resourcePos = resource->GetPosition();
	fPoint resourceCenter = resourcePos;
	resourceCenter.x += (resource->width_tiles / 2) * 4;
	resourceCenter.y += (resource->width_tiles / 2) * 4;

	int maxX = 0, maxY = 0;

	if (unitPos.x > resourceCenter.x)
		maxX = 1;
	if (unitPos.y > resourceCenter.y)
		maxY = 1;


	iPoint ret = { (int)resourcePos.x + resource->width_tiles * 4 * maxX, (int)resourcePos.y + resource->height_tiles * 4 * maxY };
	if (!App->pathFinding->IsWalkable(ret.x, ret.y))
	{
		maxX = !maxX;
		ret = { (int)resourcePos.x + resource->width_tiles * 4 * maxX, (int)resourcePos.y + resource->height_tiles * 4 * maxY };
		maxX ? ret.x += 1 : ret.x -= 1;
		maxY ? ret.y += 1 : ret.y -= 1;

		if (!App->pathFinding->IsWalkable(ret.x, ret.y))
		{
			maxY = !maxY;
			ret = { (int)resourcePos.x + resource->width_tiles * 4 * maxX, (int)resourcePos.y + resource->height_tiles * 4 * maxY };
			maxX ? ret.x += 1 : ret.x -= 1;
			maxY ? ret.y += 1 : ret.y -= 1;

			if (!App->pathFinding->IsWalkable(ret.x, ret.y))
			{
				maxX = !maxX;
				ret = { (int)resourcePos.x + resource->width_tiles * 4 * maxX, (int)resourcePos.y + resource->height_tiles * 4 * maxY };
				maxX ? ret.x += 1 : ret.x -= 1;
				maxY ? ret.y += 1 : ret.y -= 1;
			}
		}
	}
	return ret;
}

const UnitStats* M_EntityManager::GetUnitStats(Unit_Type type) const
{
	return unitsLibrary.GetStats(type);
}

const UnitSprite* M_EntityManager::GetUnitSprite(Unit_Type type) const
{
	return unitsLibrary.GetSprite(type);
}

const BuildingStats* M_EntityManager::GetBuildingStats(Building_Type type) const
{
	return buildingsLibrary.GetStats(type);
}
const BuildingSprite* M_EntityManager::GetBuildingSprite(Building_Type type) const
{
	return buildingsLibrary.GetSprite(type);
}

const ResourceStats* M_EntityManager::GetResourceStats(Resource_Type type) const
{
	return resourcesLibrary.GetStats(type);
}
const ResourceSprite* M_EntityManager::GetResourceSprite(Resource_Type type) const
{
	return resourcesLibrary.GetSprite(type);
}

void M_EntityManager::UpdateSpriteRect(Unit* unit, SDL_Rect& rect, SDL_RendererFlip& flip, float dt)
{
	//Rectangle definition variables
	int direction, size, rectX, rectY;
	const UnitSprite* unitData = unitsLibrary.GetSprite(unit->GetType());

	//Getting unit movement direction
	float angle = unit->GetVelocity().GetAngle() - 90;
	if (angle < 0)
		angle = 360 + angle;
	angle = 360 - angle;
	direction = angle / (360 / 32);

	if (direction > 16)
	{
		flip = SDL_FLIP_HORIZONTAL;
		direction -= 16;
		rectX = 16 * unitData->size - direction * unitData->size;
	}
	else
	{
		flip = SDL_FLIP_NONE;
		rectX = direction * unitData->size;
	}

	int min, max;
	unitsLibrary.GetStateLimits(unit->GetType(), unit->GetState(), min, max);

	unit->currentFrame += unitData->animationSpeed * dt;
	if (unit->currentFrame >= max + 1)
		unit->currentFrame = min;

	rectY = (int)unit->currentFrame * unitData->size;
	rect = { rectX, rectY, unitData->size, unitData->size };
}

//Call for this function every time the unit state changes (starts moving, starts idle, etc)
void M_EntityManager::UpdateCurrentFrame(Unit* unit)
{
	const UnitSprite* data = unitsLibrary.GetSprite(unit->GetType());
	switch (unit->GetState())
	{
	case(MOVEMENT_IDLE) :
	{
		unit->currentFrame = data->idle_line_start;
		break;
	}
	case(MOVEMENT_GATHER) :
	{
		unit->currentFrame = data->idle_line_start;
		break;
	}
	case(MOVEMENT_MOVE) :
	{
		unit->currentFrame = data->run_line_start;
		break;
	}
	}
}

void M_EntityManager::StopSelectedUnits()
{
	if (!selectedUnits.empty())
	{
		std::list<Unit*>::iterator it = selectedUnits.begin();

		while (it != selectedUnits.end())
		{
			(*it)->Stop();
			it++;
		}
	}
}

bool M_EntityManager::LoadUnitsLibrary(char* stats, char* sprites)
{
	bool ret = true;
	if (!LoadUnitsStats(stats))
	{
		ret = false;
	}
	if (!LoadUnitsSprites(sprites))
	{
		ret = false;
	}
	return ret;
}

bool M_EntityManager::LoadBuildingsLibrary(char* stats, char* sprites)
{
	bool ret = true;
	if (!LoadBuildingsStats(stats))
	{
		ret = false;
	}
	if (!LoadBuildingsSprites(sprites))
	{
		ret = false;
	}
	return ret;
}

bool M_EntityManager::LoadResourcesLibrary(char* stats, char* sprites)
{
	bool ret = true;
	if (!LoadResourcesStats(stats))
	{
		ret = false;
	}
	if (!LoadResourcesSprites(sprites))
	{
		ret = false;
	}
	return ret;
}

bool M_EntityManager::LoadUnitsStats(char* path)
{
	bool ret = true;
	char* buf;
	int size = App->fs->Load(path, &buf);
	pugi::xml_document file;
	pugi::xml_parse_result result = file.load_buffer(buf, size);

	RELEASE_ARRAY(buf);

	if (result == NULL)
	{
		LOG("Could not load sprite data file %s. pugi error: %s", "entityManager / Sprite data.tmx", result.description());
		return false;
	}

	pugi::xml_node node;
	for (node = file.child("stats").child("unit"); node && ret; node = node.next_sibling("unit"))
	{
		C_String tmp = node.child("name").attribute("value").as_string();
		if (tmp == "carrier")
			unitsLibrary.types.push_back(CARRIER);
		else if (tmp == "observer")
			unitsLibrary.types.push_back(OBSERVER);
		else if (tmp == "probe")
			unitsLibrary.types.push_back(PROBE);
		else if (tmp == "sapper")
			unitsLibrary.types.push_back(SAPPER);
		else if (tmp == "shuttle")
			unitsLibrary.types.push_back(SHUTTLE);
		else if (tmp == "arbiter")
			unitsLibrary.types.push_back(ARBITER);
		else if (tmp == "intercep")
			unitsLibrary.types.push_back(INTERCEP);
		else if (tmp == "scout")
			unitsLibrary.types.push_back(SCOUT);
		else if (tmp == "reaver")
			unitsLibrary.types.push_back(REAVER);
		else if (tmp == "zealot")
			unitsLibrary.types.push_back(ZEALOT);
		else if (tmp == "archon_t")
			unitsLibrary.types.push_back(ARCHON_T);
		else if (tmp == "high_templar")
			unitsLibrary.types.push_back(HIGH_TEMPLAR);
		else if (tmp == "dark_templar")
			unitsLibrary.types.push_back(DARK_TEMPLAR);
		else if (tmp == "dragoon")
			unitsLibrary.types.push_back(DRAGOON);
	}
	return true;
}

bool M_EntityManager::LoadBuildingsStats(char* path)
{
	bool ret = true;
	char* buf;
	int size = App->fs->Load(path, &buf);
	pugi::xml_document file;
	pugi::xml_parse_result result = file.load_buffer(buf, size);

	RELEASE_ARRAY(buf);

	if (result == NULL)
	{
		LOG("Could not load sprite data file %s. pugi error: %s", "entityManager / Sprite data.tmx", result.description());
		return false;
	}

	pugi::xml_node node;
	for (node = file.child("stats").child("building"); node && ret; node = node.next_sibling("building"))
	{
		C_String tmp = node.child("name").attribute("value").as_string();
		if (tmp == "nexus")
			buildingsLibrary.types.push_back(NEXUS);
		else if (tmp == "pylon")
			buildingsLibrary.types.push_back(PYLON);
		else if (tmp == "assimilator")
			buildingsLibrary.types.push_back(ASSIMILATOR);

		BuildingStats stats;
		stats.HP = node.child("HP").attribute("value").as_int();
		stats.shield = node.child("shield").attribute("value").as_int();
		stats.armor = node.child("armor").attribute("value").as_int();
		stats.cost = node.child("cost").attribute("value").as_int();
		//stats.costType = node.child("cost_type").attribute("value").as_int();
		stats.width_tiles = node.child("width_tiles").attribute("value").as_int();
		stats.height_tiles = node.child("height_tiles").attribute("value").as_int();
		stats.buildTime = node.child("build_time").attribute("value").as_int();
		stats.psi = node.child("psi").attribute("value").as_int();

		buildingsLibrary.stats.push_back(stats);
	}

	return ret;
}

bool M_EntityManager::LoadResourcesStats(char* path)
{
	bool ret = true;
	char* buf;
	int size = App->fs->Load(path, &buf);
	pugi::xml_document file;
	pugi::xml_parse_result result = file.load_buffer(buf, size);

	RELEASE_ARRAY(buf);

	if (result == NULL)
	{
		LOG("Could not load sprite data file %s. pugi error: %s", "entityManager / Sprite data.tmx", result.description());
		return false;
	}

	pugi::xml_node node;
	for (node = file.child("stats").child("resource"); node && ret; node = node.next_sibling("resource"))
	{
		C_String tmp = node.child("name").attribute("value").as_string();
		if (tmp == "mineral")
			resourcesLibrary.types.push_back(MINERAL);
		else if (tmp == "gas")
			resourcesLibrary.types.push_back(GAS);

		ResourceStats stats;
		stats.maxAmount = node.child("max_amount").attribute("value").as_int();
		stats.width_tiles = node.child("width_tiles").attribute("value").as_int();
		stats.height_tiles = node.child("height_tiles").attribute("value").as_int();

		resourcesLibrary.stats.push_back(stats);
	}

	return ret;
}
bool M_EntityManager::LoadUnitsSprites(char* path)
{
	bool ret = true;
	char* buf;
	int size = App->fs->Load(path, &buf);
	pugi::xml_document file;
	pugi::xml_parse_result result = file.load_buffer(buf, size);

	if (result == NULL)
	{
		LOG("Could not load sprite data file %s. pugi error: %s", "entityManager / Sprite data.tmx", result.description());
		ret = false;
	}
	else
	{
		RELEASE_ARRAY(buf);
	}

	pugi::xml_node node;
	for (node = file.child("sprites").child("unit"); node && ret; node = node.next_sibling("unit"))
	{
		UnitSprite sprite;
		sprite.texture = App->tex->Load(node.child("file").attribute("name").as_string());
		sprite.size = node.child("size").attribute("value").as_int();
		sprite.animationSpeed = node.child("animationSpeed").attribute("value").as_float();
		sprite.idle_line_start = node.child("idle_line_start").attribute("value").as_int();
		sprite.idle_line_end = node.child("idle_line_end").attribute("value").as_int();
		sprite.run_line_start = node.child("run_line_start").attribute("value").as_int();
		sprite.run_line_end = node.child("run_line_end").attribute("value").as_int();
		sprite.attack_line_start = node.child("attack_line_start").attribute("value").as_int();
		sprite.attack_line_end = node.child("attack_line_end").attribute("value").as_int();

		unitsLibrary.sprites.push_back(sprite);
	}

	return ret;
}

bool M_EntityManager::LoadBuildingsSprites(char* path)
{
	bool ret = true;
	char* buf;
	int size = App->fs->Load(path, &buf);
	pugi::xml_document file;
	pugi::xml_parse_result result = file.load_buffer(buf, size);

	if (result == NULL)
	{
		LOG("Could not load sprite data file %s. pugi error: %s", "entityManager / Sprite data.tmx", result.description());
		ret = false;
	}
	else
	{
		RELEASE_ARRAY(buf);
	}

	pugi::xml_node node;
	for (node = file.child("sprites").child("building"); node && ret; node = node.next_sibling("building"))
	{
		BuildingSprite sprite;
		sprite.texture = App->tex->Load(node.child("file").attribute("name").as_string());
		sprite.size_x = node.child("size_x").attribute("value").as_int();
		sprite.size_y = node.child("size_y").attribute("value").as_int();
		sprite.offset_x = node.child("offset_x").attribute("value").as_int();
		sprite.offset_y = node.child("offset_y").attribute("value").as_int();

		sprite.shadow = App->tex->Load(node.child("shadow").child("file").attribute("name").as_string());
		sprite.shadow_size_x = node.child("shadow").child("size_x").attribute("value").as_int();
		sprite.shadow_size_y = node.child("shadow").child("size_y").attribute("value").as_int();
		sprite.shadow_offset_x = node.child("shadow").child("offset_x").attribute("value").as_int();
		sprite.shadow_offset_y = node.child("shadow").child("offset_y").attribute("value").as_int();
		buildingsLibrary.sprites.push_back(sprite);
	}

	return ret;
}

bool M_EntityManager::LoadResourcesSprites(char* path)
{
	bool ret = true;
	char* buf;
	int size = App->fs->Load(path, &buf);
	pugi::xml_document file;
	pugi::xml_parse_result result = file.load_buffer(buf, size);

	if (result == NULL)
	{
		LOG("Could not load sprite data file %s. pugi error: %s", "entityManager / Sprite data.tmx", result.description());
		ret = false;
	}
	else
	{
		RELEASE_ARRAY(buf);
	}

	pugi::xml_node node;
	for (node = file.child("sprites").child("resource"); node && ret; node = node.next_sibling("resource"))
	{
		ResourceSprite sprite;
		sprite.texture = App->tex->Load(node.child("file").attribute("name").as_string());
		sprite.size_x = node.child("size_x").attribute("value").as_int();
		sprite.size_y = node.child("size_y").attribute("value").as_int();
		sprite.offset_x = node.child("offset_x").attribute("value").as_int();
		sprite.offset_y = node.child("offset_y").attribute("value").as_int();

		sprite.shadow = App->tex->Load(node.child("shadow").child("file").attribute("name").as_string());
		sprite.shadow_size_x = node.child("shadow").child("size_x").attribute("value").as_int();
		sprite.shadow_size_y = node.child("shadow").child("size_y").attribute("value").as_int();
		sprite.shadow_offset_x = node.child("shadow").child("offset_x").attribute("value").as_int();
		sprite.shadow_offset_y = node.child("shadow").child("offset_y").attribute("value").as_int();


		resourcesLibrary.sprites.push_back(sprite);
	}

	return ret;
}
void M_EntityManager::AddUnit(Unit* unit)
{
	unitList.push_back(unit);
}

void M_EntityManager::AddBuilding(Building* building)
{
	buildingList.push_back(building);
}

void M_EntityManager::AddResource(Resource* resource)
{
	resourceList.push_back(resource);
}

#pragma region Selection Methods
void M_EntityManager::SelectUnit(Unit* unit)
{
	unit->selected = true;
	unit->UpdateBarState();
	selectedUnits.push_back(unit);
}

void M_EntityManager::UnselectUnit(Unit* unit)
{
	unit->selected = false;
	unit->UpdateBarState();
	selectedUnits.remove(unit);
}

void M_EntityManager::SelectBuilding(Building* building)
{
	building->selected = true;
	building->UpdateBarState();
	selectedBuilding = building;
}

void M_EntityManager::UnselectBuilding(Building* building)
{
	building->selected = false;
	building->UpdateBarState();
	selectedBuilding = NULL;
}

void M_EntityManager::SelectResource(Resource* resource)
{
	resource->selected = true;
}

void M_EntityManager::UnselectResource(Resource* resource)
{
	resource->selected = false;
}

#pragma endregion

void M_EntityManager::DrawDebug()
{
	if (!selectedUnits.empty())
		App->render->AddRect(groupRect, true, 255, 0, 0, 255, false);

	App->render->AddRect(destinationRect, true, 255, 255, 0, 255, false);
}

//Orders

void  M_EntityManager::addOrder(Order& nOrder, UI_Button2* nButt)
{
	if (nButt != NULL)
		nOrder.SetButton(*nButt);
	orders.push_back(&nOrder);
}
