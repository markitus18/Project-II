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
	texture = App->entityManager->GetTexture(type);
	iPoint pos = App->map->MapToWorld(position.x, position.y);
	collider.x = pos.x - collider.w / 2;
	collider.y = pos.y - collider.h / 2;
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

void Building::Draw()
{
	SDL_Rect rect = { 0, 0, 64, 64 };

	if (App->sceneMap->renderBuildings)
	{
		//if (selected)
		//	App->render->Blit(App->entityManager->building_base, (int)round(position.x - 64), (int)round(position.y) - 64, true, NULL);

		iPoint pos = App->map->MapToWorld(position.x, position.y);
		App->render->Blit(texture, pos.x - 32, pos.y - 32, true, &rect);
	}

	//Should be independent from scene
	if (App->sceneMap->renderForces)
		DrawDebug();
}

void Building::DrawDebug()
{
	
	SDL_Rect rect = collider;
	App->render->DrawQuad(rect, true, 0, 255, 0, 255, false);

	//Target position
	//	App->render->DrawCircle(target.x, target.y, 10, true, 255, 255, 255);
	//Unit position
	//	App->render->DrawCircle((int)round(position.x), (int)round(position.y), 10, true, 255, 255, 255, 255);
}