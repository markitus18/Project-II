#include <stdlib.h>

#include "Building.h"
#include "Entity.h"
#include "Controlled.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_EntityManager.h"
#include "UI_Element.h"
#include "M_PathFinding.h"

#include "S_SceneMap.h"

Building::Building() :Controlled()
{

}
Building::Building(int x, int y, Building_Type _type)
{
	position.x = x;
	position.y = y;
	type = _type;
}


Building::~Building()
{}

bool Building::Start()
{
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
	sprite.tint = { 255, 255, 255, 130 };

	iPoint pos = App->pathFinding->MapToWorld(position.x, position.y);
	sprite.position.x = pos.x - spriteData->offset_x;
	sprite.position.y = pos.y - spriteData->offset_y;
	collider.x = pos.x;
	collider.y = pos.y;
	collider.w = spriteData->size_x;
	collider.h = spriteData->size_y;

	ChangeTileWalkability();
	UpdateBarPosition();
	return true;
}

void Building::SetType(Building_Type _type)
{
	type = _type;
}

bool Building::Update(float dt)
{
	bool ret = true;

	Draw();

	return ret;
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

void Building::ChangeTileWalkability()
{
	//"2" value will be width and height building values
	for (int h = position.y; h < position.y + 4 * height_tiles; h++)
	{
		for (int w = position.x; w < position.x + 4 * width_tiles; w++)
		{
			App->pathFinding->ChangeWalkability(w, h, false);
		}
	}
}

void Building::Draw()
{
	SDL_Rect rect = { 0, 0, 64, 64 };

	if (App->sceneMap->renderBuildings)
	{
		iPoint pos = App->pathFinding->MapToWorld(position.x, position.y);
		//if (selected)
		//	App->render->Blit(App->entityManager->building_base, (int)round(pos.x), (int)round(pos.y), true, NULL);
			App->render->AddSprite(&sprite, SCENE);
	}

	//Should be independent from scene
	if (App->sceneMap->renderForces)
		DrawDebug();
}

void Building::DrawDebug()
{
	SDL_Rect rect = collider;
	App->render->AddRect(rect, true, 0, 255, 0, 255, false);
}