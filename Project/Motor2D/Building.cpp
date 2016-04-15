#include <stdlib.h>

#include "Building.h"
#include "Entity.h"
#include "Controlled.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_EntityManager.h"
#include "UI_Element.h"
#include "M_PathFinding.h"
#include "Unit.h"
#include "Resource.h"
#include "M_Input.h"

Building::Building() :Controlled()
{

}
Building::Building(int x, int y, Building_Type _type) : Controlled()
{
	position.x = x;
	position.y = y;
	type = _type;
	LoadLibraryData();
	ChangeTileWalkability(false);
	UpdateBarPosition();
}



Building::~Building()
{

}

bool Building::Start()
{
	return true;
}

bool Building::Update(float dt)
{
	bool ret = true;

	if (type == ASSIMILATOR)
	{
		if (gatheringUnit)
		{
			int time = gatheringTimer.ReadSec();
			if (time >= 1)
			{
				gatheringUnit->SetActive(true);
				if (gasResource->Extract(8) == 8)
					gatheringUnit->ExitAssimilator(true);
				else
				{
					gatheringUnit->ExitAssimilator(false);
					gasResource->Extract(2);
				}
				gatheringUnit->gatheredType = GAS;
				gatheringUnit = NULL;
			}
		}
	}

	if (state == BS_DEAD)
	{
		if (logicTimer.ReadSec() > TIME_TO_ERASE_BUILDING)
		{
			ret = false;
		}
	}

	if (state != BS_DEAD)
	{
		CheckMouseHover();
		Draw();
	}


	return ret;
}

Building_Type Building::GetType() const
{
	return type;
}

void Building::UpdateBarPosition()
{
	
	iPoint pos = App->pathFinding->MapToWorld(position.x, position.y);
	//HPBar_Empty->localPosition = { pos.x + collider.w / 2 - HPBar->size_x / 2, pos.y + collider.h + 10, 0, 0 };
	//HPBar_Filled->localPosition = { pos.x + collider.w / 2 - HPBar->size_x / 2, pos.y + collider.h + 10, 0, 0 };

	if (movementType == FLYING)
	{
		//HPBar_Empty->localPosition.y -= 20;
		//HPBar_Filled->localPosition.y -= 20;
	}
	
	HPBar_Empty->UpdateSprite();
	HPBar_Filled->UpdateSprite();
	
}

void Building::ChangeTileWalkability(bool walkable)
{
	for (int h = position.y; h < position.y + height_tiles; h++)
	{
		for (int w = position.x; w < position.x + width_tiles; w++)
		{
			App->pathFinding->ChangeWalkability(w, h, walkable);
		}
	}
}

void Building::AskToEnter(Unit* unit)
{
	if (gatheringUnit == NULL)
	{
		gatheringUnit = unit;
		unit->SetActive(false);
		gatheringTimer.Start();
	}
}

void Building::CheckMouseHover()
{
	int x = 0, y = 0;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);

	if (mousePos.x > collider.x && mousePos.x < collider.x + collider.w &&
		mousePos.y > collider.y && mousePos.y < collider.y + collider.h)
	{
		App->entityManager->SetBuildingHover(this);
	}
	else if (App->entityManager->hoveringBuilding == this)
	{
		App->entityManager->hoveringBuilding = NULL;
	}
}

bool Building::Hit(int amount)
{
	//App->render->AddRect(collider, true, 255, 255, 255);
	currHP -= amount;
	if (state != BS_DEAD)
	{
		UpdateBarTexture();
	}
	if (currHP <= 0 && state != BS_DEAD)
	{
		StartDeath();
		return false;
	}
	return currHP;
}

void Building::CreateUnit(Unit_Type type)
{
	iPoint pos = FindCloseWalkableTile();
	iPoint dst = App->pathFinding->MapToWorld(pos.x, pos.y);
	App->entityManager->CreateUnit(dst.x, dst.y, type, PLAYER, this);
}

iPoint Building::FindCloseWalkableTile()
{
	iPoint tile = { (int)position.x - 1, (int)position.y - 1 };
	bool maxL = false, maxD = false, maxR = false, maxU = false;
	while (!App->pathFinding->IsWalkable(tile.x, tile.y))
	{
		if (!maxL)
		{
			tile.x++;
			if (tile.x == position.x + width_tiles + 1)
				maxL = true;
		}
		else if (!maxD)
		{
			tile.y++;
			if (tile.y == position.y + height_tiles + 1)
				maxD = true;
		}
		else if (!maxR)
		{
			tile.x--;
			if (tile.x == position.x - 1)
				maxR = true;
		}
		else if (!maxU)
		{
			tile.y--;
			if (tile.y == position.y - 1)
				maxU = true;
		}
		else
			return tile;
	}
	return tile;
}

void Building::StartDeath()
{
	state = BS_DEAD;
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
	if (App->entityManager->selectedBuilding == this)
	{
		App->entityManager->selectedBuilding = NULL;
	}
	if (type != ASSIMILATOR)
		ChangeTileWalkability(true);
	else
		gasResource->active = true;

	logicTimer.Start();
}

void Building::Destroy()
{

}

void Building::LoadLibraryData()
{
	iPoint pos = App->pathFinding->MapToWorld(position.x, position.y);

	//Loading all stats data
	const BuildingStatsData* statsData = App->entityManager->GetBuildingStats(type);
	maxHP = currHP = statsData->HP;
	shield = statsData->shield;
	armor = statsData->armor;
	width_tiles = statsData->width_tiles;
	height_tiles = statsData->height_tiles;
	buildTime = statsData->buildTime;
	psi = statsData->psi;

	//Loading all sprites data
	const BuildingSpriteData* spriteData = App->entityManager->GetBuildingSprite(type);
	sprite.texture = spriteData->texture;
	sprite.section.w = spriteData->size_x;
	sprite.section.h = spriteData->size_y;
	sprite.y_ref = pos.y + (statsData->height_tiles - 1) * 8;
	sprite.useCamera = true;
	sprite.position.x = pos.x - spriteData->offset_x;
	sprite.position.y = pos.y - spriteData->offset_y;

	//Loading shadow data
	shadow.texture = spriteData->shadow.texture;
	shadow.section = shadow.position = { 0, 0, 0, 0 };
	shadow.position.x = pos.x - spriteData->shadow.offset_x;
	shadow.position.y = pos.y - spriteData->shadow.offset_y;
	shadow.y_ref = position.y - 1;
	shadow.tint = { 0, 0, 0, 130 };

	//Collider stats
	collider.x = pos.x;
	collider.y = pos.y;
	collider.w = statsData->width_tiles * 16;
	collider.h = statsData->height_tiles * 16;

	//Base data
	base.texture = spriteData->base.texture;
	base.section = { 0, 0, spriteData->base.size_x, spriteData->base.size_y };
	base.position = { pos.x - spriteData->base.offset_x, pos.y - spriteData->base.offset_y, 0, 0 };
	base.useCamera = true;
	base.y_ref = position.y - 2;
	base.tint = { 0, 200, 0, 255 };

	//HP Bar
	HPBar_type = spriteData->HPBar_type;
	const HPBarData* HPBar = App->entityManager->GetHPBarSprite(HPBar_type - 1);
	HPBar_Empty = App->gui->CreateUI_Image({ pos.x + collider.w / 2 - HPBar->size_x / 2, pos.y + collider.h + 10, 0, 0 }, HPBar->empty, { 0, 0, HPBar->size_x, HPBar->size_y });
	HPBar_Filled = App->gui->CreateUI_ProgressBar({pos.x + collider.w / 2 - HPBar->size_x / 2, pos.y + collider.h + 10, 0, 0 }, HPBar->fill, &maxHP, &currHP, { 0, 0, HPBar->size_x, HPBar->size_y });
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
	HPBar_Empty->sprite.useCamera = HPBar_Filled->sprite.useCamera = true;
}

void Building::Draw()
{
	SDL_Rect rect = { 0, 0, 64, 64 };

	if (App->entityManager->render)
	{
		if (selected)
			App->render->AddSprite(&base, SCENE);
		App->render->AddSprite(&sprite, SCENE);
	}
	if (App->entityManager->shadows)
	{
		if (shadow.texture)
		{
			App->render->AddSprite(&shadow, SCENE);
		}
	}
	//Should be independent from scene
	if (App->entityManager->debug)
	{
		DrawDebug();
	}
	if (hasWaypoint && selected)
	{
		iPoint wayPointWorld = App->pathFinding->MapToWorld(waypointTile.x, waypointTile.y);
		SDL_Rect rect = { wayPointWorld.x, wayPointWorld.y, 16, 16 };
		App->render->AddRect(rect, true, 0, 255, 0, 150);
	}
}

void Building::DrawDebug()
{
	App->render->AddDebugRect(collider, true, 0, 255, 0, 255, false);
}

iPoint Building::GetWorldPosition()
{
	return App->pathFinding->MapToWorld(position.x, position.y);
}