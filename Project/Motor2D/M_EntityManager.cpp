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
#include "M_FileSystem.h"

const UnitSpriteData* SpritesData::GetData(Unit_Type type) const
{
	int i;
	for (i = 0; i < unitType.size(); i++)
	{
		if (unitType[i] == type)
			break;
	}
	return &data[i];
}

void SpritesData::GetStateLimits(Unit_Type type, Unit_State state, int& min, int& max)
{
	const UnitSpriteData* data = GetData(type);
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

M_EntityManager::M_EntityManager(bool start_enabled) : j1Module(start_enabled)
{

}

M_EntityManager::~M_EntityManager()
{

}

bool M_EntityManager::Start()
{
	LoadSpritesData();

	pylon_tex = App->tex->Load("graphics/protoss/units/pylon.png");
	unit_base = App->tex->Load("graphics/ui/o048.png");
	path_tex = App->tex->Load("textures/path.png");
	hpBar_empty = App->tex->Load("graphics/ui/hpbarempt.png");
	hpBar_filled = App->tex->Load("graphics/ui/hpbarfull.png");
	hpBar_mid = App->tex->Load("graphics/ui/hpbarmid.png");;
	hpBar_low = App->tex->Load("graphics/ui/hpbarlow.png");;
	building_base = App->tex->Load("graphics/ui/o110.png");
	return true;
}


bool M_EntityManager::Update(float dt)
{
	ManageInput();

	DoUnitLoop(dt);
	DoBuildingLoop(dt);
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

void M_EntityManager::DoBuildingLoop(float dt)
{
	std::list<Building*>::iterator it = buildingList.begin();
	
	while (it != buildingList.end())
	{
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
	iPoint tile = App->map->WorldToMap(x, y);
	if (App->pathFinding->IsWalkable(tile.x, tile.y))
	{
		Building* building = new Building(x, y);
		building->SetType(type);

		building->SetCollider({ 0, 0, 5 * 8, 5 * 8 });

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
	return spritesData.GetData(type)->texture;
}

SDL_Texture* M_EntityManager::GetTexture(Building_Type type)
{
	switch (type)
	{
	case (PYLON) :
		return pylon_tex;
		break;
	default:
		return NULL;
		break;
	}
}

const UnitSpriteData* M_EntityManager::GetUnitData(Unit_Type type) const
{
	return spritesData.GetData(type);
}

void M_EntityManager::UpdateSpriteRect(Unit* unit, SDL_Rect& rect, SDL_RendererFlip& flip, float dt)
{
	//Rectangle definition variables
	int direction, size, rectX, rectY;
	const UnitSpriteData* unitData = spritesData.GetData(unit->GetType());

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
	spritesData.GetStateLimits(unit->GetType(), unit->GetState(), min, max);

	unit->currentFrame += unitData->animationSpeed * dt;
	if (unit->currentFrame >= max + 1)
		unit->currentFrame = min;

	rectY = (int)unit->currentFrame * unitData->size;
	rect = { rectX, rectY, unitData->size, unitData->size };
}

//Call for this function every time the unit state changes (starts moving, starts idle, etc)
void M_EntityManager::UpdateCurrentFrame(Unit* unit)
{
	const UnitSpriteData* data = spritesData.GetData(unit->GetType());
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

bool M_EntityManager::LoadSpritesData()
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
			spritesData.unitType.push_back(CARRIER);
		else if (tmp == "observer")
			spritesData.unitType.push_back(OBSERVER);
		else if (tmp == "probe")
			spritesData.unitType.push_back(PROBE);
		else if (tmp == "sapper")
			spritesData.unitType.push_back(SAPPER);
		else if (tmp == "shuttle")
			spritesData.unitType.push_back(SHUTTLE);
		else if (tmp == "arbiter")
			spritesData.unitType.push_back(ARBITER);
		else if (tmp == "intercep")
			spritesData.unitType.push_back(INTERCEP);
		else if (tmp == "scout")
			spritesData.unitType.push_back(SCOUT);
		else if (tmp == "reaver")
			spritesData.unitType.push_back(REAVER);
		else if (tmp == "zealot")
			spritesData.unitType.push_back(ZEALOT);
		else if (tmp == "archon_t")
			spritesData.unitType.push_back(ARCHON_T);
		else if (tmp == "high_templar")
			spritesData.unitType.push_back(HIGH_TEMPLAR);
		else if (tmp == "dark_templar")
			spritesData.unitType.push_back(DARK_TEMPLAR);
		else if (tmp == "dragoon")
			spritesData.unitType.push_back(DRAGOON);

		UnitSpriteData unitData;
		unitData.texture = App->tex->Load(node.child("file").attribute("name").as_string());
		unitData.size = node.child("size").attribute("value").as_int();
		unitData.animationSpeed = node.child("animationSpeed").attribute("value").as_float();
		unitData.idle_line_start = node.child("idle_line_start").attribute("value").as_int();
		unitData.idle_line_end= node.child("idle_line_end").attribute("value").as_int();
		unitData.run_line_start = node.child("run_line_start").attribute("value").as_int();
		unitData.run_line_end = node.child("run_line_end").attribute("value").as_int();
		unitData.attack_line_start = node.child("attack_line_start").attribute("value").as_int();
		unitData.attack_line_end = node.child("attack_line_end").attribute("value").as_int();

		spritesData.data.push_back(unitData);
	}
	/*
	spritesData.unitType.push_back(ARBITER);
	spritesData.unitType.push_back(DARK_TEMPLAR);

	//Loading arbiter data
	UnitC_SpriteData arbiterData;
	arbiterData.texture = arbiter_tex;
	arbiterData.size = 76;

	arbiterData.animationSpeed = 1.0f;

	arbiterData.idle_line_start = 0;
	arbiterData.idle_line_end = 1;

	arbiterData.run_line_start = 0;
	arbiterData.run_line_end = 1;

	spritesData.data.push_back(arbiterData);

	//Loading dark templar data
	UnitC_SpriteData darkTemplarData;
	darkTemplarData.texture = darkT_tex;
	darkTemplarData.size = 64;

	darkTemplarData.animationSpeed = 0.4f;

	darkTemplarData.idle_line_start = 13;
	darkTemplarData.idle_line_end = 13;

	darkTemplarData.run_line_start = 9;
	darkTemplarData.run_line_end = 17;

	spritesData.data.push_back(darkTemplarData);
	*/
	return ret;
}


void M_EntityManager::DrawDebug()
{
	if (!selectedUnits.empty())
		App->render->DrawQuad(groupRect, true, 255, 0, 0, 255, false);

	App->render->DrawQuad(destinationRect, true, 255, 255, 0, 255, false);
}