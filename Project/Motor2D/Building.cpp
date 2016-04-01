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
					gatheringUnit->ExitAssimilator(false);
				gatheringUnit = NULL;
			}
		}
	}
	Draw();

	return ret;
}

Building_Type Building::GetType() const
{
	return type;
}

void Building::UpdateBarPosition()
{
	iPoint pos = App->pathFinding->MapToWorld(position.x, position.y);
	HPBar_Empty->localPosition.x = pos.x + collider.w / 2 - 53;
	HPBar_Empty->localPosition.y = pos.y + collider.h / 2 - 50;
	HPBar_Filled->localPosition.x = pos.x + collider.w / 2 + 2 - 53;
	HPBar_Filled->localPosition.y = pos.y + collider.h / 2 - 48;

	if (movementType == FLYING)
	{
		HPBar_Empty->localPosition.y -= 20;
		HPBar_Filled->localPosition.y -= 20;
	}

	HPBar_Empty->UpdateSprite();
	HPBar_Filled->UpdateSprite();
}

void Building::ChangeTileWalkability(bool walkable)
{
	for (int h = position.y; h < position.y + 4 * height_tiles; h++)
	{
		for (int w = position.x; w < position.x + 4 * width_tiles; w++)
		{
			App->pathFinding->ChangeWalkability(w, h, walkable);
		}
	}
}

void Building::AskToEnter(Unit* unit)
{
	if (gatheringUnit == NULL)
	{
		if (gasResource->resourceAmount)
		{
			gatheringUnit = unit;
			unit->SetActive(false);
			gatheringTimer.Start();
		}
	}
}

void Building::LoadLibraryData()
{
	iPoint pos = App->pathFinding->MapToWorld(position.x, position.y);

	//Loading all stats data
	const BuildingStats* statsData = App->entityManager->GetBuildingStats(type);
	maxHP = currHP = statsData->HP;
	shield = statsData->shield;
	armor = statsData->armor;
	cost = statsData->cost;
	//costType = statsData->costType;
	width_tiles = statsData->width_tiles;
	height_tiles = statsData->height_tiles;
	buildTime = statsData->buildTime;
	psi = statsData->psi;

	//Loading all sprites data
	const BuildingSprite* spriteData = App->entityManager->GetBuildingSprite(type);
	sprite.texture = spriteData->texture;
	sprite.section.w = spriteData->size_x;
	sprite.section.h = spriteData->size_y;
	sprite.y_ref = position.y;
	sprite.useCamera = true;
	sprite.position.x = pos.x - spriteData->offset_x;
	sprite.position.y = pos.y - spriteData->offset_y;

	//Loading shadow data
	shadow.texture = spriteData->shadow;
	shadow.section = shadow.position = { 0, 0, 0, 0 };
	shadow.position.x = pos.x - spriteData->shadow_offset_x;
	shadow.position.y = pos.y - spriteData->shadow_offset_y;
	shadow.y_ref = position.y - 1;

	//Collider stats
	shadow.tint = { 0, 0, 0, 130 };
	collider.x = pos.x;
	collider.y = pos.y;
	collider.w = statsData->width_tiles * 32;
	collider.h = statsData->height_tiles * 32;

	//Base data
	base.texture = App->tex->Load("graphics/ui/o072.png");
	base.section = { 0, 0, 80, 80 };
	base.position = { pos.x - 8, pos.y + 8, 0, 0 };
	base.useCamera = true;
	base.y_ref = position.y - 2;
	base.tint = { 0, 200, 0, 255 };
}

void Building::Draw()
{
	SDL_Rect rect = { 0, 0, 64, 64 };

	if (App->entityManager->render)
	{
		//if (selected)
		//	App->render->AddSprite(&base, SCENE);
		App->render->AddSprite(&sprite, SCENE);
	}
	if (App->entityManager->shadows)
	{
		App->render->AddSprite(&shadow, SCENE);
	}
	//Should be independent from scene
	if (App->entityManager->debug)
	{
		DrawDebug();
	}
}

void Building::DrawDebug()
{
	App->render->AddRect(collider, true, 0, 255, 0, 255, false);
}