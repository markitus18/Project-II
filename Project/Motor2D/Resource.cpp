#include <stdlib.h>

#include "Entity.h"
#include "Resource.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_EntityManager.h"
#include "UI_Element.h"
#include "M_PathFinding.h"
#include "M_Input.h"
#include "S_SceneMap.h"

Resource::Resource() : Entity()
{

}

Resource::Resource(int x, int y, Resource_Type _type)
{
	position.x = x;
	position.y = y;
	type = _type;
	LoadLibraryData();
	ChangeTileWalkability(false);
}



Resource::~Resource()
{

}

bool Resource::Start()
{

	return true;
}

bool Resource::Update(float dt)
{
	bool ret = true;

	if (resourceAmount <= 0)
	{
		active = false;
		ret = false;
	}
	CheckMouseHover();
	Draw();

	return ret;
}

void Resource::Destroy()
{
	ChangeTileWalkability(true);
}

Resource_Type Resource::GetType()
{
	return type;
}

float Resource::Extract(float amount)
{
	float ret = amount;
	App->render->AddRect(collider, true, 255, 255, 255);
	if (resourceAmount < amount)
	{
		amount = ret = resourceAmount;
	}
	resourceAmount -= amount;
	LOG("Resource amount: %i", (int)resourceAmount);
	return ret;
}

void Resource::ChangeTileWalkability(bool walkable)
{
	for (int h = position.y; h < position.y + height_tiles; h++)
	{
		for (int w = position.x; w < position.x + width_tiles; w++)
		{
			App->pathFinding->ChangeWalkability(w, h, walkable);
		}
	}
}

void Resource::CheckMouseHover()
{
	int x = 0, y = 0;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);

	if (mousePos.x > collider.x && mousePos.x < collider.x + collider.w &&
		mousePos.y > collider.y && mousePos.y < collider.y + collider.h)
	{
		App->entityManager->SetResourceHover(this);
	}
	else if (App->entityManager->hoveringResource == this)
	{
		App->entityManager->hoveringResource = NULL;
	}
}
void Resource::LoadLibraryData()
{
	iPoint pos = App->pathFinding->MapToWorld(position.x, position.y);

	//Loading all stats data
	const ResourceStats* statsData = App->entityManager->GetResourceStats(type);
	resourceAmount = resourceMaxAmount = statsData->maxAmount;
	width_tiles = statsData->width_tiles;
	height_tiles = statsData->height_tiles;

	//Loading all sprites data
	const ResourceSprite* spriteData = App->entityManager->GetResourceSprite(type);
	sprite.section = sprite.position = { 0, 0, 0, 0 };
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
	shadow.section.w = spriteData->shadow_size_x;
	shadow.section.h = spriteData->shadow_size_y;
	shadow.position.x = pos.x - spriteData->shadow_offset_x;
	shadow.position.y = pos.y - spriteData->shadow_offset_y;
	shadow.tint = { 0, 0, 0, 130 };

	collider.x = pos.x;
	collider.y = pos.y;
	collider.w = statsData->width_tiles * 16;
	collider.h = statsData->height_tiles * 16;

	//Base data
	base.texture = App->tex->Load("graphics/ui/o146.png");
	base.section = { 0, 0, 148, 148 };
	base.position = { pos.x - 8, pos.y - 40, 0, 0 };
	base.useCamera = true;
	base.y_ref = position.y - 2;
}

void Resource::Draw()
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
		App->render->AddSprite(&shadow, SCENE);
	}
	//Should be independent from scene
	if (App->entityManager->debug)
	{
		DrawDebug();
	}

}

void Resource::DrawDebug()
{
	SDL_Rect rect = collider;
	App->render->AddDebugRect(rect, true, 0, 255, 0, 255, false);
}