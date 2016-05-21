#include "M_EntityManager.h"

#include "j1App.h"
//#include "M_Textures.h" //TO CHANGE: e.manager has textures included from somewhere

#include "Unit.h"
#include "Boss.h"
#include "Building.h"
#include "Resource.h"

#include "M_Render.h"
#include "M_InputManager.h"
#include "M_PathFinding.h"
#include "M_IA.h"

#include "M_FileSystem.h"

#include "Intersections.h"
#include "M_FogOfWar.h"
#include "M_Player.h"
#include "M_Minimap.h"
#include "M_Audio.h"

#include "M_Console.h"



#pragma region //Sounds


//Units sounds

void UnitSounds::LoadSoundsFrom(const char* path)
{
	C_String tmp = path;
	tmp += "/death/1.ogg";
	death = App->audio->LoadFx(tmp.GetString());

	tmp = path;
	tmp += "/ready.ogg";
	ready = App->audio->LoadFx(tmp.GetString());

	tmp = path;
	tmp += "/hit00.ogg";
	attack = App->audio->LoadFx(tmp.GetString());

	char* number = new char[4];


	tmp = path;
	tmp += "/acknowledgement/";
	for (int n = 0; n < 4; n++)
	{
		sprintf_s(number, CHAR_BIT, "%i", n + 1);
		C_String tmp2 = tmp;
		tmp2 += number;
		tmp2 += ".ogg";
		acnkowledgement[n] = App->audio->LoadFx(tmp2.GetString());
		if (acnkowledgement[n] == 0)
		{
			break;
		}
		nOfAcnkowledgement = n + 1;
	}

	tmp = path;
	tmp += "/selected/";
	for (int n = 0; n < 4; n++)
	{
		sprintf_s(number, CHAR_BIT, "%i", n + 1);
		C_String tmp2 = tmp;
		tmp2 += number;
		tmp2 += ".ogg";
		selected[n] = App->audio->LoadFx(tmp2.GetString());
		if (selected[n] == 0)
		{
			break;
		}
		nOfselected = n + 1;
	}

	/*tmp = path;
	tmp += "/pissed/";
	for (int n = 0; n < 4; n++)
	{
	sprintf_s(number, CHAR_BIT, "%i", n + 1);
	C_String tmp2 = tmp;
	tmp2 += number;
	tmp2 += ".ogg";
	pissed[n] = App->audio->LoadFx(tmp2.GetString());
	if (pissed[n] == 0)
	{
	break;
	}
	nOfpissed = n + 1;
	}*/
	RELEASE_ARRAY(number);
}

void UnitSounds::PlayFX(soundTypes action)
{
	switch (action)
	{
	case (sound_death) :
	{
		if (death != 0)
		{
			App->audio->PlayFx(death);
		}
		break;
	}
	case (sound_ready) :
	{
		if (ready != 0)
		{
			App->audio->PlayFx(ready);
		}
		break;
	}
	case (sound_attack) :
	{
		if (attack != 0)
		{
			App->audio->PlayFx(attack);
		}
		break;
	}
	case (sound_acnkowledgement) :
	{
		if (nOfAcnkowledgement != 0)
		{
			int r = rand() % nOfAcnkowledgement;
			App->audio->PlayFx(acnkowledgement[r]);
		}
		break;
	}
	case (sound_selected) :
	{
		if (nOfselected != 0)
		{
			int r = rand() % nOfselected;
			App->audio->PlayFx(selected[r]);
		}
		break;
	}
	}
}


//Building sounds

void BuildingSounds::LoadSoundsFrom(const char* path)
{
	C_String tmp = path;
	tmp += ".ogg";
	selected = App->audio->LoadFx(tmp.GetString());

	tmp = path;
	tmp += " att.ogg";
	attack = App->audio->LoadFx(tmp.GetString());
}

void BuildingSounds::PlayFX(soundTypes action)
{
	if (action == sound_selected)
	{
		if (selected != 0)
		{
			App->audio->PlayFx(selected);
		}
	}
	else if (action == sound_attack)
	{
		if (attack != 0)
		{
			App->audio->PlayFx(attack);
		}
	}
}

#pragma endregion





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

uint* BuildingsLibrary::GetBuildingQuantity(Building_Type type)
{
	int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i] == type)
			break;
	}
	return &buildingQuantities[i];
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
	name.create("Entity_Manager");
}

M_EntityManager::~M_EntityManager()
{

}

bool M_EntityManager::Awake(pugi::xml_node&)
{
	App->console->AddCommand(&c_SpawnBuildings);
	App->console->AddCommand(&c_SpawnUnits);
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

	probe_spark_tex = App->tex->Load("graphics/neutral/missiles/ephfire.png");

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
	mouseSprite.useCamera = false;

	//Pylon area load
	pylonArea.texture = App->tex->Load("graphics/protoss/units/pylon area.png");
	pylonArea.section = { 0, 0, 508, 303 };
	pylonArea.useCamera = true;

	highTemplarTrail.texture = App->tex->Load("graphics/protoss/units/high templar particles.png");
	highTemplarTrail.section = { 0, 0, 32, 44 };
	highTemplarTrail.useCamera = true;

	stopLoop = false;

	powerTiles = new uint[App->pathFinding->width * App->pathFinding->height];
	for (uint i = 0; i < App->pathFinding->width * App->pathFinding->height; i++)
	{
		powerTiles[i] = 0;
	}

	//Allocating memory for 500 "empty" units
	for (uint i = 0; i < 500; i++)
	{
		unitList.push_back(Unit{ -100, -100, PROBE, CINEMATIC });
		unitList.back().dead = true;
	}

	for (uint i = 0; i < 150; i++)
	{
		buildingList.push_back(Building{ -500, -500, PYLON, CINEMATIC });
		buildingList.back().dead = true;
	}

	//--------------------------------------

	fogUnitIt = 0;
	fogBuildingIt = 0;
	unitsFogReady = buildingsFogReady = true;

	return true;
}

bool M_EntityManager::Update(float dt)
{
	if (!freezeInput && App->events->hoveringUI == false)
		ManageInput();

	//	if (App->sceneMap->onEvent)
	//	{
	//		UnselectAllUnits();
	//	}

	if (!stopLoop)
	{
		UpdateFogOfWar();
		performanceTimer.Start();
		DoUnitLoop(dt);
		//LOG("Entity manager unit loop took %f ms, with %i units", performanceTimer.ReadMs(), unitCount);
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
				App->gui->SetCurrentGrid(G_NONE);
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
		if (!startSelection && 	freezeInput == false)
		{
			if (App->events->hoveringUI == true)
			{
				SetMouseState(M_DEFAULT, false);
			}
			else if (hoveringBuilding)
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

	//Getting current tile
	iPoint p = App->events->GetMouseOnWorld();
	p = App->pathFinding->WorldToMap(p.x, p.y);
	currentTile_x = p.x;
	currentTile_y = p.y;

	UpdateMouseSprite(dt);
	App->render->AddSprite(&mouseSprite, CURSOR);


	return true;
}

bool M_EntityManager::CleanUp()
{
	UnselectAllUnits();
	if (selectedBuilding)
		UnselectBuilding(selectedBuilding);
	if (selectedResource)
		UnselectResource(selectedResource);

	unitList.clear();
	selectedUnits.clear();
	unitsToDelete.clear();
	for (std::vector<Building>::iterator it = buildingList.begin(); it != buildingList.end(); it++)
	{
		if (it->dead == false)
		{
			it->ChangeTileWalkability(true);
		}
	}
	buildingList.clear();
	RELEASE(boss);
	boss = NULL;

	std::list<Resource*>::iterator it3 = resourceList.begin();
	while (it3 != resourceList.end())
	{
		(*it3)->ChangeTileWalkability(true);
		RELEASE(*it3);
		it3++;
	}
	resourceList.clear();

	unitsLibrary.sprites.clear();
	unitsLibrary.stats.clear();
	unitsLibrary.types.clear();

	buildingsLibrary.sprites.clear();
	buildingsLibrary.stats.clear();
	buildingsLibrary.types.clear();

	resourcesLibrary.sprites.clear();
	resourcesLibrary.stats.clear();
	resourcesLibrary.types.clear();
	
	RELEASE_ARRAY(powerTiles);
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

//----- -Load / Save -----------------------------------------------------------------------------------------------

bool M_EntityManager::Load(pugi::xml_node& data)
{
	App->pathFinding->Disable();
	Disable();
	App->pathFinding->Enable();
	Enable();

	fogUnitIt = 0;
	fogBuildingIt = 0;
	unitsFogReady = false;
	buildingsFogReady = false;
	boss = NULL;
	hoveringBuilding = NULL;
	hoveringResource = NULL;
	hoveringUnit = NULL;
	muteUnitsSounds = true;

#pragma region //Loading resources
	for (pugi::xml_node res = data.child("resource"); res; res = res.next_sibling("resource"))
	{
		int x = res.attribute("x").as_int();
		int y = res.attribute("y").as_int();
		Resource_Type type = static_cast<Resource_Type>(res.attribute("type").as_int());
		int amount = res.attribute("amount").as_int();

		Resource* created = CreateResource(x, y, type);
		if (created)
		{
			created->resourceAmount = amount;
		}
	}
#pragma endregion

#pragma region //Loading Buildings
	std::vector<Building>::iterator currBuilding = buildingList.begin();
	for (pugi::xml_node build = data.child("building"); build && currBuilding != buildingList.end(); build = build.next_sibling("building"), currBuilding++)
	{
		int x = build.attribute("x").as_int();
		int y = build.attribute("y").as_int();
		Building_Type type = static_cast<Building_Type>(build.attribute("type").as_int());
		Player_Type controller = static_cast<Player_Type>(build.attribute("controller").as_int());

		if (type != ASSIMILATOR)
		{
			Building* created = CreateBuilding(x, y, type, controller, true);
			if (created)
			{
				created->FinishSpawn();
				created->currHP = build.attribute("HP").as_int();
				created->stats.shield = build.attribute("shield").as_int();

				for (pugi::xml_node queued = build.child("queue").child("queued"); queued; queued = queued.next_sibling("queued"), currBuilding++)
				{
					Unit_Type type = static_cast<Unit_Type>(queued.attribute("type").as_int());
					selectedBuilding = created;
					StartUnitCreation(type, false);
					selectedBuilding = NULL;
				}
			}
		}
		else
		{
			Resource* gasToBuild = CreateResource(x, y, GAS);
			int gasLeft = build.attribute("GasLeft").as_int();
			gasToBuild->resourceAmount = gasLeft;

			if (gasToBuild)
			{
				Building* created = CreateBuilding(gasToBuild->GetPosition().x, gasToBuild->GetPosition().y, ASSIMILATOR, controller, true);
				if (created)
				{
					created->FinishSpawn();
					created->currHP = build.attribute("HP").as_int();
					created->stats.shield = build.attribute("shield").as_int();
					// Set State
				}
			}
		}
	}
#pragma endregion

#pragma region //Loading Units
	std::vector<Unit>::iterator currUnit = unitList.begin();
	for (pugi::xml_node unit = data.child("unit"); unit && currUnit != unitList.end(); unit = unit.next_sibling("unit"), currUnit++)
	{
		int x = unit.attribute("x").as_int();
		int y = unit.attribute("y").as_int();
		Unit_Type type = static_cast<Unit_Type>(unit.attribute("type").as_int());
		Player_Type controller = static_cast<Player_Type>(unit.attribute("controller").as_int());

		Unit* created = CreateUnit(x, y, type, controller);
		if (created)
		{
			created->currHP = unit.attribute("HP").as_int();
			created->stats.shield = unit.attribute("shield").as_int();

			//Setting path if it had one
			pugi::xml_node path = unit.child("path");
			if (path)
			{
				int x = path.attribute("x").as_int();
				int y = path.attribute("y").as_int();
				created->Move({ x, y }, ATTACK_ATTACK);
			}

			//Setting probes to gather
			int gatherMineral = unit.attribute("gatheringMineral").as_int();
			int gatherGas = unit.attribute("gatheringGas").as_int();

			if (gatherMineral != 0)
			{
				created->SetGathering(FindClosestResource(created));
			}
			else if (gatherGas != 0)
			{
				Building* closestAssimilator = NULL;
				uint distance = UINT_MAX;
				for (std::vector<Building>::iterator it = buildingList.begin(); it != buildingList.end(); it++)
				{
					if (it->GetType() == ASSIMILATOR)
					{
						int x = it->GetCollider().x + it->GetCollider().w / 2;
						int y = it->GetCollider().y + it->GetCollider().h / 2;
						int dist = created->GetPosition().DistanceManhattan({ (float)x, (float)y });
						if (dist < distance)
						{
							closestAssimilator = &(*it);
							distance = dist;
						}
					}
				}
				if (closestAssimilator)
				{
					created->SetGathering(closestAssimilator);
				}
			}
		}
	}
#pragma endregion

#pragma region //Loading Boss

	pugi::xml_node bossNode = data.child("boss");
	if (bossNode)
	{
		int x = bossNode.attribute("x").as_int();
		int y = bossNode.attribute("y").as_int();

		Unit* created = CreateUnit(x, y, KERRIGAN, COMPUTER);
		if (created)
		{
			boss = (Boss*)created;
			App->IA->bossPhase = true;
			App->IA->boss = boss;
			created->currHP = bossNode.attribute("HP").as_int();
			created->stats.shield = bossNode.attribute("shield").as_int();
		}
	}

#pragma endregion

	muteUnitsSounds = false;
	return true;
}

// Save Game State
bool M_EntityManager::Save(pugi::xml_node& data) const
{
#pragma region	//Saving boss

	if (boss)
	{
		pugi::xml_node currUnit = data.append_child("boss");
		currUnit.append_attribute("name") = boss->name.GetString();

		currUnit.append_attribute("x") = boss->GetPosition().x;
		currUnit.append_attribute("y") = boss->GetPosition().y;

		currUnit.append_attribute("HP") = boss->currHP;
		currUnit.append_attribute("shield") = boss->stats.shield;

		currUnit.append_attribute("movState") = boss->GetMovementState();
		currUnit.append_attribute("state") = boss->GetState();
	}

#pragma endregion


#pragma region	// Saving Units
	std::vector<Unit>::const_iterator unit = unitList.cbegin();
	while (unit != unitList.cend())
	{
		if (unit->dead == false)
		{
			if (unit->GetState() != STATE_DIE && unit->stats.player == PLAYER)
			{
				pugi::xml_node currUnit = data.append_child("unit");
				currUnit.append_attribute("name") = unit->name.GetString();

				currUnit.append_attribute("x") = unit->GetPosition().x;
				currUnit.append_attribute("y") = unit->GetPosition().y;
				currUnit.append_attribute("type") = unit->stats.type;
				currUnit.append_attribute("controller") = unit->stats.player;

				currUnit.append_attribute("HP") = unit->currHP;
				currUnit.append_attribute("shield") = unit->stats.shield;

				currUnit.append_attribute("movState") = unit->GetMovementState();
				currUnit.append_attribute("state") = unit->GetState();

				bool gathering = false;
				if (unit->stats.type == PROBE && unit->gatheringResource)
				{
					currUnit.append_attribute("gatheringMineral") = 1;
					gathering = true;
				}
				else
				{
					currUnit.append_attribute("gatheringMineral") = 0;
				}

				if (unit->stats.type == PROBE && unit->gatheringBuilding)
				{
					currUnit.append_attribute("gatheringGas") = 1;
					gathering = true;
				}
				else
				{
					currUnit.append_attribute("gatheringGas") = 0;
				}

				if (unit->path.empty() == false && gathering == false)
				{
					pugi::xml_node path = currUnit.append_child("path");					
					path.append_attribute("x") = unit->path.back().x;
					path.append_attribute("y") = unit->path.back().y;

				}
			}
		}
		unit++;
	}
#pragma endregion

#pragma region	//Saving Buildings
	std::vector<Building>::const_iterator building = buildingList.cbegin();
	while (building != buildingList.cend())
	{
		if (building->dead == false)
		{
			if (building->state != BS_DEAD && building->stats.player == PLAYER)
			{
				pugi::xml_node currBuild = data.append_child("building");
				currBuild.append_attribute("name") = building->name.GetString();

				currBuild.append_attribute("x") = building->GetPosition().x;
				currBuild.append_attribute("y") = building->GetPosition().y;
				currBuild.append_attribute("type") = building->GetType();
				currBuild.append_attribute("controller") = building->stats.player;

				currBuild.append_attribute("HP") = building->currHP;
				currBuild.append_attribute("shield") = building->stats.shield;

				currBuild.append_attribute("state") = building->state;

				if (building->GetType() == ASSIMILATOR)
				{
					if (building->gasResource)
					{
						currBuild.append_attribute("GasLeft") = building->gasResource->resourceAmount;
					}
				}

				if (building->hasWaypoint)
				{
					currBuild.append_attribute("waypointX") = building->waypointTile.x;
					currBuild.append_attribute("waypointY") = building->waypointTile.y;
				}

				if (building->queue.count > 0)
				{
					pugi::xml_node queue = currBuild.append_child("queue");
					std::list<Unit_Type>::const_iterator it = building->queue.units.cbegin();
					while (it != building->queue.units.cend())
					{
						pugi::xml_node queued = queue.append_child("queued");
						queued.append_attribute("type") = (*it);
						it++;
					}
				}
			}
		}
		building++;
	}
#pragma endregion

#pragma region	//Saving Resources
	std::list<Resource*>::const_iterator resource = resourceList.cbegin();
	while (resource != resourceList.cend())
	{
			if ((*resource)->active == true)
			{
				pugi::xml_node currRes = data.append_child("resource");
				currRes.append_attribute("type") = (*resource)->GetType();

				currRes.append_attribute("x") = (*resource)->GetPosition().x;
				currRes.append_attribute("y") = (*resource)->GetPosition().y;
				currRes.append_attribute("amount") = (*resource)->resourceAmount;	
			}
		resource++;
	}
#pragma endregion

	return true;
}


// ---- Mouse functions  --------------------------------------------------------------------------------------------

void M_EntityManager::UpdateMouseSprite(float dt)
{
	iPoint mousePos = App->events->GetMouseOnScreen();
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
	if (unitsFogReady && buildingsFogReady)
	{
		if (boss)
		{
			App->fogOfWar->DrawCircle(boss->GetPosition().x, boss->GetPosition().y, boss->stats.visionRange, true, 2);
			App->fogOfWar->DrawCircle(boss->GetPosition().x, boss->GetPosition().y, boss->stats.visionRange, true, 0);
		}
		App->fogOfWar->Copy(2, 1);
		App->fogOfWar->ClearMap(2);
		fogUnitIt = 0;
		fogBuildingIt = 0;
		unitsFogReady = buildingsFogReady = false;
	}
	for (int n = 0; n < 5 && (unitsFogReady == false || buildingsFogReady == false); n++)
	{
		if (unitList.empty() == false)
		{
			if (unitsFogReady == false)
			{
				if ((unitList[fogUnitIt].dead == false && unitList[fogUnitIt].GetMovementState() != MOVEMENT_DEAD && unitList[fogUnitIt].stats.player == PLAYER))
				{
					App->fogOfWar->DrawCircle(unitList[fogUnitIt].GetPosition().x, unitList[fogUnitIt].GetPosition().y, unitList[fogUnitIt].stats.visionRange, true, 2);
					App->fogOfWar->DrawCircle(unitList[fogUnitIt].GetPosition().x, unitList[fogUnitIt].GetPosition().y, unitList[fogUnitIt].stats.visionRange, true, 0);
				}
				else if (unitList[fogUnitIt].dead == false && unitList[fogUnitIt].stats.type == KERRIGAN)
				{
					App->fogOfWar->DrawCircle(unitList[fogUnitIt].GetPosition().x, unitList[fogUnitIt].GetPosition().y, 100, true, 2);
					App->fogOfWar->DrawCircle(unitList[fogUnitIt].GetPosition().x, unitList[fogUnitIt].GetPosition().y, 100, true, 0);
				}
				fogUnitIt++;
				while (fogUnitIt != unitList.size())
				{
					if ((unitList[fogUnitIt].dead == false && unitList[fogUnitIt].GetMovementState() != MOVEMENT_DEAD && unitList[fogUnitIt].stats.player == PLAYER) || unitList[fogUnitIt].stats.type == KERRIGAN)
					{
						break;
					}
					if (fogUnitIt >= unitList.size())
					{
						break;
					}
					fogUnitIt++;
				}
				if (fogUnitIt >= unitList.size())
				{
					unitsFogReady = true;
				}
			}
		}

		if (buildingList.empty() == false)
		{
			if (buildingsFogReady == false)
			{
				if (buildingList[fogBuildingIt].state != BS_DEAD && buildingList[fogBuildingIt].stats.player == PLAYER)
				{
					SDL_Rect collider = buildingList[fogBuildingIt].GetCollider();
					App->fogOfWar->DrawCircle(collider.x + collider.w / 2, collider.y + collider.h / 2, buildingList[fogBuildingIt].stats.visionRange, true, 2);
					App->fogOfWar->DrawCircle(collider.x + collider.w / 2, collider.y + collider.h / 2, buildingList[fogBuildingIt].stats.visionRange, true, 0);
				}
				fogBuildingIt++;
				while (fogBuildingIt != buildingList.size())
				{
					if (buildingList[fogBuildingIt].state != BS_DEAD && buildingList[fogBuildingIt].stats.player == PLAYER)
					{
						break;
					}
					if (fogBuildingIt >= buildingList.size() - 1)
					{
						break;
					}
					fogBuildingIt++;
				}
				if (fogBuildingIt >= buildingList.size() - 1)
				{
					buildingsFogReady = true;
				}
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
	Unit* allySelected = NULL;
	Unit* enemyToSelect = NULL;
#pragma region boss Update
	if (boss)
	{
		if (!boss->dead && boss->active)
		{
			if (selectEntities && boss->GetState() != STATE_DIE)
			{
				//Selecting units
				if (IsEntitySelected(boss) && selectedUnits.size() < 12)
				{
					selectedType = boss->GetType();
					unitSelected = true;
					if (boss->stats.player == COMPUTER)
					{
						enemyToSelect = boss;
					}
					else
					{
						allySelected = boss;
					}

					if (boss->selected == false)
					{
						if (boss->stats.player == PLAYER)
						{
							if (selectedUnits.empty() == true)
							{
								PlayUnitSound(boss->stats.type, sound_selected);
							}
							SelectUnit(boss);
						}
					}
				}
			}
			//Unit update
			boss->Update(dt);
			App->minimap->DrawUnit(boss);
		}
	}
#pragma endregion
	for (int i = 0; i < unitList.size(); i++)
	{
		if (!unitList[i].dead && unitList[i].active)
		{
			if (selectEntities && unitList[i].GetState() != STATE_DIE)
			{
				//Selecting units
				if (IsEntitySelected(&unitList[i]) && selectedUnits.size() < 12)
				{
					if (unitSelected)
					{
						if (selectedType != unitList[i].GetType())
							differentTypesSelected = true;
						multipleUnitsSelected = true;
					}
					selectedType = unitList[i].GetType();
					unitSelected = true;
					if (unitList[i].stats.player == COMPUTER)
					{
						enemyToSelect = &unitList[i];
					}
					else
					{
						allySelected = &unitList[i];
					}

					if (unitList[i].selected == false)
					{
						if (unitList[i].stats.player == PLAYER)
						{
							if (selectedUnits.empty() == true)
							{
								PlayUnitSound(unitList[i].stats.type, sound_selected);
							}
							SelectUnit(&unitList[i]);
						}
					}
				}
			}
			//Unit update
			if (unitList[i].Update(dt))
			{
				App->minimap->DrawUnit(&unitList[i]);
			}
		}
	}
	if (unitSelected)
	{
		if (allySelected != NULL)
		{
			if (selectedEnemyUnit)
				UnselectUnit(selectedEnemyUnit);
			if (differentTypesSelected)
				App->gui->SetCurrentGrid(G_BASIC_UNIT);
			else
			{
				App->gui->SetCurrentGrid(allySelected, multipleUnitsSelected);
			}
		}
		else if (enemyToSelect)
		{
			if (selectedEnemyUnit && selectedEnemyUnit->GetState() != STATE_DIE)
				UnselectUnit(selectedEnemyUnit);
			selectedEnemyUnit = enemyToSelect;
			SelectUnit(selectedEnemyUnit);
			App->gui->SetCurrentGrid(G_NONE);
		}

	}
}

void M_EntityManager::DoBuildingLoop(float dt)
{
	bool buildingSelected = false;
	for (int i = 0; i < buildingList.size(); i++)
	{
		Building* building = &buildingList[i];
		if (building->active && !building->dead)
		{
			if (selectEntities && building->state != BS_DEAD)
			{
				if (IsEntitySelected(building) && !buildingSelected && selectedUnits.empty())
				{
					App->gui->SetCurrentGrid(building);
					SelectBuilding(building);
					buildingSelected = true;
				}
				else if (building->selected)
				{
					UnselectBuilding(building);
				}
			}
			if (building->Update(dt))
			{
				App->minimap->DrawBuilding(building);
			}
		}
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
			App->minimap->DrawResource(*it);
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
	if (App->events->GetEvent(E_LEFT_CLICK) == EVENT_UP)
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

	if (App->events->GetEvent(E_LEFT_CLICK) == EVENT_DOWN)
	{
		if (moveUnits)
		{
			iPoint pos = App->events->GetMouseOnWorld();
			MoveSelectedUnits(pos.x, pos.y);
			executedOrder = true;
		}
		else if (attackUnits)
		{
			iPoint pos = App->events->GetMouseOnWorld();
			SendToAttack(pos.x, pos.y);
			executedOrder = true;
		}
		else if (setWaypoint)
		{
			iPoint worldPos = App->events->GetMouseOnWorld();
			iPoint tile = App->pathFinding->WorldToMap(worldPos.x, worldPos.y);
			selectedBuilding->waypointTile = tile;
			selectedBuilding->hasWaypoint = true;
			setWaypoint = false;
			executedOrder = true;
		}
		else if (!createBuilding)
		{
			selectionRect.x = App->events->GetMouseOnScreen().x;
			selectionRect.y = App->events->GetMouseOnScreen().y;
			selectionStarted = true;
		}
	}

	if (App->events->GetEvent(E_LEFT_CLICK) == EVENT_REPEAT)
	{
		if (!executedOrder && !createBuilding && selectionStarted)
		{
			int x, y;
			x = App->events->GetMouseOnScreen().x;
			y = App->events->GetMouseOnScreen().y;
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

	if (App->events->GetEvent(E_RIGHT_CLICK) == EVENT_DOWN)
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
			iPoint pos = App->events->GetMouseOnWorld();
			MoveSelectedUnits(pos.x, pos.y);
		}
		else if (selectedBuilding)
		{
			iPoint worldPos = App->events->GetMouseOnWorld();
			iPoint tile = App->pathFinding->WorldToMap(worldPos.x, worldPos.y);
			selectedBuilding->waypointTile = tile;
			selectedBuilding->hasWaypoint = true;
		}
	}


	//Enable / Disable debug
	if (App->events->GetEvent(E_DEBUG_ENTITY_MANAGER) == EVENT_DOWN)
	{
		debug = !debug;
	}
	if (debug)
	{
		//Enable / Disable render
		//if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		//{
		//	render = !render;
		//}
		//Enable / Disable shadows
		//if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		//{
		//	shadows = !shadows;
		//}
	}

	if (selectedBuilding)
	{
		if (selectedBuilding->GetType() == NEXUS)
		{
			if (App->events->GetEvent(E_SPAWN_PROBE) == EVENT_DOWN)
			{
				StartUnitCreation(PROBE);
			}
		}

		if (selectedBuilding->GetType() == GATEWAY)
		{
			if (App->events->GetEvent(E_SPAWN_CARRIER) == EVENT_DOWN)
			{
				StartUnitCreation(CARRIER);
			}
			if (App->events->GetEvent(E_SPAWN_SHUTTLE) == EVENT_DOWN)
			{
				StartUnitCreation(SHUTTLE);
			}
			if (App->events->GetEvent(E_SPAWN_ZEALOT) == EVENT_DOWN)
			{
				StartUnitCreation(ZEALOT);
			}
			if (App->events->GetEvent(E_SPAWN_DRAGOON) == EVENT_DOWN)
			{
				if (*GetBuildingQuantity(CYBERNETICS_CORE) >= 1)
				{
					StartUnitCreation(DRAGOON);
				}
			}
			if (App->events->GetEvent(E_SPAWN_SCOUT) == EVENT_DOWN)
			{
				if (*GetBuildingQuantity(ROBOTICS_BAY) >= 1)
				{
					StartUnitCreation(SCOUT);
				}
			}
			if (App->events->GetEvent(E_SPAWN_REAVER) == EVENT_DOWN)
			{
				if (*GetBuildingQuantity(TEMPLAR_ARCHIVES) >= 1)
				{
					StartUnitCreation(REAVER);
				}
			}
			if (App->events->GetEvent(E_SPAWN_OBSERVER) == EVENT_DOWN)
			{
				if (*GetBuildingQuantity(CYBERNETICS_CORE) >= 1)
				{
					StartUnitCreation(OBSERVER);
				}
			}
			if (App->events->GetEvent(E_SPAWN_HIGH_TEMPLAR) == EVENT_DOWN)
			{
				if (*GetBuildingQuantity(TEMPLAR_ARCHIVES) >= 1)
				{
					StartUnitCreation(HIGH_TEMPLAR);
				}
			}
			if (App->events->GetEvent(E_SPAWN_DARK_TEMPLAR) == EVENT_DOWN)
			{
				if (*GetBuildingQuantity(ROBOTICS_BAY) >= 1)
				{
					StartUnitCreation(DARK_TEMPLAR);
				}
			}
		}
	}
	if (selectedUnits.empty() == false)
	{
		if (selectedUnits.size() == 1 && selectedUnits.front()->GetType() == PROBE)
		{
			if (App->events->GetEvent(E_SPAWN_NEXUS) == EVENT_DOWN)
			{
				StartBuildingCreation(NEXUS);
			}
			if (App->events->GetEvent(E_SPAWN_PYLON) == EVENT_DOWN)
			{
				StartBuildingCreation(PYLON);
			}
			if (App->events->GetEvent(E_SPAWN_ASSIMILATOR) == EVENT_DOWN)
			{
				StartBuildingCreation(ASSIMILATOR);
			}
			if (App->events->GetEvent(E_SPAWN_PHOTON_CANNON) == EVENT_DOWN)
			{
				if (*GetBuildingQuantity(CYBERNETICS_CORE) >= 1)
				{
					StartBuildingCreation(PHOTON_CANNON);
				}
			}
			if (App->events->GetEvent(E_SPAWN_GATEWAY) == EVENT_DOWN)
			{
				StartBuildingCreation(GATEWAY);
			}
			if (App->events->GetEvent(E_SPAWN_CYBERNETICS_CORE) == EVENT_DOWN)
			{
				StartBuildingCreation(CYBERNETICS_CORE);
			}
			if (App->events->GetEvent(E_SPAWN_ROBOTICS_BAY) == EVENT_DOWN)
			{
				if (*GetBuildingQuantity(CYBERNETICS_CORE) >= 1)
				{
					StartBuildingCreation(ROBOTICS_BAY);
				}
			}
			if (App->events->GetEvent(E_SPAWN_TEMPLAR_ARCHIVES) == EVENT_DOWN)
			{
				if (*GetBuildingQuantity(ROBOTICS_BAY) >= 1)
				{
					StartBuildingCreation(TEMPLAR_ARCHIVES);
				}
			}
		}
	}

}

void M_EntityManager::FreezeInput()
{
	SetMouseState(M_DEFAULT, false);
	freezeInput = true;
	if (startSelection)
	{
		selectEntities = true;
		UnselectAllUnits();
	}
}

void M_EntityManager::UnfreezeInput()
{
	freezeInput = false;
}
// ---- Creation / Spawn units and buildings --------------------------------------------------------------------------------------------

void M_EntityManager::StartUnitCreation(Unit_Type type, bool useResources)
{
	const UnitStatsData* stats = GetUnitStats(type);
	if (selectedBuilding && selectedBuilding->queue.units.size() < 5)
	{
		if (selectedBuilding && (App->player->CanBeCreated(stats->mineralCost, stats->gasCost, 0) || useResources == false))
		{
			App->gui->addQueueSlot(type);
			if (useResources)
			{
				App->player->SubstractMineral(stats->mineralCost);
				App->player->SubstractGas(stats->gasCost);
			}
			selectedBuilding->AddNewUnit(type, stats->buildTime, stats->psi);
		}
	}
}

bool M_EntityManager::CanBeCreated(Unit_Type type)
{
	const UnitStatsData* stats = GetUnitStats(type);
	return App->player->CanBeCreated(stats->mineralCost, stats->gasCost, stats->psi);
}

void M_EntityManager::PayUnitcosts(Unit_Type type)
{
	const UnitStatsData* stats = GetUnitStats(type);
	App->player->SubstractMineral(stats->mineralCost);
	App->player->SubstractGas(stats->gasCost);
	App->player->AddPsi(stats->psi);
}

Unit* M_EntityManager::CreateUnit(int x, int y, Unit_Type type, Player_Type playerType, Building* building)
{
	if (playerType == PLAYER)
	{
		PlayUnitSound(type, sound_ready);
	}
	else
	{
		PlayUnitSound(type, sound_ready, { (float)x, (float)y });
	}
	const UnitStatsData* stats = GetUnitStats(type);
	iPoint tile = App->pathFinding->WorldToMap(x, y);

	Unit* unit = NULL;
	if (type == KERRIGAN)
	{
		unit = new Boss(x, y, type, playerType);
		boss = (Boss*)unit;
	}
	else
	{
		unit = AddUnit(Unit(x, y, type, playerType));
	}

	unit->active = true;
	int size = (2 * stats->type + 1);
	unit->SetCollider({ 0, 0, size * 8, size * 8 });

	unit->SetPriority(currentPriority++);
	unit->Start();

	if (building)
	{
		App->gui->removeQueueSlot(building);
		if (building->hasWaypoint)
			unit->Move(building->waypointTile, ATTACK_STAND, PRIORITY_MEDIUM);
	}
	unitCount++;

	return unit;

	return NULL;
}


void M_EntityManager::StartBuildingCreation(Building_Type type)
{
	const BuildingStatsData* stats = GetBuildingStats(type);
	if (App->player->CanBeCreated(stats->mineralCost, stats->gasCost, 0))
	{
		const BuildingSpriteData* data = GetBuildingSprite(type);
		builidingCreationPlayer = PLAYER;
		buildingCreationSprite.texture = data->texture;
		buildingCreationSprite.section = { 0, 0, data->size_x, data->size_y };
		buildingCreationSprite.useCamera = true;
		buildingCreationSprite.layer = GUI_MAX_LAYERS;
		buildingCreationSprite.y_ref = App->pathFinding->width * App->pathFinding->tile_width;
		buildingCreationType = type;
		UpdateCreationSprite();
	}
}

Building* M_EntityManager::CreateBuilding(int x, int y, Building_Type type, Player_Type player, bool force)
{
	const BuildingStatsData* stats = GetBuildingStats(type);
	Building* building = NULL;
	if (force || stats->race != PROTOSS || HasPower(x + stats->width_tiles / 2, y + stats->height_tiles / 2, type))
	{
		if (IsBuildingCreationWalkable(x, y, type))
		{
			building = AddBuilding(Building(x, y, type, player));

			building->active = true;

			App->player->SubstractMineral(stats->mineralCost);
			App->player->SubstractGas(stats->gasCost);

			if (type == ASSIMILATOR)
			{
				Resource* gas = FindRes(x, y, GAS);
				if (gas)
				{
					building->gasResource = gas;
				}
			}
			building->Start();

			return building;
		}
		return NULL;
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

void M_EntityManager::FinishBuildingSpawn(Building* building)
{
	uint* buildingQuantity = GetBuildingQuantity(building->GetType());

	(*buildingQuantity)++;

	if (*buildingQuantity == 1)
	{
		App->gui->setButtonStateOnBuildingType(building->GetType(), true);
	}
}


void M_EntityManager::UpdateCreationSprite()
{
	const BuildingSpriteData* buildingSprite = GetBuildingSprite(buildingCreationType);
	const BuildingStatsData* buildingStats = GetBuildingStats(buildingCreationType);

	logicTile.x = (currentTile_x / 2) * 2;
	logicTile.y = (currentTile_y / 2) * 2;
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

	if (buildingStats->race == PROTOSS)
	{
		if (buildingWalkable = HasPower(logicTile.x + buildingStats->width_tiles / 2, logicTile.y + buildingStats->height_tiles / 2, buildingCreationType))
		{
			buildingWalkable = IsBuildingCreationWalkable(logicTile.x, logicTile.y, buildingCreationType);
		}
		else
		{
			iPoint pos = App->pathFinding->MapToWorld(logicTile.x, logicTile.y);
			SDL_Rect rect = { pos.x, pos.y, buildingStats->width_tiles * 16, buildingStats->height_tiles * 16 };
			App->render->AddRect(rect, true, 255, 0, 0, 100, true);
		}
	}
	else
	{
		buildingWalkable = IsBuildingCreationWalkable(logicTile.x, logicTile.y, buildingCreationType);
	}

	createBuilding = true;
}
void M_EntityManager::UpdatePower(int startX, int startY, bool activate)
{
	//Center tiles
	for (int y = startY - 2; y <= startY + 4; y++)
	{
		for (int x = startX - 14; x <= startX + 17; x++)
		{
			ChangePowerTile(x, y, activate);
		}
	}
	//2 each side
	for (int y = startY - 4; y <= startY + 6; y++)
	{
		for (int x = startX - 13; x <= startX + 16; x++)
		{
			ChangePowerTile(x, y, activate);
		}
		if (y == startY - 3)
			y = startY + 5;
	}
	//1 each side
	int y = startY - 5;
	for (int x = startX - 12; x <= startX + 15; x++)
	{
		ChangePowerTile(x, y, activate);
	}
	y = startY + 7;
	for (int x = startX - 12; x <= startX + 15; x++)
	{
		ChangePowerTile(x, y, activate);
	}

	//1 each side
	y = startY - 6;
	for (int x = startX - 10; x <= startX + 13; x++)
	{
		ChangePowerTile(x, y, activate);
	}
	y = startY + 8;
	for (int x = startX - 10; x <= startX + 13; x++)
	{
		ChangePowerTile(x, y, activate);
	}

	//1 each side
	y = startY - 7;
	for (int x = startX - 8; x <= startX + 11; x++)
	{
		ChangePowerTile(x, y, activate);
	}
	y = startY + 9;
	for (int x = startX - 8; x <= startX + 11; x++)
	{
		ChangePowerTile(x, y, activate);
	}

	//1 each side, last
	y = startY - 8;
	for (int x = startX - 5; x <= startX + 8; x++)
	{
		ChangePowerTile(x, y, activate);
	}
	y = startY + 10;
	for (int x = startX - 5; x <= startX + 8; x++)
	{
		ChangePowerTile(x, y, activate);
	}
}

void M_EntityManager::ChangePowerTile(int tileX, int tileY, bool activate)
{
	if (activate)
		powerTiles[tileY * App->pathFinding->width + tileX] ++;
	else
	{
		powerTiles[tileY * App->pathFinding->width + tileX] --;
		if (powerTiles[tileY * App->pathFinding->width + tileX] < 0)
			powerTiles[tileY * App->pathFinding->width + tileX] = 0;
	}
}

bool M_EntityManager::HasPower(int tileX, int tileY, Building_Type type)
{
	if (type == NEXUS || type == ASSIMILATOR || type == PYLON || type == ZERG_SAMPLE)
		return true;
	return(powerTiles[tileY * App->pathFinding->width + tileX] > 0);
}

bool M_EntityManager::IsBuildingCreationWalkable(int x, int y, Building_Type type)
{
	bool ret = true;

	if (type != ASSIMILATOR)
	{
		const BuildingStatsData* buildingStats = GetBuildingStats(type);
		for (int h = 0; h < buildingStats->height_tiles; h++)
		{
			for (int w = 0; w < buildingStats->width_tiles; w++)
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
		Resource* gas = FindRes(x, y, GAS);
		if (gas && !gas->ocupied)
		{
			ret = true;
		}
		else
		{
			iPoint pos = App->pathFinding->MapToWorld(x, y);
			const BuildingStatsData* buildingStats = GetBuildingStats(type);
			SDL_Rect rect = { pos.x, pos.y, buildingStats->width_tiles * 8, buildingStats->height_tiles * 8};
			App->render->AddRect(rect, true, 0, 250, 0, 100, true);
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
			if (!App->pathFinding->IsWalkable(x + w, y + h))
			{
				ret = false;
			}
		}
	}
	return ret;
}

bool M_EntityManager::deleteUnit(std::list<Unit*>::iterator it)
{
	/*
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
	*/

	return true;
}

bool M_EntityManager::deleteBuilding(std::list<Building*>::iterator it)
{
	/*
	if (selectedBuilding == *it)
	selectedBuilding = NULL;
	(*it)->Destroy();
	buildingList.remove(*it);
	*/
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
		if (groupRect.h > 300 || groupRect.w > 300)
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
			if ((*it)->stats.player != unit->stats.player || (*it)->stats.type == GODMODE)
			{
				if (!(!(*it)->stats.canAttackFlying && unit->GetMovementType() == FLYING))
				{
					(*it)->SetAttack(unit);
				}
			}
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
		{
			if ((*it)->stats.player != building->stats.player || (*it)->stats.type == GODMODE)
			{
				(*it)->SetAttack(building);
			}
		}
		else
		{
			//Move units to building
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


void M_EntityManager::PlayUnitSound(Unit_Type type, soundTypes action, fPoint position)
{
	if (muteUnitsSounds == false)
	{
		if ((position.x >= App->render->camera.x / App->events->GetScale() && position.x <= (App->render->camera.x + App->events->GetScreenSize().x) / App->events->GetScale() &&
			position.y >= App->render->camera.y / App->events->GetScale() && position.y <= (App->render->camera.y + App->events->GetScreenSize().y) / App->events->GetScale())
			|| (position.x == 0 && position.y == 0))
		{
			std::vector<UnitSounds>::iterator it = unitsSoundsLibrary.begin();
			while (it != unitsSoundsLibrary.end())
			{
				if (it->typeOfUnit == type)
				{
					it->PlayFX(action);
					break;
				}
				it++;
			}
		}
	}

}

void M_EntityManager::PlayBuildingSound(Building_Type type, E_Race race, soundTypes action, fPoint position)
{
	if (muteUnitsSounds == false)
	{
		if ((position.x >= App->render->camera.x / App->events->GetScale() && position.x <= (App->render->camera.x + App->events->GetScreenSize().x) / App->events->GetScale() &&
			position.y >= App->render->camera.y / App->events->GetScale() && position.y <= (App->render->camera.y + App->events->GetScreenSize().y) / App->events->GetScale())
			|| (position.x == 0 && position.y == 0))
		{
			if (action == sound_ready)
			{
				if (race == PROTOSS)
				{
					if (ProtossBuildingSpawn != 0)
					{
						App->audio->PlayFx(ProtossBuildingSpawn);
					}
				}
			}
			else if (action == sound_death)
			{
				if (race == PROTOSS)
				{
					if (ProtossBuildingDeath != 0)
					{
						App->audio->PlayFx(ProtossBuildingDeath);
					}
				}
				else
				{
					if (ZergBuildingDeath != 0)
					{
						App->audio->PlayFx(ZergBuildingDeath);
					}
				}
			}
			else
			{
				std::vector<BuildingSounds>::iterator it = buildingsSoundsLibrary.begin();
				while (it != buildingsSoundsLibrary.end())
				{
					if (it->typeOfBuilding == type)
					{
						it->PlayFX(action);
						break;
					}
					it++;
				}
			}
		}
	}
}


Building* M_EntityManager::FindClosestNexus(Unit* unit)
{
	Building* ret = NULL;
	int dst = 4000;
	for (int i = 0; i < buildingList.size(); i++)
	{
		Building* building = &buildingList[i];
		if (!building->dead && building->GetType() == NEXUS && building->state != BS_DEAD && building->state != BS_SPAWNING)
		{
			iPoint worldPos = App->pathFinding->MapToWorld(building->GetPosition().x, building->GetPosition().y);
			int newDst = abs(worldPos.x - unit->GetPosition().x) + abs(worldPos.y - unit->GetPosition().y);
			if (newDst < dst)
			{
				dst = newDst;
				ret = building;
			}
		}
	}
	return ret;
}

Resource* M_EntityManager::FindClosestResource(Unit* unit)
{
	Resource* ret = NULL;
	std::list<Resource*>::iterator it = resourceList.begin();
	int dst = 4000;
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

Resource* M_EntityManager::FindRes(int x, int y, Resource_Type type)
{
	Resource* ret = NULL;
	std::list<Resource*>::iterator it = resourceList.begin();

	while (it != resourceList.end())
	{
		if ((*it)->GetType() == type && (*it)->GetPosition().x == x && (*it)->GetPosition().y == y)
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
uint* M_EntityManager::GetBuildingQuantity(Building_Type type)
{
	return buildingsLibrary.GetBuildingQuantity(type);
}

void M_EntityManager::RemoveBuildingCount(Building_Type type)
{
	uint* quantity = App->entityManager->GetBuildingQuantity(type);
	(*quantity)--;

	if (*quantity == 0)
	{
		App->gui->setButtonStateOnBuildingType(type, false);
	}
}

const HPBarData* M_EntityManager::GetHPBarSprite(int type) const
{
	return &HPBars[type];
}

//Call for this function every time the unit state changes (starts moving, starts idle, etc)
void M_EntityManager::UpdateCurrentFrame(Unit* unit)
{
	const UnitSpriteData* data = unit->spriteData;
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

void M_EntityManager::MoveSelectedUnits(int x, int y)
{
	if (selectedUnits.empty() == false)
	{
		PlayUnitSound(selectedUnits.front()->stats.type, sound_acnkowledgement);
	}
	if (hoveringResource)
	{
		if (hoveringResource->GetType() == MINERAL && hoveringResource->resourceAmount)
			SendToGather(hoveringResource);
	}
	else if (hoveringBuilding)
	{
		if (hoveringBuilding->GetType() == ASSIMILATOR && hoveringBuilding->state != BS_SPAWNING && hoveringBuilding->state != BS_DEAD)
			SendToGather(hoveringBuilding);
		else
			SendToAttack(hoveringBuilding);
	}
	else if (hoveringUnit)
	{
		SendToAttack(hoveringUnit);
	}
	else
	{
		iPoint tile = App->pathFinding->WorldToMap(x, y);
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
		else if (tmp == "Scout")
			unitsLibrary.types.push_back(SCOUT);
		else if (tmp == "Reaver")
			unitsLibrary.types.push_back(REAVER);
		else if (tmp == "Zealot")
			unitsLibrary.types.push_back(ZEALOT);
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
		else if (tmp == "R")
			unitsLibrary.types.push_back(GODMODE);
		else if (tmp == "Scout_Cin")
			unitsLibrary.types.push_back(SCOUT_CIN);
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

			C_String tmpPath = "sounds/";
			C_String tmp2 = node.child("race").attribute("value").as_string();
			if (tmp2 == "Protoss")
			{
				stats.race = PROTOSS;
				tmpPath += "protoss/units/";
			}
			else
			{
				tmpPath += "zerg/units/";
				stats.race = ZERG;
			}

			if (node.child("flying").attribute("value").as_bool())
			{
				stats.movementType = FLYING;
			}
			else
			{
				stats.movementType = GROUND;
			}


			//Loading sounds for this unit

			tmpPath += stats.name;
			UnitSounds sounds;
			sounds.typeOfUnit = unitsLibrary.types.back();
			sounds.LoadSoundsFrom(tmpPath.GetString());
			unitsSoundsLibrary.push_back(sounds);

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

	ProtossBuildingSpawn = App->audio->LoadFx("sounds/protoss/units/buildingSpawn.ogg");
	ProtossBuildingDeath = App->audio->LoadFx("sounds/protoss/units/buildingDeath.ogg");
	ZergBuildingDeath = App->audio->LoadFx("sounds/zerg/units/buildingDeath.ogg");

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
		else if (tmp == "Photon Cannon")
			buildingsLibrary.types.push_back(PHOTON_CANNON);
		else if (tmp == "Stargate")
			buildingsLibrary.types.push_back(STARGATE);
		else if (tmp == "Robotics Facility")
			buildingsLibrary.types.push_back(ROBOTICS_FACILITY);
		else if (tmp == "Citadel of Adun")
			buildingsLibrary.types.push_back(CITADEL);
		else if (tmp == "Templar Archives")
			buildingsLibrary.types.push_back(TEMPLAR_ARCHIVES);
		else if (tmp == "Robotics Support Bay")
			buildingsLibrary.types.push_back(ROBOTICS_BAY);
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
		else if (tmp == "Hive")
			buildingsLibrary.types.push_back(HIVE);
		else if (tmp == "Sunken Colony")
			buildingsLibrary.types.push_back(SUNKEN_COLONY);
		else if (tmp == "Spore Colony")
			buildingsLibrary.types.push_back(SPORE_COLONY);
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

			C_String soundPath = "sounds/";
			C_String tmp2 = node.child("race").attribute("value").as_string();
			if (tmp2 == "Protoss")
			{
				soundPath += "protoss/units/";
				stats.race = PROTOSS;
			}
			else
			{
				soundPath += "zerg/units/";
				stats.race = ZERG;
			}

			pugi::xml_node combat = node.child("combat");
			if (combat)
			{
				stats.damage = combat.child("vs_medium").attribute("value").as_int();
				stats.attackRange = node.child("attack_range").attribute("value").as_int();
			}
			else
			{
				stats.damage = 0;
				stats.attackRange = 0;
			}

			soundPath += stats.name;

			BuildingSounds sounds;
			sounds.LoadSoundsFrom(soundPath.GetString());
			sounds.typeOfBuilding = buildingsLibrary.types.back();
			buildingsSoundsLibrary.push_back(sounds);


			buildingsLibrary.stats.push_back(stats);
			buildingsLibrary.buildingQuantities.push_back(0);
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

			sprite.birth = App->tex->Load(node.child("birth").child("file").attribute("name").as_string());
			if (sprite.birth)
			{
				sprite.birthNFrames = node.child("birth").child("nframes").attribute("value").as_int();
				sprite.birthDuration = node.child("birth").child("duration").attribute("value").as_float();
				sprite.birthSize.x = node.child("birth").child("size_x").attribute("value").as_int();
				sprite.birthSize.y = node.child("birth").child("size_y").attribute("value").as_int();
			}
			else
			{
				sprite.birthNFrames = sprite.birthDuration = sprite.birthSize.x = sprite.birthSize.y = 0;
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

			sprite.anim_column_start = node.child("animation_column_start").attribute("value").as_int();
			sprite.anim_column_end = node.child("animation_column_end").attribute("value").as_int();
			sprite.animSpeed = node.child("animation_speed").attribute("value").as_int();

			sprite.HPBar_type = node.child("HPBar_type").attribute("value").as_int();

			sprite.shadow.texture = App->tex->Load(node.child("shadow").child("file").attribute("name").as_string());
			sprite.shadow.size_x = node.child("shadow").child("size_x").attribute("value").as_int();
			sprite.shadow.size_y = node.child("shadow").child("size_y").attribute("value").as_int();
			sprite.shadow.offset_x = node.child("shadow").child("offset_x").attribute("value").as_int();
			sprite.shadow.offset_y = node.child("shadow").child("offset_y").attribute("value").as_int();

			sprite.shadow.column_start = node.child("shadow").child("animation_column_start").attribute("value").as_int(0);
			sprite.shadow.column_end = node.child("shadow").child("animation_column_end").attribute("value").as_int(0);
			sprite.shadow.animation_speed = node.child("shadow").child("animation_speed").attribute("value").as_int(0);

			sprite.base.texture = App->tex->Load(node.child("base").child("file").attribute("name").as_string());
			sprite.base.size_x = node.child("base").child("size_x").attribute("value").as_int();
			sprite.base.size_y = node.child("base").child("size_y").attribute("value").as_int();
			sprite.base.offset_x = node.child("base").child("offset_x").attribute("value").as_int();
			sprite.base.offset_y = node.child("base").child("offset_y").attribute("value").as_int();

			buildingsLibrary.sprites.push_back(sprite);
		}
	}


	//Loading fire animations
	fire1.sprite.texture = App->tex->Load("graphics/neutral/building burnc blue.png");
	fire2.sprite.texture = App->tex->Load("graphics/neutral/building burnf blue.png");
	fire3.sprite.texture = App->tex->Load("graphics/neutral/building burnv blue.png");

	fire3.sprite.section.w = fire3.rect_size_x = fire2.sprite.section.w = fire2.rect_size_x = fire1.sprite.section.w = fire1.rect_size_x = 64;
	fire3.sprite.section.h = fire3.rect_size_y = fire2.sprite.section.h = fire2.rect_size_y = fire1.sprite.section.h = fire1.rect_size_y = 96;
	fire3.sprite.useCamera = fire2.sprite.useCamera = fire1.sprite.useCamera = true;
	fire3.animSpeed = fire2.animSpeed = fire1.animSpeed = 10.0f;
	fire3.type = fire2.type = fire1.type = A_RIGHT;
	fire3.firstRect = fire2.firstRect = fire1.firstRect = 0;
	fire3.lastRect = fire2.lastRect = fire1.lastRect = 10;

	blood1.sprite.texture = App->tex->Load("graphics/zerg/units/building blood1.png");
	blood2.sprite.texture = App->tex->Load("graphics/zerg/units/building blood2.png");
	blood3.sprite.texture = App->tex->Load("graphics/zerg/units/building blood3.png");

	blood3.sprite.section.w = blood3.rect_size_x = blood2.sprite.section.w = blood2.rect_size_x = blood1.sprite.section.w = blood1.rect_size_x = 64;
	blood3.sprite.section.h = blood3.rect_size_y = blood2.sprite.section.h = blood2.rect_size_y = blood1.sprite.section.h = blood1.rect_size_y = 96;
	blood3.sprite.useCamera = blood2.sprite.useCamera = blood1.sprite.useCamera = true;
	blood3.animSpeed = blood2.animSpeed = blood1.animSpeed = 10.0f;
	blood3.type = blood2.type = blood1.type = A_DOWN;
	blood3.firstRect = blood2.firstRect = blood1.firstRect = 0;
	blood3.lastRect = blood2.lastRect = blood1.lastRect = 11;

	building_spawn_animation.sprite.texture = App->tex->Load("graphics/protoss/units/warp_in.png");
	building_spawn_animation.sprite.section.w = building_spawn_animation.rect_size_x = 51;
	building_spawn_animation.sprite.section.h = building_spawn_animation.rect_size_y = 51;
	building_spawn_animation.animSpeed = 30.0f;
	building_spawn_animation.type = A_RIGHT;
	building_spawn_animation.firstRect = 0;
	building_spawn_animation.lastRect = 5;

	protoss_rubble_s.sprite.texture = App->tex->Load("graphics/protoss/units/rubble small.png");
	protoss_rubble_l.sprite.texture = App->tex->Load("graphics/protoss/units/rubble large.png");
	zerg_rubble_s.sprite.texture = App->tex->Load("graphics/zerg/units/rubble small.png");
	zerg_rubble_l.sprite.texture = App->tex->Load("graphics/zerg/units/rubble large.png");

	zerg_rubble_s.sprite.section.w = zerg_rubble_s.rect_size_x = protoss_rubble_s.sprite.section.w = protoss_rubble_s.rect_size_x = 96;
	zerg_rubble_s.sprite.section.h = zerg_rubble_s.rect_size_y = protoss_rubble_s.sprite.section.h = protoss_rubble_s.rect_size_y = 96;
	zerg_rubble_l.sprite.section.w = zerg_rubble_l.rect_size_x = protoss_rubble_l.sprite.section.w = protoss_rubble_l.rect_size_x = 128;
	zerg_rubble_l.sprite.section.h = zerg_rubble_l.rect_size_y = protoss_rubble_l.sprite.section.h = protoss_rubble_l.rect_size_y = 128;
	zerg_rubble_l.animSpeed = protoss_rubble_l.animSpeed = zerg_rubble_s.animSpeed = protoss_rubble_s.animSpeed = (float)4 / (float)60;
	zerg_rubble_l.type = protoss_rubble_l.type = zerg_rubble_s.type = protoss_rubble_s.type = A_DOWN;
	zerg_rubble_l.loopable = protoss_rubble_l.loopable = zerg_rubble_s.loopable = protoss_rubble_s.loopable = false;
	zerg_rubble_l.firstRect = protoss_rubble_l.firstRect = zerg_rubble_s.firstRect = protoss_rubble_s.firstRect = 0;
	zerg_rubble_l.lastRect = protoss_rubble_l.lastRect = zerg_rubble_l.lastRect = protoss_rubble_s.lastRect = 3;
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

void M_EntityManager::SpawnBuildings()
{
	for (uint i = 0; i < buildingList.size(); i++)
	{
		Building* building = &buildingList[i];
		if (building->state == BS_SPAWNING)
		{
			building->FinishSpawn();
		}
	}
}

void M_EntityManager::SpawnUnits()
{
	if (selectedBuilding)
	{
		selectedBuilding->SpawnUnits();
	}
}

Unit* M_EntityManager::AddUnit(Unit& unit)
{
	int i = 0;
	bool found = false;
	for (i = 0; i < unitList.size(); i++)
	{
		if (unitList[i].dead)
		{
			found = true;
			unitList[i] = Unit(unit);
			break;
		}
	}
	if (found)
		return &unitList[i];
	return NULL;
}

Building* M_EntityManager::AddBuilding(Building& building)
{
	int i = 0;
	bool found = false;
	for (i = 0; i < buildingList.size(); i++)
	{
		if (buildingList[i].dead)
		{
			found = true;
			buildingList[i] = Building(building);
			break;
		}
	}
	if (found)
		return &buildingList[i];
	return NULL;
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
	App->gui->UI_UnitSelect(unit);
}

void M_EntityManager::UnselectUnit(Unit* unit)
{
	unit->selected = false;
	unit->UpdateBarState();
	App->gui->UI_UnitUnselect(unit);
	selectedUnits.remove(unit);
}

void M_EntityManager::SelectBuilding(Building* building)
{
	PlayBuildingSound(building->GetType());
	building->selected = true;
	building->UpdateBarState();
	selectedBuilding = building;
	App->gui->setProductionQueue(building);
	App->gui->SetCurrentGrid(building);
}

void M_EntityManager::UnselectBuilding(Building* building)
{
	building->selected = false;
	building->UpdateBarState();
	App->gui->setProductionQueue(NULL);
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
	selectedEnemyUnit = NULL;

	if (!selectedUnits.empty())
	{
		App->gui->UI_Unselect();
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

	UnselectAllUnits();

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
		PlayUnitSound(hoveringUnit->stats.type, sound_selected);
		if (hoveringUnit->stats.player == COMPUTER)
		{
			selectedEnemyUnit = hoveringUnit;
			App->gui->SetCurrentGrid(G_NONE);

		}
		else
		{
			App->gui->SetCurrentGrid(hoveringUnit, false);
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
		App->gui->SetCurrentGrid(G_NONE);
	}

}

#pragma endregion

void M_EntityManager::DrawDebug()
{
	if (!selectedUnits.empty())
		App->render->AddDebugRect(groupRect, true, 255, 0, 0, 255, false);

	App->render->AddDebugRect(destinationRect, true, 255, 255, 0, 255, false);
}

#pragma region Commands

void M_EntityManager::C_SpawnBuildings::function(const C_DynArray<C_String>* arg)
{
	App->entityManager->SpawnBuildings();
}

void M_EntityManager::C_SpawnUnits::function(const C_DynArray<C_String>* arg)
{
	App->entityManager->SpawnUnits();
}
#pragma endregion

/*
void  M_EntityManager::addOrder(Order& nOrder, UI_Button2* nButt)
{
if (nButt != NULL)
nOrder.SetButton(*nButt);
orders.push_back(&nOrder);
}
*/
