#include <stdlib.h>

#include "Building.h"
#include "Entity.h"
#include "Controlled.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_Map.h"
#include "M_EntityManager.h"
#include "UI_Element.h"

#include "S_SceneMap.h"

Building::Building() :Controlled()
{

}
Building::Building(int x, int y)
{
	position.x = x;
	position.y = y;
}
Building::Building(fPoint pos)
{
	position = pos;
}

Building::~Building()
{}

bool Building::Start()
{
	const BuildingSprite* spriteData = App->entityManager->GetBuildingSprite(type);
	sprite.texture = spriteData->texture;
	sprite.section = { 0, 0, spriteData->size, spriteData->size };
	sprite.y_ref = position.y;
	sprite.useCamera = true;
	iPoint pos = App->map->MapToWorld(position.x, position.y);
	sprite.tint = { 255, 0, 0, 0 };
	sprite.position = { pos.x, pos.y};
	collider.x = pos.x;
	collider.y = pos.y;
	collider.w = collider.h = spriteData->size;
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
	iPoint pos = App->map->MapToWorld(position.x, position.y);
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
void Building::Draw()
{
	SDL_Rect rect = { 0, 0, 64, 64 };

	if (App->sceneMap->renderBuildings)
	{
		iPoint pos = App->map->MapToWorld(position.x, position.y);
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