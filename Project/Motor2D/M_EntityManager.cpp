#include "M_EntityManager.h"

#include "j1App.h"
#include "M_Textures.h"
#include "Unit.h"
#include "Boss.h"
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
#include "M_FogOfWar.h"


// ---- Units library --------------------------------------------------------------------------------------------

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
	case (MOVEMENT_ATTACK_IDLE) :
	{
									min = data->idle_line_start;
									max = data->idle_line_end;
									break;
	}
	case (MOVEMENT_ATTACK_ATTACK) :
	{
									  min = data->attack_line_start;
									  max = data->attack_line_end;
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


// ---- Buildings library --------------------------------------------------------------------------------------------

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


// ---- Resources library --------------------------------------------------------------------------------------------

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


// ---- M_Entity Manager --------------------------------------------------------------------------------------------

// ---- Basic module functions --------------------------------------------------------------------------------------------

M_EntityManager::M_EntityManager(bool start_enabled) : j1Module(start_enabled)
{

}

M_EntityManager::~M_EntityManager()
{

}

bool M_EntityManager::Awake(pugi::xml_node&)
{
	return true;
}

bool M_EntityManager::Start()
{
	LoadUnitsLibrary("entityManager/Unit stats data.xml", "entityManager/Unit sprite data.xml");
	LoadBuildingsLibrary("entityManager/Building stats data.xml", "entityManager/Building sprite data.xml");
	LoadResourcesLibrary("entityManager/Resource stats data.xml", "entityManager/Resource sprite data.xml");
	LoadHPBars();

	walkable_tile = App->tex->Load("graphics/walkable tile.png");
	nonwalkable_tile = App->tex->Load("graphics/building incorrect tile.png");

	gather_mineral_tex = App->tex->Load("graphics/neutral/units/ore chunk.png");
	gather_gas_tex = App->tex->Load("graphics/neutral/units/gas orb.png");
	gather_mineral_shadow_tex = App->tex->Load("graphics/neutral/units/ore chunk shadow.png");
	gather_gas_shadow_tex = App->tex->Load("graphics/neutral/units/gas orb shadow.png");

	probe_spark_tex = App->tex->Load("graphics/neutral/missiles/pspark.png");

	path_tex = App->tex->Load("textures/path.png");

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

	return true;
}

bool M_EntityManager::Update(float dt)
{
	if (!App->sceneMap->onEvent && !App->gui->mouseHover)
		ManageInput();

	if (App->sceneMap->onEvent)
	{
		UnselectAllUnits();
	}

	UpdateFogOfWar();
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
		if (!selectedBuilding && !selectedResource && selectedUnits.empty())
		{
			App->gui->SetCurrentGrid(NULL);
		}
		selectEntities = false;
		startSelection = false;
		selectionStarted = false;
		selectionRect.w = selectionRect.h = 0;
	}

	if (debug)
		DrawDebug();

	if (selectionRect.w != 0 || selectionRect.h != 0)
	{
		App->render->AddRect(selectionRect, false, 0, 255, 0, 255, false);
	}
	if (!startSelection && 	!App->sceneMap->onEvent)
	{
		if (hoveringBuilding)
		{
			if (hoveringBuilding->stats.player != COMPUTER)
				SetMouseState(M_ALLY_HOVER, false);
			else
				SetMouseState(M_ENEMY_HOVER, false);
		}
		else if (hoveringResource)
		{
			SetMouseState(M_RESOURCE_HOVER, false);
		}
		else if (hoveringUnit)
		{
			if (hoveringUnit->stats.player != COMPUTER)
				SetMouseState(M_ALLY_HOVER, false);
			else
				SetMouseState(M_ENEMY_HOVER, false);
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
		std::list<Unit*>::iterator it2 = it;
		while (it != unitsToDelete.end())
		{
			it2 = it;
			it2++;
			deleteUnit(it);
			delete(*it);
			unitsToDelete.remove(*it);
			it = it2;
		}
		unitsToDelete.clear();
	}

	if (!buildingsToDelete.empty())
	{
		std::list<Building*>::iterator it = buildingsToDelete.begin();
		std::list<Building*>::iterator it2 = it;
		while (it != buildingsToDelete.end())
		{
			it2++;
			deleteBuilding(it);
			delete(*it);
			buildingsToDelete.remove(*it);
			it = it2;
		}
		buildingsToDelete.clear();
	}

	if (!resourcesToDelete.empty())
	{
		std::list<Resource*>::iterator it = resourcesToDelete.begin();
		std::list<Resource*>::iterator it2 = it;
		while (it != resourcesToDelete.end())
		{
			it2++;
			deleteResource(it);
			delete(*it);
			resourcesToDelete.remove(*it);
			it = it2;
		}
		resourcesToDelete.clear();
	}

	UpdateMouseSprite(dt);
	App->render->AddSprite(&mouseSprite, GUI);


	return true;
}

bool M_EntityManager::CleanUp()
{
	UnselectAllUnits();
	if (selectedBuilding)
		UnselectBuilding(selectedBuilding);
	if (selectedResource)
		UnselectResource(selectedResource);

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

	App->input->EnableCursorImage();

	unitsLibrary.sprites.clear();
	unitsLibrary.stats.clear();
	unitsLibrary.types.clear();

	buildingsLibrary.sprites.clear();
	buildingsLibrary.stats.clear();
	buildingsLibrary.types.clear();

	resourcesLibrary.sprites.clear();
	resourcesLibrary.stats.clear();
	resourcesLibrary.types.clear();

	App->tex->UnLoad(walkable_tile);
	App->tex->UnLoad(nonwalkable_tile);

	App->tex->UnLoad(gather_mineral_tex);
	App->tex->UnLoad(gather_gas_tex);
	App->tex->UnLoad(gather_mineral_shadow_tex);
	App->tex->UnLoad(gather_gas_shadow_tex);

	App->tex->UnLoad(probe_spark_tex);

	App->tex->UnLoad(path_tex);

	App->tex->UnLoad(buildingTile.texture);
	App->tex->UnLoad(buildingTileN.texture);

	while (!mouseTextures.empty())
	{
		App->tex->UnLoad(mouseTextures.back());
		mouseTextures.pop_back();
	}
	mouseTexturesNumber.clear();

	//Unloading HP Bars
	while (!HPBars.empty())
	{
		App->tex->UnLoad(HPBars.back().empty);
		App->tex->UnLoad(HPBars.back().fill);
		App->tex->UnLoad(HPBars.back().shield);
		HPBars.pop_back();
	}

	return true;
}


// ---- Mouse functions  --------------------------------------------------------------------------------------------

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


// ---- Entities loops  --------------------------------------------------------------------------------------------

void M_EntityManager::UpdateFogOfWar()
{
	if (unitList.empty() == false)
	{
		std::list<Unit*>::iterator unitIt = unitList.begin();
		while (unitIt != unitList.end())
		{
			if ((*unitIt)->stats.player == PLAYER && (*unitIt)->GetMovementState() != MOVEMENT_DIE)
			{
				App->fogOfWar->DrawCircle((*unitIt)->GetPosition().x, (*unitIt)->GetPosition().y, (*unitIt)->stats.visionRange);
			}
			unitIt++;
		}
	}

	if (buildingList.empty() == false)
	{
		std::list<Building*>::iterator buildIt = buildingList.begin();
		{
			while (buildIt != buildingList.end())
			{
				if ((*buildIt)->stats.player == PLAYER && (*buildIt)->state != BS_DEAD)
				{
					App->fogOfWar->DrawCircle((*buildIt)->GetCollider().x + (*buildIt)->GetCollider().w / 2, (*buildIt)->GetCollider().y + (*buildIt)->GetCollider().h / 2, 230); //TODO buildings don't have vision range
				}
				buildIt++;
			}
		}
	}
}

void M_EntityManager::DoUnitLoop(float dt)
{
	//Selection controllers
	Unit_Type selectedType;
	bool unitSelected = false;
	bool multipleUnitsSelected = false;
	bool differentTypesSelected = false;
	bool allySelected = false;
	Unit* enemyToSelect = NULL;

	std::list<Unit*>::iterator it = unitList.begin();

	while (it != unitList.end())
	{
		if ((*it)->active)
		{
			if (selectEntities && (*it)->GetState() != STATE_DIE)
			{
				//Selecting units
				if (IsEntitySelected(*it) && selectedUnits.size() < 12)
				{
					if (unitSelected)
					{
						if (selectedType != (*it)->GetType())
							differentTypesSelected = true;
						multipleUnitsSelected = true;
					}
					selectedType = (*it)->GetType();
					unitSelected = true;
					if ((*it)->stats.player == COMPUTER)
					{
						enemyToSelect = *it;
					}
					else
					{
						allySelected = true;
					}

					if ((*it)->selected == false)
					{
						if ((*it)->stats.player == PLAYER)
							SelectUnit(*it);
					}
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
		if (allySelected)
		{
			if (selectedEnemyUnit)
				UnselectUnit(selectedEnemyUnit);
			if (differentTypesSelected)
				App->gui->SetCurrentGrid(G_BASIC_UNIT);
			else
			{
				App->gui->SetCurrentGrid(selectedType, multipleUnitsSelected);
			}
		}
		else if (enemyToSelect)
		{
			if (selectedEnemyUnit && selectedEnemyUnit->GetState() != STATE_DIE)
				UnselectUnit(selectedEnemyUnit);
			selectedEnemyUnit = enemyToSelect;
			SelectUnit(selectedEnemyUnit);
			App->gui->SetCurrentGrid(NULL);
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
			if (selectEntities && (*it)->state != BS_DEAD)
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
			if (!(*it)->Update(dt))
			{
				buildingsToDelete.push_back(*it);
			}
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
					App->gui->SetCurrentGrid(G_NONE);
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


// ---- Input stuff --------------------------------------------------------------------------------------------

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
				if (!selectedUnits.empty())
					(*selectedUnits.begin())->SendToBuild(buildingCreationType, logicTile);
				createBuilding = false;
			}
		}
		else if (startSelection)
		{
			selectEntities = true;
			UnselectAllUnits();
		}
		else if (!executedOrder && selectionStarted)
		{
			DoSingleSelection();
			selectionStarted = false;
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
			selectionStarted = true;
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if (!executedOrder && !createBuilding && selectionStarted)
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
		{
			createBuilding = false;
			buildingCreationType = NEXUS;
		}
		else if (moveUnits)
			moveUnits = false;
		else if (!selectedUnits.empty())
		{
			MoveSelectedUnits();
		}
		else if (selectedBuilding)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			iPoint worldPos = App->render->ScreenToWorld(x, y);
			iPoint tile = App->pathFinding->WorldToMap(worldPos.x, worldPos.y);
			selectedBuilding->waypointTile = tile;
			selectedBuilding->hasWaypoint = true;
		}
	}
	if (debug && App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		if (selectedResource)
			selectedResource->Extract(300);
	}


	//Enable / Disable debug
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		debug = !debug;
	}
	if (debug)
	{
		//Enable / Disable render
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		{
			render = !render;
		}
		//Enable / Disable shadows
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		{
			shadows = !shadows;
		}
	}
}


// ---- Creation / Spawn units and buildings --------------------------------------------------------------------------------------------

void M_EntityManager::StartUnitCreation(Unit_Type type)
{
	const UnitStatsData* stats = GetUnitStats(type);
	if (selectedBuilding && App->sceneMap->player.psi + stats->psi <= App->sceneMap->player.maxPsi && App->sceneMap->player.mineral >= stats->mineralCost && App->sceneMap->player.gas >= stats->gasCost)
	{
		App->sceneMap->player.psi += stats->psi;
		App->sceneMap->player.mineral -= stats->mineralCost;
		App->sceneMap->player.gas -= stats->gasCost;

		selectedBuilding->AddNewUnit(type, stats->buildTime, stats->psi);
	}
	else
	{
		if (App->sceneMap->player.mineral < stats->mineralCost)
			App->sceneMap->DisplayMineralFeedback();
		else if (App->sceneMap->player.gas < stats->gasCost)
			App->sceneMap->DisplayGasFeedback();
		else if (App->sceneMap->player.maxPsi < stats->psi + App->sceneMap->player.psi)
			App->sceneMap->DisplayPsiFeedback();
	}
}

Unit* M_EntityManager::CreateUnit(int x, int y, Unit_Type type, Player_Type playerType, Building* building)
{
	const UnitStatsData* stats = GetUnitStats(type);
	iPoint tile = App->pathFinding->WorldToMap(x, y);

	Unit* unit = NULL;
	if (type == KERRIGAN)
	{
		unit = new Boss(x, y, type, playerType);
	}
	else
	{
		unit = new Unit(x, y, type, playerType);
	}

	unit->active = true;

	int size = (2 * stats->type + 1);
	unit->SetCollider({ 0, 0, size * 8, size * 8 });

	unit->SetPriority(currentPriority++);
	unit->Start();

	AddUnit(unit);
	if (building)
	{
		if (building->hasWaypoint)
			unit->Move(building->waypointTile, ATTACK_STAND, PRIORITY_MEDIUM);
	}
	return unit;

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
	else
	{
		if (App->sceneMap->player.mineral < stats->mineralCost)
		{
			App->sceneMap->DisplayMineralFeedback();
		}
		else if (App->sceneMap->player.gas < stats->gasCost)
		{
			App->sceneMap->DisplayGasFeedback();
		}
	}
}

Building* M_EntityManager::CreateBuilding(int x, int y, Building_Type type, Player_Type player)
{
	const BuildingStatsData* stats = GetBuildingStats(type);

	if (IsBuildingCreationWalkable(x, y, type))
	{
		Building* building = new Building(x, y, type);

		building->active = true;

		App->sceneMap->player.realMaxPsi += stats->psi;
		App->sceneMap->player.maxPsi = App->sceneMap->player.realMaxPsi;
		if (App->sceneMap->player.maxPsi > 200)
		{
			App->sceneMap->player.maxPsi = 200;
		}
		App->sceneMap->player.mineral -= stats->mineralCost;
		App->sceneMap->player.gas -= stats->gasCost;

		building->stats.player = player;
		building->Start();

		buildingCreationType = NEXUS;

		AddBuilding(building);

		if (type == ASSIMILATOR)
		{
			Resource* gas = FindRes(x, y);
			if (gas)
			{
				building->gasResource = gas;
				gas->active = false;
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
	createBuilding = true;
}

bool M_EntityManager::IsBuildingCreationWalkable(int x, int y, Building_Type type)
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
		Resource* gas = FindRes(x, y);
		if (gas)
		{
			ret = true;
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
	if ((*it)->stats.player == PLAYER)
	{
		App->sceneMap->player.psi -= (*it)->psi;
	}
	if ((*it)->selected)
	{
		selectedUnits.remove(*it);
	}
	if ((*it) == selectedEnemyUnit)
	{
		selectedEnemyUnit = NULL;
	}
	(*it)->Destroy();
	unitList.remove(*it);


	return true;
}

bool M_EntityManager::deleteBuilding(std::list<Building*>::iterator it)
{
	if ((*it)->stats.player == PLAYER)
	{
		App->sceneMap->player.realMaxPsi -= (*it)->psi;
	}
	App->sceneMap->player.maxPsi = App->sceneMap->player.realMaxPsi;
	if (App->sceneMap->player.maxPsi > 200)
	{
		App->sceneMap->player.maxPsi = 200;
	}
	(*it)->Destroy();
	buildingList.remove(*it);

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
			if ((*it)->stats.player == PLAYER && (*it)->GetState() != STATE_DIE)
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
				(*it)->Move(dstTile, state, PRIORITY_HIGH);
			}
			it++;
		}
	}
}

void M_EntityManager::SendToGather(Resource* resource)
{
	std::list<Unit*>::iterator it = selectedUnits.begin();

	while (it != selectedUnits.end())
	{
		if ((*it)->GetType() == PROBE && (*it)->GetState() != STATE_DIE)
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
			if ((*it)->GetType() == PROBE && (*it)->GetState() != STATE_DIE)
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
		if ((*it)->stats.player == PLAYER && (*it)->GetState() != STATE_DIE)
		{
			if (!(!(*it)->stats.canAttackFlying && unit->GetMovementType() == FLYING))
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
		if ((*it)->stats.player == PLAYER && (*it)->GetState() != STATE_DIE)
			(*it)->SetAttack(building);
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

Resource* M_EntityManager::FindRes(int x, int y)
{
	Resource* ret = NULL;
	std::list<Resource*>::iterator it = resourceList.begin();

	while (it != resourceList.end())
	{
		if ((*it)->GetPosition().x == x && (*it)->GetPosition().y == y)
		{
			ret = (*it);
			break;
		}
		else
			it++;
	}

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
	maxX ? ret.x += 0 : ret.x -= 1;
	maxY ? ret.y += 0 : ret.y -= 1;
	if (!App->pathFinding->IsWalkable(ret.x, ret.y))
	{
		maxX = !maxX;
		ret = { (int)buildingPos.x + building->width_tiles * maxX, (int)buildingPos.y + building->height_tiles * maxY };
		maxX ? ret.x += 0 : ret.x -= 1;
		maxY ? ret.y += 0 : ret.y -= 1;

		if (!App->pathFinding->IsWalkable(ret.x, ret.y))
		{
			maxY = !maxY;
			ret = { (int)buildingPos.x + building->width_tiles * maxX, (int)buildingPos.y + building->height_tiles * maxY };
			maxX ? ret.x += 0 : ret.x -= 1;
			maxY ? ret.y += 0 : ret.y -= 1;

			if (!App->pathFinding->IsWalkable(ret.x, ret.y))
			{
				maxX = !maxX;
				ret = { (int)buildingPos.x + building->width_tiles * maxX, (int)buildingPos.y + building->height_tiles * maxY };
				maxX ? ret.x += 0 : ret.x -= 1;
				maxY ? ret.y += 0 : ret.y -= 1;
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
	resourceCenter.y += (resource->height_tiles / 2);

	int maxX = 0, maxY = 0;

	if (unitPos.x > resourceCenter.x)
		maxX = 1;
	if (unitPos.y > resourceCenter.y)
		maxY = 1;


	iPoint ret = { (int)resourcePos.x + resource->width_tiles * maxX, (int)resourcePos.y + resource->height_tiles * maxY };
	maxX ? ret.x += 0 : ret.x -= 1;
	maxY ? ret.y += 0 : ret.y -= 1;
	if (!App->pathFinding->IsWalkable(ret.x, ret.y))
	{
		maxX = !maxX;
		ret = { (int)resourcePos.x + resource->width_tiles * maxX, (int)resourcePos.y + resource->height_tiles * maxY };
		maxX ? ret.x += 0 : ret.x -= 1;
		maxY ? ret.y += 0 : ret.y -= 1;

		if (!App->pathFinding->IsWalkable(ret.x, ret.y))
		{
			maxY = !maxY;
			ret = { (int)resourcePos.x + resource->width_tiles * maxX, (int)resourcePos.y + resource->height_tiles * maxY };
			maxX ? ret.x += 0 : ret.x -= 1;
			maxY ? ret.y += 0 : ret.y -= 1;

			if (!App->pathFinding->IsWalkable(ret.x, ret.y))
			{
				maxX = !maxX;
				ret = { (int)resourcePos.x + resource->width_tiles * maxX, (int)resourcePos.y + resource->height_tiles * maxY };
				maxX ? ret.x += 0 : ret.x -= 1;
				maxY ? ret.y += 0 : ret.y -= 1;
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
	const UnitSpriteData* unitData = unitsLibrary.GetSprite(unit->GetType());

	//Rectangle definition variables
	int direction, size, rectX = 0, rectY = 0;

	if (dt)
	{
		unit->animation.Update(dt);
			
		if (unit->GetMovementState() != MOVEMENT_DIE && unit->GetMovementState() != MOVEMENT_DEAD)
		{
			if (unit->GetMovementState() == MOVEMENT_ATTACK_ATTACK && unit->animation.loopEnd)
			{
				unit->movement_state = MOVEMENT_WAIT;
				UpdateCurrentFrame(unit);
				UpdateSpriteRect(unit, unit->animation.sprite, dt);
			}

			if (unit->GetMovementType() == FLYING && unit->GetType() != MUTALISK)
			{
				if ((int)unit->currentFrame == 2 || (int)unit->currentFrame == 0)
					unit->flyingOffset = 0;
				else if ((int)unit->currentFrame == 1)
					unit->flyingOffset = -2;
				else if ((int)unit->currentFrame == 3)
					unit->flyingOffset = 2;
				rectY = 0;
			}

			if (unit->GetMovementType() == FLYING)
			{
				sprite.position.y = (int)round(unit->GetPosition().y - unitData->size / 2) + unit->flyingOffset;
			}

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
			sprite.section.x = rectX;
		}
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
		unit->animation.currentRect = unit->currentFrame = data->idle_line_start;
		unit->animation.firstRect = data->idle_line_start;
		unit->animation.lastRect = data->idle_line_end;
		unit->animation.loopable = true;
		unit->animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_ATTACK_IDLE) :
	{
		unit->animation.currentRect = unit->currentFrame = data->idle_line_start;
		unit->animation.firstRect = data->idle_line_start;
		unit->animation.lastRect = data->idle_line_end;
		unit->animation.loopable = true;
		unit->animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_ATTACK_ATTACK) :
	{
		unit->animation.currentRect = unit->currentFrame = data->attack_line_start;
		unit->animation.firstRect = data->attack_line_start;
		unit->animation.lastRect = data->attack_line_end;
		unit->animation.loopable = false;
		unit->animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_GATHER) :
	{
		unit->animation.currentRect = unit->currentFrame = data->idle_line_start;
		unit->animation.firstRect = data->idle_line_start;
		unit->animation.lastRect = data->idle_line_end;
		unit->animation.loopable = true;
		unit->animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_MOVE) :
	{
		unit->animation.currentRect = unit->currentFrame = data->run_line_start;
		unit->animation.firstRect = data->run_line_start;
		unit->animation.lastRect = data->run_line_end;
		unit->animation.loopable = true;
		unit->animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_DIE) :
	{
		unit->animation.currentRect = unit->currentFrame = data->death_column_start;
		unit->animation.sprite.section.y = data->size * data->death_line;
		unit->animation.sprite.section.x = 0;
		unit->animation.firstRect = data->death_column_start;
		unit->animation.lastRect = data->death_column_end;
		unit->animation.type = A_RIGHT;
		unit->animation.loopable = false;
	//	unit->animation.animSpeed = 15.0f;
		unit->animation.loopEnd = false;
		break;
	}
	case(MOVEMENT_DEAD) :
	{
		unit->animation.sprite.texture = data->corpse;
		unit->animation.rect_size_x = unit->animation.sprite.section.w = data->deathSize.x;
		unit->animation.rect_size_y = unit->animation.sprite.section.h = data->deathSize.y;
		unit->animation.sprite.position.x = round(unit->GetPosition().x) - data->deathSize.x / 2;
		unit->animation.sprite.position.y = round(unit->GetPosition().y) - data->deathSize.y / 2;
		unit->animation.sprite.section.x = unit->animation.sprite.section.y = 0;
		unit->animation.currentRect = 0;
		unit->animation.firstRect = 0;
		unit->animation.lastRect = data->deathNFrames;
		unit->animation.animSpeed = 1 / (data->deathDuration / data->deathNFrames);
		unit->animation.type = A_DOWN;
		unit->animation.loopable = false;
		unit->animation.loopEnd = false;
		break;
	}
	}
}

void M_EntityManager::MoveSelectedUnits()
{
	if (hoveringResource)
	{
		if (hoveringResource->GetType() == MINERAL && hoveringResource->resourceAmount)
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
		bool found = true;

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
		else if (tmp == "Zergling")
			unitsLibrary.types.push_back(ZERGLING);
		else if (tmp == "Mutalisk")
			unitsLibrary.types.push_back(MUTALISK);
		else if (tmp == "Hydralisk")
			unitsLibrary.types.push_back(HYDRALISK);
		else if (tmp == "Ultralisk")
			unitsLibrary.types.push_back(ULTRALISK);
		else if (tmp == "Kerrigan")
			unitsLibrary.types.push_back(KERRIGAN);
		else if (tmp == "Infested Terran")
			unitsLibrary.types.push_back(INFESTED_TERRAN);
		else
		{
			found = false;
		}

		if (found)
		{
			UnitStatsData stats;
			stats.name = node.child("name").attribute("value").as_string();
			stats.type = node.child("type").attribute("value").as_int();
			stats.invisible = node.child("invisible").attribute("value").as_bool();
			stats.HP = node.child("HP").attribute("value").as_int();
			stats.shield = node.child("shield").attribute("value").as_int();
			stats.energy = node.child("energy").attribute("value").as_int();
			stats.armor = node.child("armor").attribute("value").as_int();
			stats.mineralCost = node.child("mineral_cost").attribute("value").as_int();
			stats.gasCost = node.child("gas_cost").attribute("value").as_int();
			stats.psi = node.child("psi").attribute("value").as_int();
			stats.cooldown = node.child("cooldown").attribute("value").as_float();
			stats.speed = node.child("speed").attribute("value").as_float();
			stats.visionRange = node.child("vision_range").attribute("value").as_int();
			stats.detectionRange = node.child("detection_range").attribute("value").as_int();
			stats.attackRange = node.child("attack_range").attribute("value").as_int();
			stats.buildTime = node.child("build_time").attribute("value").as_int();
			stats.damage = node.child("combat").child("ground").child("vs_small").attribute("value").as_int();
			stats.canAttackFlying = node.child("combat").child("air").attribute("value").as_bool();

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
		bool found = true;
		C_String tmp = node.child("name").attribute("value").as_string();
		if (tmp == "Nexus")
			buildingsLibrary.types.push_back(NEXUS);
		else if (tmp == "Pylon")
			buildingsLibrary.types.push_back(PYLON);
		else if (tmp == "Assimilator")
			buildingsLibrary.types.push_back(ASSIMILATOR);
		else if (tmp == "Gateway")
			buildingsLibrary.types.push_back(GATEWAY);
		else if (tmp == "Cybernetics Core")
			buildingsLibrary.types.push_back(CYBERNETICS_CORE);
		else if (tmp == "Forge")
			buildingsLibrary.types.push_back(FORGE);
		else if (tmp == "Photon Cannon")
			buildingsLibrary.types.push_back(PHOTON_CANNON);
		else if (tmp == "Shield Battery")
			buildingsLibrary.types.push_back(SHIELD_BATTERY);
		else if (tmp == "Stargate")
			buildingsLibrary.types.push_back(STARGATE);
		else if (tmp == "Robotics Facility")
			buildingsLibrary.types.push_back(ROBOTICS_FACILITY);
		else if (tmp == "Fleet Beacon")
			buildingsLibrary.types.push_back(FLEET_BACON);
		else if (tmp == "Citadel of Adun")
			buildingsLibrary.types.push_back(CITADEL);
		else if (tmp == "Templar Archives")
			buildingsLibrary.types.push_back(TEMPLAR_ARCHIVES);
		else if (tmp == "Robotics Support Bay")
			buildingsLibrary.types.push_back(ROBOTICS_BAY);
		else if (tmp == "Arbiter Tribunal")
			buildingsLibrary.types.push_back(ARBITER_TRIBUNAL);
		else if (tmp == "Zerg Sample")
			buildingsLibrary.types.push_back(ZERG_SAMPLE);
		else if (tmp == "Lair")
			buildingsLibrary.types.push_back(LAIR);
		else if (tmp == "Spawning Pool")
			buildingsLibrary.types.push_back(SPAWNING_POOL);
		else if (tmp == "Hydralisk Den")
			buildingsLibrary.types.push_back(HYDRALISK_DEN);
		else if (tmp == "Spire")
			buildingsLibrary.types.push_back(SPIRE);
		else if (tmp == "Ultralisk Cavern")
			buildingsLibrary.types.push_back(ULTRALISK_CAVERN);
		else if (tmp == "Infested Command Center")
			buildingsLibrary.types.push_back(INFESTED_COMMAND_CENTER);
		else if (tmp == "Evolution Chamber")
			buildingsLibrary.types.push_back(EVOLUTION_CHAMBER);
		else if (tmp == "Hive")
			buildingsLibrary.types.push_back(HIVE);
		else if (tmp == "Sunken Colony")
			buildingsLibrary.types.push_back(SUNKEN_COLONY);
		else if (tmp == "Spore Colony")
			buildingsLibrary.types.push_back(SPORE_COLONY);
		else if (tmp == "Creep Colony")
			buildingsLibrary.types.push_back(CREEP_COLONY);
		else if (tmp == "Extractor")
			buildingsLibrary.types.push_back(EXTRACTOR);
		else
		{
			found = false;
		}

		if (found)
		{
			BuildingStatsData stats;
			stats.name = tmp;
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
	for (int n = 0; n < unitsLibrary.stats.size(); n++)
	{
		const UnitStatsData* unitStats = GetUnitStats(static_cast<Unit_Type>(n));
		pugi::xml_node node;
		//Looking for the correct building sprite to load
		for (node = file.child("sprites").child("unit"); node && unitStats->name != node.child("name").attribute("value").as_string(); node = node.next_sibling("unit"))
		{
		}
		if (node != NULL)
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
			sprite.death_line = node.child("death_line").attribute("value").as_int();
			sprite.death_column_start = node.child("death_column_start").attribute("value").as_int();
			sprite.death_column_end = node.child("death_column_end").attribute("value").as_int();
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

			sprite.corpse = App->tex->Load(node.child("death").child("file").attribute("name").as_string());
			if (sprite.corpse)
			{
				sprite.deathNFrames = node.child("death").child("nframes").attribute("value").as_int();
				sprite.deathDuration = node.child("death").child("duration").attribute("value").as_float();
				sprite.deathSize.x = node.child("death").child("size").attribute("x").as_int();
				sprite.deathSize.y = node.child("death").child("size").attribute("y").as_int();
			}
			else
			{
				sprite.deathNFrames = sprite.deathDuration = sprite.deathSize.x = sprite.deathSize.y = 0;
			}

			unitsLibrary.sprites.push_back(sprite);
		}

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
	for (int n = 0; n < buildingsLibrary.stats.size(); n++)
	{
		const BuildingStatsData* buildingStats = GetBuildingStats(static_cast<Building_Type>(n));
		pugi::xml_node node;
		//Looking for the correct building sprite to load
		for (node = file.child("sprites").child("building"); node && buildingStats->name != node.child("name").attribute("value").as_string(); node = node.next_sibling("building"))
		{
		}
		if (node != NULL)
		{
			C_String string(node.child("name").attribute("value").as_string());
			//Loading data
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
	}


	//Loading fire animations
	fire1.sprite.texture = App->tex->Load("graphics/neutral/building burnc.png");
	fire1.sprite.section.w = fire1.rect_size_x = 64;
	fire1.sprite.section.h = fire1.rect_size_y = 96;
	fire1.sprite.useCamera = true;
	fire1.animSpeed = 10.0f;
	fire1.type = A_RIGHT;
	fire1.firstRect = 0;
	fire1.lastRect = 10;

	fire1.sprite.texture = App->tex->Load("graphics/neutral/building burnf.png");
	fire1.sprite.section.w = fire1.rect_size_x = 64;
	fire1.sprite.section.h = fire1.rect_size_y = 96;
	fire1.sprite.useCamera = true;
	fire1.animSpeed = 10.0f;
	fire1.type = A_RIGHT;
	fire1.firstRect = 0;
	fire1.lastRect = 10;

	fire1.sprite.texture = App->tex->Load("graphics/neutral/building burnv.png");
	fire1.sprite.section.w = fire1.rect_size_x = 64;
	fire1.sprite.section.h = fire1.rect_size_y = 96;
	fire1.sprite.useCamera = true;
	fire1.animSpeed = 10.0f;
	fire1.type = A_RIGHT;
	fire1.firstRect = 0;
	fire1.lastRect = 10;

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
	if (selectedBuilding == building)
		selectedBuilding = NULL;
}

void M_EntityManager::SelectResource(Resource* resource)
{
	resource->selected = true;
	selectedResource = resource;
	App->gui->SetCurrentGrid(G_NONE);
}

void M_EntityManager::UnselectResource(Resource* resource)
{
	resource->selected = false;
	selectedResource = NULL;
}

void M_EntityManager::UnselectAllUnits()
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
}

void M_EntityManager::DoSingleSelection()
{
	if (selectedBuilding)
		UnselectBuilding(selectedBuilding);
	if (selectedResource)
		UnselectResource(selectedResource);

	if (hoveringUnit)
	{	/*
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		{
			if (hoveringUnit->stats.player == PLAYER)
			{
				if (hoveringUnit->selected)
					UnselectUnit(hoveringUnit);
				else
					SelectUnit(hoveringUnit);
			}

		}
		else
		{*/
			UnselectAllUnits();
			SelectUnit(hoveringUnit);
			if (hoveringUnit->stats.player == COMPUTER)
			{
				selectedEnemyUnit = hoveringUnit;
				App->gui->SetCurrentGrid(NULL);

			}
			else
			{
				App->gui->SetCurrentGrid(hoveringUnit->GetType(), false);
			}


		//}
	}
	else if (hoveringBuilding)
	{
		UnselectAllUnits();
		SelectBuilding(hoveringBuilding);
	}

	else if (hoveringResource)
	{
		UnselectAllUnits();
		SelectResource(hoveringResource);
	}
	else
	{
		UnselectAllUnits();
		App->gui->SetCurrentGrid(NULL);
	}

}

#pragma endregion

void M_EntityManager::DrawDebug()
{
	if (!selectedUnits.empty())
		App->render->AddDebugRect(groupRect, true, 255, 0, 0, 255, false);

	App->render->AddDebugRect(destinationRect, true, 255, 255, 0, 255, false);
}

//Orders

/*
void  M_EntityManager::addOrder(Order& nOrder, UI_Button2* nButt)
{
	if (nButt != NULL)
		nOrder.SetButton(*nButt);
	orders.push_back(&nOrder);
}
*/
