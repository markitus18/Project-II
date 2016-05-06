#include "M_Minimap.h"

#include "j1App.h"
#include "M_EntityManager.h"
#include "M_GUI.h"
#include "S_SceneMap.h"
#include "M_InputManager.h"
#include "M_Render.h"
#include "M_Map.h"
#include "M_FogOfWar.h"
#include "M_EntityManager.h"
#include "Unit.h"
#include "Building.h"
#include "Resource.h"
#include "M_Player.h"
#include "M_PathFinding.h"

#include "M_Audio.h"

//This should be removed. It is used to check the "OnEvent" bool
#include "S_SceneMap.h"


M_Minimap::M_Minimap(bool start_enabled) : j1Module(start_enabled)
{
	name.create("minimap");
}

M_Minimap::~M_Minimap(){};

bool M_Minimap::Start()
{
	minimap = App->tex->Load("maps/graphic.png");

	int w, h;
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;
	map = App->gui->CreateUI_Image({ w * (5.0f / 1280.0f), 45, w * (130.0f / 1280.0f), 130 }, minimap, { 0, 0, 0, 0 });

	map->collider = { -8, -8, map->localPosition.w + 16, map->localPosition.h + 16 };
	map->SetParent(App->sceneMap->controlPanel);
	map->SetLayer(1);
	map->AddListener(this);

	pingPos = { 0, 0 };
	pingRadius = 0.0f;
	pingTimer.Start();
	ping.texture = App->tex->Load("graphics/ui/MinimapPing.png");
	ping.section = { 0, 0, 0, 0 };
	pinging = false;
	ping.useCamera = false;
	ping.y_ref = 500;
	boss.texture = App->tex->Load("graphics/ui/boss_minimap.png");
	boss.section = { 0, 0, 0, 0 };
	boss.useCamera = false;

	underAttack1 = App->audio->LoadFx("sounds/protoss/units/advisor/upd00.wav");
	return true;

}

bool M_Minimap::Update(float dt)
{
	//TMP updating UI
	int w, h, scale;
	scale = App->events->GetScale();
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;


#pragma region	//Drawing minimap Units, Buildings & Resources

	if (App->entityManager->unitList.empty() == false)
	{
		std::list<Unit*>::iterator it = App->entityManager->unitList.begin();
		while (it != App->entityManager->unitList.end())
		{
			if (App->fogOfWar->IsVisible((*it)->GetPosition().x, (*it)->GetPosition().y))
			{
				if ((*it)->active && ((*it)->GetMovementState() != MOVEMENT_DEAD || App->entityManager->debug))
				{
					iPoint toDraw = WorldToMinimap((*it)->GetPosition().x, (*it)->GetPosition().y);
					if ((*it)->selected && ((*it)->stats.type != KERRIGAN))
					{
						App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 255, 255, 255, 200);
					}
					else if ((*it)->stats.player == PLAYER)
					{
						App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 16, 252, 24, 200);
					}
					else if ((*it)->stats.player == COMPUTER && ((*it)->stats.type != KERRIGAN))
					{
						App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 255, 0, 0, 200);
					}
					else if ((*it)->stats.player == CINEMATIC)
					{
						App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 255, 230, 10, 200);
					}
					else if ((*it)->stats.type == KERRIGAN)
					{
						boss.position.x = toDraw.x - 5;
						boss.position.y = toDraw.y - 5;
						boss.position.w = boss.position.h = 10;
						App->render->AddSprite(&boss, CURSOR);
					}
				}
			}
			it++;
		}
	}

	if (App->entityManager->buildingList.empty() == false)
	{
		std::list<Building*>::iterator it2 = App->entityManager->buildingList.begin();
		while (it2 != App->entityManager->buildingList.end())
		{
			if ((*it2)->active && (*it2)->state != BS_DEAD)
			{
				if (App->fogOfWar->IsVisible((*it2)->GetCollider().x, (*it2)->GetCollider().y))
				{
					iPoint toDraw = App->pathFinding->MapToWorld((*it2)->GetPosition().x, (*it2)->GetPosition().y);
					toDraw = WorldToMinimap(toDraw.x, toDraw.y);

					int mini_x = 3;
					int mini_y = 3;

					if ((*it2)->GetType() == NEXUS || (*it2)->GetType() == GATEWAY || (*it2)->GetType() == STARGATE || (*it2)->GetType() == ZERG_SAMPLE ||
						(*it2)->GetType() == LAIR || (*it2)->GetType() == INFESTED_COMMAND_CENTER || (*it2)->GetType() == HIVE)
					{
						mini_x = 4;
						mini_y = 4;
					}
					else if ((*it2)->GetType() == ASSIMILATOR)
					{
						mini_x = 4;
					}

					if ((*it2)->selected)
					{
						App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, mini_x, mini_y }, false, 255, 255, 255, 200);
					}
					else if ((*it2)->stats.player == PLAYER)
					{
						App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, mini_x, mini_y }, false, 16, 252, 24, 200);
					}
					else if ((*it2)->stats.player == COMPUTER)
					{
						App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, mini_x, mini_y }, false, 255, 0, 0, 200);
					}
				}

			}
			it2++;
		}
	}

	if (App->entityManager->resourceList.empty() == false)
	{
		std::list<Resource*>::iterator it3 = App->entityManager->resourceList.begin();
		while (it3 != App->entityManager->resourceList.end())
		{
			if (App->fogOfWar->IsVisible((*it3)->GetCollider().x, (*it3)->GetCollider().y, 0))
			{
				iPoint toDraw = WorldToMinimap((*it3)->GetCollider().x, (*it3)->GetCollider().y);
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 3, 2 }, false, 0, 228, 252, 200);
			}
			it3++;
		}
	}


#pragma endregion

	iPoint pos = WorldToMinimap(App->render->camera.x / scale, App->render->camera.y / scale);
	App->render->AddDebugRect({ pos.x, pos.y, w * (56.0f / 1280.0f) / scale, h * (56.0f / 1280.0f) / scale }, false, 255, 255, 255, 255, false);

#pragma region	//Moving camera around

	if (App->sceneMap->onEvent == false && App->render->movingCamera == false)
	{
		if (movingMap)
		{
			iPoint pos = App->events->GetMouseOnScreen();
			pos = MinimapToWorld(pos.x, pos.y);

			App->render->camera.x = pos.x * scale - App->render->camera.w / scale;
			App->render->camera.y = pos.y * scale - App->render->camera.h / scale;
		}
	}



#pragma endregion

	if (App->events->GetEvent(E_MINIMAP_PING) == EVENT_DOWN)
	{
		PingOnWorld(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y);
	}

	if (pinging)
	{
		if (pingTimer.ReadSec() < 1)
		{
			pingRadius += 24 * dt;
		}
		else if (pingRadius > 1)
		{
			pingRadius -= 12 * dt;
		}
		else
		{
			pinging = false;
		}
		ping.position.x = pingPos.x - ceil(pingRadius);
		ping.position.y = pingPos.y - ceil(pingRadius);
		ping.position.w = ping.position.h = ceil(pingRadius) * 2;
		App->render->AddSprite(&ping, CURSOR);
	}

	return true;
}

bool M_Minimap::CleanUp()
{
	App->gui->DeleteUIElement(map);
	App->tex->UnLoad(minimap);
	App->tex->UnLoad(ping.texture);
	App->tex->UnLoad(boss.texture);
	return true;
}

void M_Minimap::OnGUI(GUI_EVENTS event, UI_Element* element)
{
	if (element == map)
	{
		if (event == UI_MOUSE_DOWN)
		{
			movingMap = true;
		}
		else if (event == UI_MOUSE_EXIT || event == UI_MOUSE_UP || event == UI_LOST_FOCUS)
		{
			movingMap = false;
		}
		if (event == UI_RIGHT_MOUSE_DOWN)
		{
			iPoint pos = MinimapToWorld(App->events->GetMouseOnScreen().x, App->events->GetMouseOnScreen().y);
			App->entityManager->MoveSelectedUnits(pos.x, pos.y);
		}
	}
}

iPoint M_Minimap::WorldToMinimap(int x, int y)
{
	SDL_Rect mapPos = map->GetWorldPosition();

	float currentX = x / (float)(App->map->data.width * App->map->data.tile_width);
	float currentY = y / (float)(App->map->data.height * App->map->data.tile_height);

	currentX = mapPos.x + currentX * mapPos.w;
	currentY = mapPos.y + currentY * mapPos.h;

	return iPoint(currentX, currentY);
}

iPoint M_Minimap::MinimapToWorld(int x, int y)
{
	SDL_Rect mapPos = map->GetWorldPosition();

	float currentX = (x - mapPos.x) / (float)mapPos.w;
	float currentY = (y - mapPos.y) / (float)mapPos.h;

	currentX = currentX * (App->map->data.width * App->map->data.tile_width);
	currentY = currentY * (App->map->data.height * App->map->data.tile_height);

	return iPoint(currentX, currentY);
}

void M_Minimap::PingOnWorld(int x, int y, bool forced)
{
	iPoint tmp = WorldToMinimap(x, y);

	PingOnMinimap(tmp.x, tmp.y, forced);
}

void M_Minimap::PingOnMinimap(int x, int y, bool forced)
{
	if (forced || pingTimer.ReadSec() > 20)
	{
		//if (rand() % 2 == 0)
		//{
			App->audio->PlayFx(underAttack1);
		//}
		//else
		//{
		//	App->audio->PlayFx(underAttack2);
		//}
		pingTimer.Start();
		pinging = true;
		pingPos = { x, y };
		pingRadius = 0.0f;
	}


}
