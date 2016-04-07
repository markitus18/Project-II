#include "M_EntityManager.h"

#include "j1App.h"
#include "M_Textures.h"
#include "Unit.h"
#include "Building.h"
#include "Resource.h"
#include "Controlled.h"
#include "M_Render.h"
#include "M_Input.h"
#include "M_PathFinding.h"
#include "S_SceneMap.h"
#include "M_CollisionController.h"
#include "M_FileSystem.h"
#include "M_Window.h"
#include "M_GUI.h"
#include "Intersections.h"

const UnitStatsData* UnitsLibrary::GetStats(Unit_Type _type) const
{
	int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i] == _type)
			break;
	}
	return &stats[i];
}

const UnitSpriteData* UnitsLibrary::GetSprite(Unit_Type _type) const
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
	const UnitSpriteData* data = GetSprite(type);
	switch (state)
	{
	case (MOVEMENT_IDLE) :
	{
		min = data->idle_line_start;
		max = data->idle_line_end;
		break;
	}
	case (MOVEMENT_ATTACK) :
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

const BuildingStatsData* BuildingsLibrary::GetStats(Building_Type type) const
{
	int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i] == type)
			break;
	}
	return &stats[i];
}

const BuildingSpriteData* BuildingsLibrary::GetSprite(Building_Type type) const
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
	LoadHPBars();

	walkable_tile = App->tex->Load("graphics/walkable tile.png");
	nonwalkable_tile = App->tex->Load("graphics/building incorrect tile.png");

	unit_base = App->tex->Load("graphics/ui/o048.png");
	path_tex = App->tex->Load("textures/path.png");
	hpBar_empty = App->tex->Load("graphics/ui/hpbar2empt.png");
	hpBar_filled = App->tex->Load("graphics/ui/hpbar2full.png");
	hpBar_mid = App->tex->Load("graphics/ui/hpbar2mid.png");;
	hpBar_low = App->tex->Load("graphics/ui/hpbar2low.png");;
	building_base = App->tex->Load("graphics/ui/o110.png");

	buildingTile.texture = App->tex->Load("graphics/building correct tile.png");
	buildingTile.section = { 0, 0, 32, 32 };
	buildingTile.tint = { 255, 255, 255, 200 };
	buildingTile.useCamera = true;

	buildingTileN.texture = App->tex->Load("graphics/building incorrect tile.png");
	buildingTileN.section = { 0, 0, 32, 32 };
	buildingTileN.tint = { 255, 255, 255, 200 };
	buildingTileN.useCamera = true;

	//Mouse Load --------------------------------------------------
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/arrow.png"));
	mouseTexturesNumber.push_back(5);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/drag.png"));
	mouseTexturesNumber.push_back(4);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/magg.png"));
	mouseTexturesNumber.push_back(13);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/magy.png"));
	mouseTexturesNumber.push_back(13);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/magr.png"));
	mouseTexturesNumber.push_back(13);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/scrollu.png"));
	mouseTexturesNumber.push_back(2);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/scrollur.png"));
	mouseTexturesNumber.push_back(2);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/scrollr.png"));
	mouseTexturesNumber.push_back(2);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/scrolldr.png"));
	mouseTexturesNumber.push_back(2);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/scrolld.png"));
	mouseTexturesNumber.push_back(2);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/scrolldl.png"));
	mouseTexturesNumber.push_back(2);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/scrolll.png"));
	mouseTexturesNumber.push_back(2);
	mouseTextures.push_back(App->tex->Load("graphics/ui/cursors/scrollul.png"));
	mouseTexturesNumber.push_back(2);

	mouseSprite.texture = mouseTextures[0];
	mouseSprite.section = { 0, 0, 128, 128 };
	mouseSprite.layer = GUI_MAX_LAYERS;
	mouseSprite.useCamera = true;

	App->input->DisableCursorImage();
	// -----------------------------------------------------------
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
		SetMouseState(M_DEFAULT, false);
		selectEntities = false;
		startSelection = false;
		selectionRect.w = selectionRect.h = 0;
	}

	if (debug)
		DrawDebug();

	if (selectionRect.w != 0 || selectionRect.h != 0)
	{
		App->render->AddRect(selectionRect, false, 0, 255, 0, 255, false);
	}
	if (!startSelection)
	{
		if (hoveringBuilding)
		{
			SetMouseState(M_ALLY_HOVER, false);
			App->render->AddRect(hoveringBuilding->GetCollider(), true, 255, 255, 0, 100);
		}
		else if (hoveringResource)
		{
			SetMouseState(M_RESOURCE_HOVER, false);
			App->render->AddRect(hoveringResource->GetCollider(), true, 255, 255, 0, 100);
		}
		else if (hoveringUnit)
		{
			if (hoveringUnit->stats.player == PLAYER)
				SetMouseState(M_ALLY_HOVER, false);
			else
				SetMouseState(M_ENEMY_HOVER, false);
			App->render->AddRect(hoveringUnit->GetCollider(), true, 255, 255, 0, 100);
		}
		else if (mouseState == M_ALLY_HOVER || mouseState == M_ENEMY_HOVER || mouseState == M_RESOURCE_HOVER)
			SetMouseState(M_DEFAULT, false);
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

	UpdateMouseSprite(dt);
	App->render->AddSprite(&mouseSprite, GUI);


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

	std::list<Resource*>::iterator it3 = resourceList.begin();
	while (it3 != resourceList.end())
	{
		RELEASE(*it3);
		it3++;
	}
	resourceList.clear();

	return true;
}

void M_EntityManager::UpdateMouseSprite(float dt)
{
	int x = 0, y = 0;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	mouseSprite.position.x = mousePos.x - 64;
	mouseSprite.position.y = mousePos.y - 64;
	mouseSprite.texture = mouseTextures[static_cast<int>(mouseState)];
	UpdateMouseAnimation(dt);
}

void M_EntityManager::SetMouseState(Mouse_State state, bool externalModule)
{
	if (((externalModule && mouseState != M_SELECTION && mouseState != M_ALLY_HOVER && mouseState != M_ENEMY_HOVER && mouseState != M_RESOURCE_HOVER) || !externalModule) && state != mouseState)
	{
		mouseState = state; 
		mouseMaxRect = mouseTexturesNumber[static_cast<int>(state)] - 1;
		mouseRect = 0;
	}
}

void M_EntityManager::UpdateMouseAnimation(float dt)
{
	mouseRect += mouseAnimationSpeed * dt;
	if (mouseRect > mouseMaxRect + 1)
		mouseRect = mouseMinRect;
	mouseSprite.section.y = (int)mouseRect * 128;
}

void M_EntityManager::DoUnitLoop(float dt)
{
	//Selection controllers
	Unit_Type selectedType;
	bool unitSelected = false;
	bool multipleUnitsSelected = false;
	bool differentTypesSelected = false;
	

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
					if (unitSelected)
					{
						if (selectedType != (*it)->GetType())
							differentTypesSelected = true;
						multipleUnitsSelected = true;
					}
					selectedType = (*it)->GetType();
					unitSelected = true;

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
	if (unitSelected)
	{
		if (differentTypesSelected)
			App->gui->SetCurrentGrid(G_DEFAULT);
		else
		{
			App->gui->SetCurrentGrid(selectedType, multipleUnitsSelected);
		}
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
					App->gui->SetCurrentGrid((*it)->GetType());
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
					App->gui->SetCurrentGrid(G_DEFAULT);
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
	if (!App->gui->mouseClicked)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
		{
			if (createBuilding)
			{
				if (buildingWalkable)
				{
					CreateBuilding(logicTile.x, logicTile.y, buildingCreationType, PLAYER);
					createBuilding = false;
				}
			}
			else if (startSelection)
			{
				selectEntities = true;
			}
			else if (!executedOrder)
			{
				DoSingleSelection();
			}
			else
				executedOrder = false;
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			if (moveUnits)
			{
				MoveSelectedUnits();
				executedOrder = true;
			}
			else if (attackUnits)
			{
				int x, y;
				App->input->GetMousePosition(x, y);
				iPoint pos = App->render->ScreenToWorld(x, y);
				SendToAttack(pos.x, pos.y);
				executedOrder = true;
			}
			else if (setWaypoint)
			{
				int x, y;
				App->input->GetMousePosition(x, y);
				iPoint worldPos = App->render->ScreenToWorld(x, y);
				iPoint tile = App->pathFinding->WorldToMap(worldPos.x, worldPos.y);
				selectedBuilding->waypointTile = tile;
				selectedBuilding->hasWaypoint = true;
				setWaypoint = false;
				executedOrder = true;
			}
			else if (!createBuilding)
			{
				App->input->GetMousePosition(selectionRect.x, selectionRect.y);
			}
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
		{
			if (!executedOrder && !createBuilding)
			{
				int x, y;
				App->input->GetMousePosition(x, y);
				if (selectionRect.x != x && selectionRect.y != y)
				{
					if (!startSelection)
					{
						startSelection = true;
						SetMouseState(M_SELECTION, false);
					}
					selectionRect.w = x - selectionRect.x;
					selectionRect.h = y - selectionRect.y;
				}
			}
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			if (createBuilding)
				createBuilding = false;
			else if (moveUnits)
				moveUnits = false;
			else if (!selectedUnits.empty())
			{
				MoveSelectedUnits();
			}
		}
	}

	//Enable / Disable render
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		render = !render;
	}

	//Enable / Disable debug
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	{
		debug = !debug;
	}
	//Enable / Disable shadows
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		shadows = !shadows;
	}
}
void M_EntityManager::StartUnitCreation(Unit_Type type)
{
		const UnitStatsData* stats = GetUnitStats(type);
	if (selectedBuilding && App->sceneMap->player.psi + stats->psi <= App->sceneMap->player.maxPsi)
	{
		fPoint buildingTile = selectedBuilding->GetPosition();
		iPoint buildingPos = App->pathFinding->MapToWorld(buildingTile.x, buildingTile.y);

		App->sceneMap->player.psi += stats->psi;
		CreateUnit(buildingPos.x - selectedBuilding->width_tiles / 2 * 2 - 1, buildingPos.y - selectedBuilding->height_tiles / 2 * 2 - 1, type, PLAYER, selectedBuilding);
	}
}

Unit* M_EntityManager::CreateUnit(int x, int y, Unit_Type type, Player_Type playerType, Building* building)
{
	const UnitStatsData* stats = GetUnitStats(type);
	iPoint tile = App->pathFinding->WorldToMap(x, y);
	if (App->pathFinding->IsWalkable(tile.x, tile.y))
	{
		Unit* unit = new Unit(x, y, type, playerType);

		unit->active = true;

		int size = (2 * unit->GetSizeType() + 1);
		unit->SetCollider({ 0, 0, size * 8, size * 8 });
			
		unit->SetPriority(currentPriority++);
		unit->Start();

		AddUnit(unit);
		if (building)
		{
			if (building->hasWaypoint)
				unit->Move(building->waypointTile, ATTACK_STAND);
		}
		return unit;
	}
	return NULL;
}


void M_EntityManager::StartBuildingCreation(Building_Type type)
{
	const BuildingStatsData* stats = GetBuildingStats(type);
	if (App->sceneMap->player.mineral >= stats->mineralCost && App->sceneMap->player.gas >= stats->gasCost)
	{
		const BuildingSpriteData* data = GetBuildingSprite(type);
		buildingCreationSprite.texture = data->texture;
		buildingCreationSprite.section = { 0, 0, data->size_x, data->size_y };
		buildingCreationSprite.useCamera = true;
		buildingCreationSprite.layer = GUI_MAX_LAYERS;
		buildingCreationSprite.y_ref = App->pathFinding->width * App->pathFinding->tile_width;
		buildingCreationType = type;
		UpdateCreationSprite();
	}
}

Building* M_EntityManager::CreateBuilding(int x, int y, Building_Type type, Player_Type player)
{
	const BuildingStatsData* stats = GetBuildingStats(type);

	if (IsBuildingCreationWalkable(x, y, type))
	{
		Building* building = new Building(x, y, type);

		building->active = true;

		App->sceneMap->player.maxPsi += stats->psi;
		App->sceneMap->player.mineral -= stats->mineralCost;
		App->sceneMap->player.gas -= stats->gasCost;

		building->stats.player = player;
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
				it++;
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
	const BuildingSpriteData* buildingSprite = GetBuildingSprite(buildingCreationType);
	logicTile.x = (App->sceneMap->currentTile_x / 2) * 2;
	logicTile.y = (App->sceneMap->currentTile_y / 2) * 2;
	iPoint p = App->pathFinding->MapToWorld(logicTile.x, logicTile.y);
	buildingCreationSprite.position.x = p.x - buildingSprite->offset_x;
	buildingCreationSprite.position.y = p.y - buildingSprite->offset_y;
	buildingTile.position.x = p.x;
	buildingTile.position.y = p.y;
	buildingTile.y_ref = buildingCreationSprite.y_ref + 1;
	buildingTileN.position.x = p.x;
	buildingTileN.y_ref = buildingCreationSprite.y_ref + 1;;
	buildingTileN.position.y = p.y;
	App->render->AddSprite(&buildingCreationSprite, SCENE);

	buildingWalkable = IsBuildingCreationWalkable(logicTile.x, logicTile.y, buildingCreationType);
	/*
	if (buildingWalkable)
	{
		App->render->AddSprite(&buildingTile, SCENE);
	}
	else
	{
		App->render->AddSprite(&buildingTileN, SCENE);
	}
	*/
	createBuilding = true;
}

bool M_EntityManager::IsBuildingCreationWalkable(int x, int y, Building_Type type) const
{
	bool ret = true;

	if (type != ASSIMILATOR)
	{
		const BuildingStatsData* buildingStats = GetBuildingStats(type);
		for (int h = 0; h < buildingStats->height_tiles; h++)
		{
			for (int w = 0; w < buildingStats-> width_tiles; w++)
			{
				iPoint pos = App->pathFinding->MapToWorld(x + w, y + h);
				SDL_Rect rect = { pos.x, pos.y, 2 * 8, 2 * 8 };
				if (!App->pathFinding->IsWalkable(x + w, y + h))
				{
					App->render->AddRect(rect, true, 255, 0, 0, 100, true);
					ret = false;
				}
				else
				{
					App->render->AddRect(rect, true, 0, 250, 0, 100, true);
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

	for (int h = 0; h < resourceStats->height_tiles; h++)
	{
		for (int w = 0; w < resourceStats->width_tiles; w++)
		{
			if (!App->pathFinding->IsWalkable(x + w , y + h))
			{
				ret = false;
			}
		}
	}
	return ret;
}

bool M_EntityManager::deleteUnit(std::list<Unit*>::iterator it)
{
	App->sceneMap->player.psi -= (*it)->psi;
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
	App->sceneMap->player.maxPsi -= (*it)->psi;
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
		return (I_Rect_Rect(rect, itemRect));
	}
}

void M_EntityManager::SetUnitHover(Unit* unit)
{
	hoveringUnit = unit;
	hoveringBuilding = NULL;
	hoveringResource = NULL;
}

void M_EntityManager::SetBuildingHover(Building* building)
{
	hoveringUnit = NULL;
	hoveringBuilding = building;
	hoveringResource = NULL;
}

void M_EntityManager::SetResourceHover(Resource* resource)
{
	hoveringUnit = NULL;
	hoveringBuilding = NULL;
	hoveringResource = resource;
}

void M_EntityManager::SendNewPath(int x, int y, Attack_State state)
{
	if (App->pathFinding->IsWalkable(x, y))
	{
		//Moving group rectangle to the destination point
		iPoint Rcenter = App->pathFinding->MapToWorld(x, y);
		destinationRect = { Rcenter.x - groupRect.w / 2, Rcenter.y - groupRect.h / 2, groupRect.w, groupRect.h };
		bool ignoreRect = false;
		if (groupRect.h > 300 ||  groupRect.w > 300)
		{
			destinationRect.x = Rcenter.x;
			destinationRect.y = Rcenter.y;
			ignoreRect = true;
		}

		//Iteration through all selected units
		std::list<Unit*>::iterator it = selectedUnits.begin();
		while (it != selectedUnits.end())
		{
			std::vector<iPoint> newPath;

			//Distance from rectangle position to unit position
			iPoint posFromRect;
			if (!ignoreRect)
			{
				posFromRect.x = (*it)->GetPosition().x - groupRect.x;
				posFromRect.y = (*it)->GetPosition().y - groupRect.y;
			}
			else
			{
				posFromRect = { 0, 0 };
			}

			//Destination tile: destination rectangle + previous distance
			iPoint dstTile = App->pathFinding->WorldToMap(destinationRect.x + posFromRect.x, destinationRect.y + posFromRect.y);

			//Unit tile position
			fPoint unitPos = (*it)->GetPosition();
			iPoint unitTile = App->pathFinding->WorldToMap(round(unitPos.x), round(unitPos.y));

			//If destination is not walkable, use the player's clicked tile
			if (!App->pathFinding->IsWalkable(dstTile.x, dstTile.y))
				dstTile = { x, y };

			//If a path is found, send it to the unit
			(*it)->Move(dstTile, state);

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
	if (building->GetType() == ASSIMILATOR)
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
}

void M_EntityManager::SendToAttack(Unit* unit)
{
	std::list<Unit*>::iterator it = selectedUnits.begin();

	while (it != selectedUnits.end())
	{
		if ((*it)->GetType() == PROBE)
		{
			(*it)->SetAttack(unit);
		}
		it++;
	}
}

void M_EntityManager::SendToAttack(Building* building)
{
	std::list<Unit*>::iterator it = selectedUnits.begin();

	while (it != selectedUnits.end())
	{
		if ((*it)->GetType() == PROBE)
		{
			(*it)->SetAttack(building);
		}
		it++;
	}
}

void M_EntityManager::SendToAttack(int x, int y)
{
	iPoint dst = App->pathFinding->WorldToMap(x, y);
	SendNewPath(dst.x, dst.y, ATTACK_ATTACK);
	attackUnits = false;
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
		if ((*it)->GetType() == MINERAL && !(*it)->gatheringUnit && (*it)->resourceAmount)
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
	buildingCenter.x += (building->width_tiles / 2);
	buildingCenter.y += (building->width_tiles / 2);
	bool maxX = false, maxY = false;

	if (unitPos.x > buildingCenter.x)
		maxX = 1;
	if (unitPos.y > buildingCenter.y)
		maxY = 1;

	iPoint ret = { (int)buildingPos.x + building->width_tiles * maxX, (int)buildingPos.y + building->height_tiles * maxY };
	maxX ? ret.x += 1 : ret.x -= 1;
	maxY ? ret.y += 1 : ret.y -= 1;
	if (!App->pathFinding->IsWalkable(ret.x, ret.y))
	{
		maxX = !maxX;
		ret = { (int)buildingPos.x + building->width_tiles * maxX, (int)buildingPos.y + building->height_tiles * maxY };
		maxX ? ret.x += 1 : ret.x -= 1;
		maxY ? ret.y += 1 : ret.y -= 1;

		if (!App->pathFinding->IsWalkable(ret.x, ret.y))
		{
			maxY = !maxY;
			ret = { (int)buildingPos.x + building->width_tiles * maxX, (int)buildingPos.y + building->height_tiles * maxY };
			maxX ? ret.x += 1 : ret.x -= 1;
			maxY ? ret.y += 1 : ret.y -= 1;

			if (!App->pathFinding->IsWalkable(ret.x, ret.y))
			{
				maxX = !maxX;
				ret = { (int)buildingPos.x + building->width_tiles * maxX, (int)buildingPos.y + building->height_tiles * maxY };
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
	resourceCenter.x += (resource->width_tiles / 2);
	resourceCenter.y += (resource->width_tiles / 2);

	int maxX = 0, maxY = 0;

	if (unitPos.x > resourceCenter.x)
		maxX = 1;
	if (unitPos.y > resourceCenter.y)
		maxY = 1;


	iPoint ret = { (int)resourcePos.x + resource->width_tiles * maxX, (int)resourcePos.y + resource->height_tiles * maxY };
	if (!App->pathFinding->IsWalkable(ret.x, ret.y))
	{
		maxX = !maxX;
		ret = { (int)resourcePos.x + resource->width_tiles * maxX, (int)resourcePos.y + resource->height_tiles * maxY };
		maxX ? ret.x += 1 : ret.x -= 1;
		maxY ? ret.y += 1 : ret.y -= 1;

		if (!App->pathFinding->IsWalkable(ret.x, ret.y))
		{
			maxY = !maxY;
			ret = { (int)resourcePos.x + resource->width_tiles * maxX, (int)resourcePos.y + resource->height_tiles * maxY };
			maxX ? ret.x += 1 : ret.x -= 1;
			maxY ? ret.y += 1 : ret.y -= 1;

			if (!App->pathFinding->IsWalkable(ret.x, ret.y))
			{
				maxX = !maxX;
				ret = { (int)resourcePos.x + resource->width_tiles * maxX, (int)resourcePos.y + resource->height_tiles * maxY };
				maxX ? ret.x += 1 : ret.x -= 1;
				maxY ? ret.y += 1 : ret.y -= 1;
			}
		}
	}
	return ret;
}

const UnitStatsData* M_EntityManager::GetUnitStats(Unit_Type type) const
{
	return unitsLibrary.GetStats(type);
}

const UnitSpriteData* M_EntityManager::GetUnitSprite(Unit_Type type) const
{
	return unitsLibrary.GetSprite(type);
}

const BuildingStatsData* M_EntityManager::GetBuildingStats(Building_Type type) const
{
	return buildingsLibrary.GetStats(type);
}
const BuildingSpriteData* M_EntityManager::GetBuildingSprite(Building_Type type) const
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
const HPBarData* M_EntityManager::GetHPBarSprite(int type) const
{
	return &HPBars[type];
}

void M_EntityManager::UpdateSpriteRect(Unit* unit, C_Sprite& sprite, float dt)
{
	//Rectangle definition variables
	int direction, size, rectX, rectY;
	const UnitSpriteData* unitData = unitsLibrary.GetSprite(unit->GetType());

	//Getting unit movement direction----
	float angle = unit->GetVelocity().GetAngle() - 90;
	if (angle < 0)
		angle = 360 + angle;
	angle = 360 - angle;
	direction = angle / (360 / 32);

	if (direction > 16)
	{
		sprite.flip = SDL_FLIP_HORIZONTAL;
		direction -= 16;
		rectX = 16 * unitData->size - direction * unitData->size;
	}
	else
	{
		sprite.flip = SDL_FLIP_NONE;
		rectX = direction * unitData->size;
	}
	//----------------------------------
	//Getting rect from frame
	int min, max;
	unitsLibrary.GetStateLimits(unit->GetType(), unit->GetMovementState(), min, max);

	unit->currentFrame += unitData->animationSpeed * dt;

	if (unit->currentFrame >= max + 1)
		unit->currentFrame = min;

	if (unit->GetMovementType() == FLYING)
	{
		if ((int)unit->currentFrame == 2 || (int)unit->currentFrame == 0)
			unit->flyingOffset = 0;
		else if ((int)unit->currentFrame == 1)
			unit->flyingOffset = -2;
		else if ((int)unit->currentFrame == 3)
			unit->flyingOffset = 2;
		rectY = 0;
	}
	else
	{
		rectY = (int)unit->currentFrame * unitData->size;
	}
	sprite.section = { rectX, rectY, unitData->size, unitData->size };
	if (unit->GetMovementType() == FLYING)
	{
		sprite.position.y = (int)round(unit->GetPosition().y - unitData->size / 2) + unit->flyingOffset;
	}
}

//Call for this function every time the unit state changes (starts moving, starts idle, etc)
void M_EntityManager::UpdateCurrentFrame(Unit* unit)
{
	const UnitSpriteData* data = unitsLibrary.GetSprite(unit->GetType());
	switch (unit->GetMovementState())
	{
	case(MOVEMENT_IDLE) :
	{
		unit->currentFrame = data->idle_line_start;
		break;
	}
	case(MOVEMENT_ATTACK) :
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

void M_EntityManager::MoveSelectedUnits()
{
	if (hoveringResource)
	{
		if (hoveringResource->resourceAmount)
			SendToGather(hoveringResource);
	}
	else if (hoveringBuilding)
	{
		if (hoveringBuilding->GetType() == ASSIMILATOR)
			SendToGather(hoveringBuilding);
		else if (hoveringBuilding->stats.player == COMPUTER)
			SendToAttack(hoveringBuilding);
	}
	else if (hoveringUnit)
	{
		SendToAttack(hoveringUnit);
	}
	else
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint pos = App->render->ScreenToWorld(x, y);
		iPoint tile = App->pathFinding->WorldToMap(pos.x, pos.y);
		SendNewPath(tile.x, tile.y, ATTACK_STAND);
	}
	moveUnits = false;
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
		if (tmp == "Carrier")
			unitsLibrary.types.push_back(CARRIER);
		else if (tmp == "Observer")
			unitsLibrary.types.push_back(OBSERVER);
		else if (tmp == "Probe")
			unitsLibrary.types.push_back(PROBE);
		else if (tmp == "Shuttle")
			unitsLibrary.types.push_back(SHUTTLE);
		else if (tmp == "Arbiter")
			unitsLibrary.types.push_back(ARBITER);
		else if (tmp == "Scout")
			unitsLibrary.types.push_back(SCOUT);
		else if (tmp == "Reaver")
			unitsLibrary.types.push_back(REAVER);
		else if (tmp == "Zealot")
			unitsLibrary.types.push_back(ZEALOT);
		else if (tmp == "Archon")
			unitsLibrary.types.push_back(ARCHON_T);
		else if (tmp == "High Templar")
			unitsLibrary.types.push_back(HIGH_TEMPLAR);
		else if (tmp == "Dark Templar")
			unitsLibrary.types.push_back(DARK_TEMPLAR);
		else if (tmp == "Dragoon")
			unitsLibrary.types.push_back(DRAGOON);
		else if (tmp == "Interceptor")
			unitsLibrary.types.push_back(INTERCEPTOR);
		else if (tmp == "Corsair")
			unitsLibrary.types.push_back(CORSAIR);

		UnitStatsData stats;
		stats.type = node.child("type").attribute("value").as_int();
		stats.invisible = node.child("invisible").attribute("value").as_bool();
		stats.HP = node.child("HP").attribute("value").as_int();
		stats.shield = node.child("shield").attribute("value").as_int();
		stats.energy = node.child("energy").attribute("value").as_int();
		stats.armor = node.child("armor").attribute("value").as_int();
		stats.psi = node.child("psi").attribute("value").as_int();
		stats.cooldown = node.child("cooldown").attribute("value").as_float();
		stats.speed = node.child("speed").attribute("value").as_float();
		stats.visionRange = node.child("vision_range").attribute("value").as_int();
		stats.detectionRange = node.child("detection_range").attribute("value").as_int();
		stats.attackRange = node.child("attack_range").attribute("value").as_int();
		stats.buildTime = node.child("build_time").attribute("value").as_int();
		if (node.child("flying").attribute("value").as_bool())
		{
			stats.movementType = FLYING;
		}
		else
		{
			stats.movementType = GROUND;
		}

		unitsLibrary.stats.push_back(stats);
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
		if (tmp == "Nexus")
			buildingsLibrary.types.push_back(NEXUS);
		else if (tmp == "Pylon")
			buildingsLibrary.types.push_back(PYLON);
		else if (tmp == "Assimilator")
			buildingsLibrary.types.push_back(ASSIMILATOR);
		else if (tmp == "Gateway")
			buildingsLibrary.types.push_back(GATEWAY);
		else if (tmp == "Zerg Sample")
			buildingsLibrary.types.push_back(ZERG_SAMPLE);
		else if (tmp == "Zerg Sample")
			buildingsLibrary.types.push_back(CYBERNETICS_CORE);
		else if (tmp == "Zerg Sample")
			buildingsLibrary.types.push_back(FORGE);
		else if (tmp == "Zerg Sample")
			buildingsLibrary.types.push_back(PHOTON_CANNON);
		BuildingStatsData stats;
		stats.HP = node.child("HP").attribute("value").as_int();
		stats.shield = node.child("shield").attribute("value").as_int();
		stats.armor = node.child("armor").attribute("value").as_int();
		stats.mineralCost = node.child("mineral_cost").attribute("value").as_int();
		stats.gasCost = node.child("gas_cost").attribute("value").as_int();
		stats.width_tiles = node.child("width_tiles").attribute("value").as_int();
		stats.height_tiles = node.child("height_tiles").attribute("value").as_int();
		stats.visionRange = node.child("vision_range").attribute("value").as_int();
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
		UnitSpriteData sprite;
		sprite.texture = App->tex->Load(node.child("file").attribute("name").as_string());
		sprite.size = node.child("size").attribute("value").as_int();
		sprite.animationSpeed = node.child("animationSpeed").attribute("value").as_float();
		sprite.idle_line_start = node.child("idle_line_start").attribute("value").as_int();
		sprite.idle_line_end = node.child("idle_line_end").attribute("value").as_int();
		sprite.run_line_start = node.child("run_line_start").attribute("value").as_int();
		sprite.run_line_end = node.child("run_line_end").attribute("value").as_int();
		sprite.attack_line_start = node.child("attack_line_start").attribute("value").as_int();
		sprite.attack_line_end = node.child("attack_line_end").attribute("value").as_int();

		sprite.HPBar_type = node.child("HPBar_type").attribute("value").as_int();

		sprite.shadow.texture = App->tex->Load(node.child("shadow").child("file").attribute("name").as_string());
		sprite.shadow.size_x = node.child("shadow").child("size_x").attribute("value").as_int();
		sprite.shadow.size_y = node.child("shadow").child("size_y").attribute("value").as_int();
		sprite.shadow.offset_x = node.child("shadow").child("offset_x").attribute("value").as_int();
		sprite.shadow.offset_y = node.child("shadow").child("offset_y").attribute("value").as_int();

		sprite.base.texture = App->tex->Load(node.child("base").child("file").attribute("name").as_string());
		sprite.base.size_x = node.child("base").child("size_x").attribute("value").as_int();
		sprite.base.size_y = node.child("base").child("size_y").attribute("value").as_int();
		sprite.base.offset_x = node.child("base").child("offset_x").attribute("value").as_int();
		sprite.base.offset_y = node.child("base").child("offset_y").attribute("value").as_int();

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
		BuildingSpriteData sprite;
		sprite.texture = App->tex->Load(node.child("file").attribute("name").as_string());
		sprite.size_x = node.child("size_x").attribute("value").as_int();
		sprite.size_y = node.child("size_y").attribute("value").as_int();
		sprite.offset_x = node.child("offset_x").attribute("value").as_int();
		sprite.offset_y = node.child("offset_y").attribute("value").as_int();

		sprite.HPBar_type = node.child("HPBar_type").attribute("value").as_int();

		sprite.shadow.texture = App->tex->Load(node.child("shadow").child("file").attribute("name").as_string());
		sprite.shadow.size_x = node.child("shadow").child("size_x").attribute("value").as_int();
		sprite.shadow.size_y = node.child("shadow").child("size_y").attribute("value").as_int();
		sprite.shadow.offset_x = node.child("shadow").child("offset_x").attribute("value").as_int();
		sprite.shadow.offset_y = node.child("shadow").child("offset_y").attribute("value").as_int();

		sprite.base.texture = App->tex->Load(node.child("base").child("file").attribute("name").as_string());
		sprite.base.size_x = node.child("base").child("size_x").attribute("value").as_int();
		sprite.base.size_y = node.child("base").child("size_y").attribute("value").as_int();
		sprite.base.offset_x = node.child("base").child("offset_x").attribute("value").as_int();
		sprite.base.offset_y = node.child("base").child("offset_y").attribute("value").as_int();

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

		sprite.shadow.texture = App->tex->Load(node.child("shadow").child("file").attribute("name").as_string());
		sprite.shadow.size_x = node.child("shadow").child("size_x").attribute("value").as_int();
		sprite.shadow.size_y = node.child("shadow").child("size_y").attribute("value").as_int();
		sprite.shadow.offset_x = node.child("shadow").child("offset_x").attribute("value").as_int();
		sprite.shadow.offset_y = node.child("shadow").child("offset_y").attribute("value").as_int();

		sprite.base.texture = App->tex->Load(node.child("base").child("file").attribute("name").as_string());
		sprite.base.size_x = node.child("base").child("size_x").attribute("value").as_int();
		sprite.base.size_y = node.child("base").child("size_y").attribute("value").as_int();
		sprite.base.offset_x = node.child("base").child("offset_x").attribute("value").as_int();
		sprite.base.offset_y = node.child("base").child("offset_y").attribute("value").as_int();

		resourcesLibrary.sprites.push_back(sprite);
	}

	return ret;
}

bool M_EntityManager::LoadHPBars()
{
	bool ret = true;

	HPBarData bar1;
	bar1.empty = App->tex->Load("graphics/ui/protoss/HP Bars/HP1_Empty.png");
	bar1.fill = App->tex->Load("graphics/ui/protoss/HP Bars/HP1_Fill.png");
	bar1.shield = App->tex->Load("graphics/ui/protoss/HP Bars/HP1_Shield.png");
	bar1.size_x = 109;

	HPBars.push_back(bar1);

	HPBarData bar2;
	bar2.empty = App->tex->Load("graphics/ui/protoss/HP Bars/HP2_Empty.png");
	bar2.fill = App->tex->Load("graphics/ui/protoss/HP Bars/HP2_Fill.png");
	bar2.shield = App->tex->Load("graphics/ui/protoss/HP Bars/HP2_Shield.png");
	bar2.size_x = 91;

	HPBars.push_back(bar2);

	HPBarData bar3;
	bar3.empty = App->tex->Load("graphics/ui/protoss/HP Bars/HP3_Empty.png");
	bar3.fill = App->tex->Load("graphics/ui/protoss/HP Bars/HP3_Fill.png");
	bar3.shield = App->tex->Load("graphics/ui/protoss/HP Bars/HP3_Shield.png");
	bar3.size_x = 52;

	HPBars.push_back(bar3);

	HPBarData bar4;
	bar4.empty = App->tex->Load("graphics/ui/protoss/HP Bars/HP4_Empty.png");
	bar4.fill = App->tex->Load("graphics/ui/protoss/HP Bars/HP4_Fill.png");
	bar4.shield = App->tex->Load("graphics/ui/protoss/HP Bars/HP4_Shield.png");
	bar4.size_x = 31;

	HPBars.push_back(bar4);

	HPBarData bar5;
	bar5.empty = App->tex->Load("graphics/ui/protoss/HP Bars/HP5_Empty.png");
	bar5.fill = App->tex->Load("graphics/ui/protoss/HP Bars/HP5_Fill.png");
	bar5.shield = App->tex->Load("graphics/ui/protoss/HP Bars/HP5_Shield.png");
	bar5.size_x = 22;

	HPBars.push_back(bar5);

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
	App->gui->SetCurrentGrid(building->GetType());
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
	selectedResource = resource;
	App->gui->SetCurrentGrid(G_DEFAULT);
}

void M_EntityManager::UnselectResource(Resource* resource)
{
	resource->selected = false;
}

void M_EntityManager::DoSingleSelection()
{
	if (!selectedUnits.empty())
	{
		std::list<Unit*>::iterator it = selectedUnits.begin();
		std::list<Unit*>::iterator it2;
		while (it != selectedUnits.end())
		{
			it2 = it;
			it2++;
			UnselectUnit(*it);
			it = it2;
		}
	}
	if (selectedBuilding)
		UnselectBuilding(selectedBuilding);
	if (selectedResource)
		UnselectResource(selectedResource);

	if (hoveringUnit)
	{
		SelectUnit(hoveringUnit);
		App->gui->SetCurrentGrid(hoveringUnit->GetType(), false);
	}
	else if (hoveringBuilding)
		SelectBuilding(hoveringBuilding);
	else if (hoveringResource)
		SelectResource(hoveringResource);
}

#pragma endregion

void M_EntityManager::DrawDebug()
{
	if (!selectedUnits.empty())
		App->render->AddDebugRect(groupRect, true, 255, 0, 0, 255, false);

	App->render->AddDebugRect(destinationRect, true, 255, 255, 0, 255, false);
}

//Orders

void  M_EntityManager::addOrder(Order& nOrder, UI_Button2* nButt)
{
	if (nButt != NULL)
		nOrder.SetButton(*nButt);
	orders.push_back(&nOrder);
}
