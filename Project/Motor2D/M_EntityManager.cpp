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

const UnitSprite* UnitSpritesData::GetData(Unit_Type type) const
{
	int i;
	for (i = 0; i < unitType.size(); i++)
	{
		if (unitType[i] == type)
			break;
	}
	return &data[i];
}

void UnitSpritesData::GetStateLimits(Unit_Type type, Unit_State state, int& min, int& max)
{
	const UnitSprite* data = GetData(type);
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

const BuildingSprite* BuildingSpritesData::GetData(Building_Type type) const
{
	int i;
	for (i = 0; i < buildingType.size(); i++)
	{
		if (buildingType[i] == type)
			break;
	}
	return &data[i];
}

M_EntityManager::M_EntityManager(bool start_enabled) : j1Module(start_enabled)
{

}

M_EntityManager::~M_EntityManager()
{

}

bool M_EntityManager::Start()
{
	LoadUnitSpritesData();
	LoadBuildingSpritesData();

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
		logicTile.x = (App->sceneMap->currentTile_x / 4) * 4;
		logicTile.y = (App->sceneMap->currentTile_y / 4) * 4;

		iPoint p = App->pathFinding->MapToWorld(logicTile.x, logicTile.y);

		buildingCreationSprite.position.x = p.x;
		buildingCreationSprite.position.y = p.y;

		buildingTile.position.x = p.x;
		buildingTile.position.y = p.y;

		buildingTileN.position.x = p.x;
		buildingTileN.position.y = p.y;

		App->render->AddSprite(&buildingCreationSprite, SCENE);
		if (buildingWalkable)
		{
			App->render->AddSprite(&buildingTile, SCENE);
		}
		else
		{
			App->render->AddSprite(&buildingTileN, SCENE);
		}


		buildingWalkable = true;
		//First two loops to iterate graphic tiles. "2" value should be building size
		for (int h = 0; h < 2; h++)
		{
			for (int w = 0; w < 2; w++)
			{
				//Now we iterate logic tiles
				for (int h2 = 0; h2 < 4; h2++)
				{
					for (int w2 = 0; w2 < 4; w2++)
					{
						if (!App->pathFinding->IsWalkable(logicTile.x + w2, logicTile.y + h2))
						{
							buildingWalkable = false;
						}
						//Now we iterate logic tiles
					}
				}
			}
		}
	}

	if (selectUnits)
	{
		selectUnits = false;
		selectionRect.w = selectionRect.h = 0;
	}

	if (App->sceneMap->renderForces)
		DrawDebug();

	if (selectionRect.w != 0 || selectionRect.h != 0)
		App->render->AddRect(selectionRect, false, 0, 255, 0, 255, false);

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
void M_EntityManager::StartBuildingCreation(Building_Type type)
{
	const BuildingSprite* data = GetBuildingSprite(type);
	buildingCreationSprite.texture = data->texture;
	buildingCreationSprite.section = { 0, 0, data->size, data->size };
	buildingCreationSprite.useCamera = true;
	buildingCreationSprite.layer = GUI_MAX_LAYERS;
	buildingCreationType = type;
	createBuilding = true;
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

Building* M_EntityManager::CreateBuilding(int x, int y, Building_Type type)
{
	iPoint tile = App->pathFinding->WorldToMap(x, y);
	if (App->pathFinding->IsWalkable(tile.x, tile.y))
	{
		Building* building = new Building(x, y);
		building->SetType(type);

		building->Start();

		AddBuilding(building);
		return building;
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

bool M_EntityManager::IsEntitySelected(Entity* entity) const
{
	SDL_Rect itemRect = entity->GetCollider();
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

SDL_Texture* M_EntityManager::GetTexture(Unit_Type type)
{
	return unitSpritesData.GetData(type)->texture;
}

SDL_Texture* M_EntityManager::GetTexture(Building_Type type)
{
	return buildingSpritesData.GetData(type)->texture;
}

const UnitSprite* M_EntityManager::GetUnitSprite(Unit_Type type) const
{
	return unitSpritesData.GetData(type);
}
const BuildingSprite* M_EntityManager::GetBuildingSprite(Building_Type type) const
{
	return buildingSpritesData.GetData(type);
}

void M_EntityManager::UpdateSpriteRect(Unit* unit, SDL_Rect& rect, SDL_RendererFlip& flip, float dt)
{
	//Rectangle definition variables
	int direction, size, rectX, rectY;
	const UnitSprite* unitData = unitSpritesData.GetData(unit->GetType());

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
	unitSpritesData.GetStateLimits(unit->GetType(), unit->GetState(), min, max);

	unit->currentFrame += unitData->animationSpeed * dt;
	if (unit->currentFrame >= max + 1)
		unit->currentFrame = min;

	rectY = (int)unit->currentFrame * unitData->size;
	rect = { rectX, rectY, unitData->size, unitData->size };
}

//Call for this function every time the unit state changes (starts moving, starts idle, etc)
void M_EntityManager::UpdateCurrentFrame(Unit* unit)
{
	const UnitSprite* data = unitSpritesData.GetData(unit->GetType());
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

bool M_EntityManager::LoadUnitSpritesData()
{
	bool ret = true;
	char* buf;
	int size = App->fs->Load("entityManager/Sprite data.xml", &buf);
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
			unitSpritesData.unitType.push_back(CARRIER);
		else if (tmp == "observer")
			unitSpritesData.unitType.push_back(OBSERVER);
		else if (tmp == "probe")
			unitSpritesData.unitType.push_back(PROBE);
		else if (tmp == "sapper")
			unitSpritesData.unitType.push_back(SAPPER);
		else if (tmp == "shuttle")
			unitSpritesData.unitType.push_back(SHUTTLE);
		else if (tmp == "arbiter")
			unitSpritesData.unitType.push_back(ARBITER);
		else if (tmp == "intercep")
			unitSpritesData.unitType.push_back(INTERCEP);
		else if (tmp == "scout")
			unitSpritesData.unitType.push_back(SCOUT);
		else if (tmp == "reaver")
			unitSpritesData.unitType.push_back(REAVER);
		else if (tmp == "zealot")
			unitSpritesData.unitType.push_back(ZEALOT);
		else if (tmp == "archon_t")
			unitSpritesData.unitType.push_back(ARCHON_T);
		else if (tmp == "high_templar")
			unitSpritesData.unitType.push_back(HIGH_TEMPLAR);
		else if (tmp == "dark_templar")
			unitSpritesData.unitType.push_back(DARK_TEMPLAR);
		else if (tmp == "dragoon")
			unitSpritesData.unitType.push_back(DRAGOON);

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

		unitSpritesData.data.push_back(sprite);
	}

	return ret;
}
bool M_EntityManager::LoadBuildingSpritesData()
{
	buildingSpritesData.buildingType.push_back(PYLON);

	BuildingSprite sprite;
	sprite.texture = App->tex->Load("graphics/protoss/units/pylon.png");
	sprite.size = 64;
	
	buildingSpritesData.data.push_back(sprite);

	return true;
}

void M_EntityManager::DrawDebug()
{
	if (!selectedUnits.empty())
		App->render->AddRect(groupRect, true, 255, 0, 0, 255, false);

	App->render->AddRect(destinationRect, true, 255, 255, 0, 255, false);
}