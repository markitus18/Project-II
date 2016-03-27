#include "M_EntityManager.h"

#include "j1App.h"
#include "M_Textures.h"
#include "Unit.h"
#include "Building.h"
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

void UnitsLibrary::GetStateLimits(Unit_Type type, Unit_State state, int& min, int& max)
{
	const UnitSprite* data = GetSprite(type);
	switch (state)
	{
	case (IDLE) :
	{
		min = data->idle_line_start;
		max = data->idle_line_end;
		break;
	}
	case (MOVE) :
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

const BuildingStats* BuildingsLibrary::GetStats(Building_Type _type) const
{
	int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i] == _type)
			break;
	}
	return &stats[i];
}

const BuildingSprite* BuildingsLibrary::GetSprite(Building_Type _type) const
{
	int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i] == _type)
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

	return true;
}


bool M_EntityManager::Update(float dt)
{
	ManageInput();

	DoUnitLoop(dt);
	DoBuildingLoop(dt);
	UpdateSelectionRect();

	if (createBuilding)
	{
		UpdateCreationSprite();
	}
	if (selectUnits)
	{
		selectUnits = false;
		selectionRect.w = selectionRect.h = 0;
	}

	if (App->sceneMap->renderForces)
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
		if (selectUnits)
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
		it++;
	}
}

void M_EntityManager::DoBuildingLoop(float dt)
{
	std::list<Building*>::iterator it = buildingList.begin();
	bool buildingSelected = false;
	while (it != buildingList.end())
	{
		if (selectUnits)
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
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		selectUnits = true;
	}

}


Unit* M_EntityManager::CreateUnit(int x, int y, Unit_Type type)
{
	iPoint tile = App->pathFinding->WorldToMap(x, y);
	if ( App->pathFinding->IsWalkable(tile.x, tile.y))
	{
		Unit* unit = new Unit(x, y);
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

	if (IsBuildingCreationWalkable(type))
	{
		Building* building = new Building(x, y, type);

		building->Start();

		AddBuilding(building);
		return building;
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

	buildingWalkable = IsBuildingCreationWalkable(buildingCreationType);

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

bool M_EntityManager::IsBuildingCreationWalkable(Building_Type type)
{
	bool ret = true;
	const BuildingStats* buildingStats = GetBuildingStats(type);
	//First two loops to iterate graphic tiles. "2" value should be building size
	for (int h = 0; h < buildingStats->height_tiles; h++)
	{
		for (int w = 0; w < buildingStats->width_tiles; w++)
		{
			//Now we iterate logic tiles
			for (int h2 = 0; h2 < 4; h2++)
			{
				for (int w2 = 0; w2 < 4; w2++)
				{
					if (!App->pathFinding->IsWalkable(logicTile.x + w2 * w + w2, logicTile.y + h2 * h + h2))
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
	RELEASE (*it);


	return true;
}

bool M_EntityManager::deleteBuilding(std::list<Building*>::iterator it)
{
	(*it)->Destroy();
	if ((*it)->selected)
	{
		//selectedUnits.remove(*it);
	}
	buildingList.remove(*it);
	RELEASE(*it);

	return true;
}

bool M_EntityManager::IsEntitySelected(Entity* entity) //const
{
	SDL_Rect itemRect = entity->GetCollider();
	SDL_Rect rect = selectionRect;

	iPoint rectPos = App->render->ScreenToWorld(rect.x, rect.y);
	rect.x = rectPos.x;
	rect.y = rectPos.y;

	//App->render->DrawQuad(itemRect);
	//App->render->DrawQuad(rect);

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
			App->pathFinding->GetNewPath(unitTile, dstTile, newPath);
			(*it)->SetNewPath(newPath);

			it++;
		}
	}
}

const UnitStats* M_EntityManager::GetUnitStats(Unit_Type type) const
{
	return unitsLibrary.GetStats(type);
}

const BuildingStats* M_EntityManager::GetBuildingStats(Building_Type type) const
{
	return buildingsLibrary.GetStats(type);
}

const UnitSprite* M_EntityManager::GetUnitSprite(Unit_Type type) const
{
	return unitsLibrary.GetSprite(type);
}

const BuildingSprite* M_EntityManager::GetBuildingSprite(Building_Type type) const
{
	return buildingsLibrary.GetSprite(type);
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
	case(IDLE) :
	{
		unit->currentFrame = data->idle_line_start;
		break;
	}
	case(MOVE) :
	{
		unit->currentFrame = data->run_line_start;
		break;
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

bool M_EntityManager::LoadUnitsStats(char* path)
{
	return true;
}

bool M_EntityManager::LoadUnitsSprites(char* path)
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
		ret = false;
	}

	pugi::xml_node node;
	for (node = file.child("sprites").child("unit"); node && ret; node = node.next_sibling("unit"))
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

bool M_EntityManager::LoadBuildingsSprites(char* path)
{
	//TODO: Load from xml
	BuildingSprite sprite;
	sprite.texture = App->tex->Load("graphics/protoss/units/nexus.png");
	sprite.size_x = 192;
	sprite.size_y = 224;
	sprite.offset_x = 33;
	sprite.offset_y = 72;
	buildingsLibrary.sprites.push_back(sprite);

	BuildingSprite sprite2;
	sprite2.texture = App->tex->Load("graphics/protoss/units/pylon.png");
	sprite2.size_x = 64;
	sprite2.size_y = 64;
	sprite2.offset_x = 0;
	sprite2.offset_y = 0;
	buildingsLibrary.sprites.push_back(sprite2);

	return true;
}

void M_EntityManager::AddUnit(Unit* unit)
{
	unitList.push_back(unit);
}

void M_EntityManager::AddBuilding(Building* building)
{
	buildingList.push_back(building);
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
#pragma endregion

void M_EntityManager::DrawDebug()
{
	if (!selectedUnits.empty())
		App->render->AddRect(groupRect, true, 255, 0, 0, 255, false);

	App->render->AddRect(destinationRect, true, 255, 255, 0, 255, false);
}