#include <stdlib.h>

#include "Building.h"
#include "Entity.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_Map.h"
#include "M_EntityManager.h"
#include "UI_Element.h"

#include "S_SceneMap.h"

Building::Building() :Entity()
{

}
Building::Building(float x, float y)
{
	position = { x, y };
}
Building::Building(fPoint pos)
{
	position = pos;
}

Building::~Building()
{}

bool Building::Start()
{
	UpdateCollider();

	texture = App->entityManager->GetTexture(type);
	return true;
}

void Building::SetType(Building_Type _type)
{
	type = _type;
}

void Building::SetPriority(int _priority)
{
	priority = _priority;
}

bool Building::Update(float dt)
{
	bool ret = true;
	bool collided = false;

	Draw();

	return ret;
}

void Building::UpdateCollider()
{
	collider.x = round(position.x - collider.w / 2);
	collider.y = round(position.y - collider.h / 2);
}

void Building::Draw()
{
	SDL_Rect rect = { 0, 0, 76, 76 };
	SDL_RendererFlip flip = SDL_FLIP_NONE;

	if (App->sceneMap->renderBuildings)
	{
		if (selected)
			App->render->Blit(App->entityManager->building_base, (int)round(position.x - 64), (int)round(position.y) - 64, true, NULL);
		int positionY = (int)round(position.y - 38);

		App->render->Blit(texture, (int)round(position.x - 38), positionY, true, &rect, flip);
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