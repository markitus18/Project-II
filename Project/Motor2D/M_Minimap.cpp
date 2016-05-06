#include "M_Minimap.h"

#include "j1App.h"
#include "M_GUI.h"
#include "M_InputManager.h"
#include "M_Render.h"
#include "M_Map.h"
#include "M_FogOfWar.h"
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

	creepTex[0] = App->tex->Load("maps/creep1.png");
	creepTex[1] = App->tex->Load("maps/creep2.png");
	creepTex[2] = App->tex->Load("maps/creep3.png");
	creepTex[3] = App->tex->Load("maps/creep4.png");

	int w, h;
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;

	creep[0] = App->gui->CreateUI_Image({ 0, 0, w * (130.0f / 1280.0f), 130 }, creepTex[0], { 0, 0, 0, 0 });
	creep[0]->SetLayer(2);

	creep[1] = App->gui->CreateUI_Image({ 0, 0, w * (130.0f / 1280.0f), 130 }, creepTex[1], { 0, 0, 0, 0 });
	creep[1]->SetLayer(2);

	creep[2] = App->gui->CreateUI_Image({ 0, 0, w * (130.0f / 1280.0f), 130 }, creepTex[2], { 0, 0, 0, 0 });
	creep[2]->SetLayer(2);

	creep[3] = App->gui->CreateUI_Image({ 0, 0, w * (130.0f / 1280.0f), 130 }, creepTex[3], { 0, 0, 0, 0 });
	creep[3]->SetLayer(2);

	map = App->gui->CreateUI_Image({ w * (5.0f / 1280.0f), 45, w * (130.0f / 1280.0f), 130 }, minimap, { 0, 0, 0, 0 });
	map->collider = { -8, -8, map->localPosition.w + 16, map->localPosition.h + 16 };
	map->SetParent(App->sceneMap->controlPanel);
	map->SetLayer(1);
	map->AddListener(this);

	creep[0]->SetParent(map);
	creep[1]->SetParent(map);
	creep[2]->SetParent(map);
	creep[3]->SetParent(map);

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
	boss_white.texture = App->tex->Load("graphics/ui/boss_minimap_selected.png");
	boss_white.section = { 0, 0, 0, 0 };
	boss_white.useCamera = false;
	boss_purple.texture = App->tex->Load("graphics/ui/boss_minimap_spell.png");
	boss_purple.section = { 0, 0, 0, 0 };
	boss_purple.useCamera = false;
	bossRadius = 100;
	roughTimer = 0;

	underAttack1 = App->audio->LoadFx("sounds/protoss/units/advisor/upd00.wav");
	return true;

}

bool M_Minimap::Update(float dt)
{
	int w, h, scale;
	scale = App->events->GetScale();
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;

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
	App->tex->UnLoad(boss_white.texture);
	App->tex->UnLoad(boss_purple.texture);
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

iPoint M_Minimap::WorldToMinimap(int x, int y) const
{
	SDL_Rect mapPos = map->GetWorldPosition();

	float currentX = x / (float)(App->map->data.width * App->map->data.tile_width);
	float currentY = y / (float)(App->map->data.height * App->map->data.tile_height);

	currentX = mapPos.x + currentX * mapPos.w;
	currentY = mapPos.y + currentY * mapPos.h;

	return iPoint(currentX, currentY);
}

iPoint M_Minimap::MinimapToWorld(int x, int y) const
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

void M_Minimap::DrawUnit(Unit* unit)
{
	if (App->fogOfWar->IsVisible(unit->GetPosition().x, unit->GetPosition().y))
	{
		if (unit->active && (unit->GetMovementState() != MOVEMENT_DEAD || App->entityManager->debug))
		{
			iPoint toDraw = WorldToMinimap(unit->GetPosition().x, unit->GetPosition().y);
			if (unit->selected && (unit->stats.type != KERRIGAN))
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 255, 255, 255, 200);
			}
			else if (unit->stats.player == PLAYER)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 16, 252, 24, 200);
			}
			else if (unit->stats.player == COMPUTER && (unit->stats.type != KERRIGAN))
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 255, 0, 0, 200);
			}
			else if (unit->stats.player == CINEMATIC)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 255, 230, 10, 200);
			}
			else if (unit->stats.type == KERRIGAN)
			{
					if (bossRadius > 90)
						bossRadius -= 1;
					else
					{
						if (bossRadius > 10)
						{
							bossRadius -= 2;
							if (bossRadius < 60)
								bossRadius -= 2;
						}
					}

					if (unit->GetState() == MOVEMENT_BOSS_STUNNED)
					{
						if (roughTimer <= 50)
						{
							boss_purple.position.x = toDraw.x - (bossRadius / 2);
							boss_purple.position.y = toDraw.y - (bossRadius / 2);
							boss_purple.position.w = boss_purple.position.h = bossRadius;
							App->render->AddSprite(&boss_purple, CURSOR);
							roughTimer += 1;
						}
						else
						{
							boss.position.x = toDraw.x - (bossRadius / 2);
							boss.position.y = toDraw.y - (bossRadius / 2);
							boss.position.w = boss.position.h = bossRadius;
							App->render->AddSprite(&boss, CURSOR);
							roughTimer += 1;
							if (roughTimer >= 100)
								roughTimer = 0;
						}
					}
					else
					{
						if (!unit->selected)
						{
							boss.position.x = toDraw.x - (bossRadius / 2);
							boss.position.y = toDraw.y - (bossRadius / 2);
							boss.position.w = boss.position.h = bossRadius;
							App->render->AddSprite(&boss, CURSOR);
						}
						if (unit->selected)
						{
							boss_white.position.x = toDraw.x - (bossRadius / 2);
							boss_white.position.y = toDraw.y - (bossRadius / 2);
							boss_white.position.w = boss_white.position.h = bossRadius;
							App->render->AddSprite(&boss_white, CURSOR);
						}
					}
			}
		}
	}
}

void M_Minimap::DrawBuilding(Building* building)
{
	if (building->active && building->state != BS_DEAD)
	{
		if (App->fogOfWar->IsVisible(building->GetCollider().x, building->GetCollider().y))
		{
			iPoint toDraw = App->pathFinding->MapToWorld(building->GetPosition().x, building->GetPosition().y);
			toDraw = WorldToMinimap(toDraw.x, toDraw.y);

			int mini_x = 3;
			int mini_y = 3;

			if (building->GetType() == NEXUS || building->GetType() == GATEWAY || building->GetType() == STARGATE || building->GetType() == ZERG_SAMPLE ||
				building->GetType() == LAIR || building->GetType() == INFESTED_COMMAND_CENTER || building->GetType() == HIVE)
			{
				mini_x = 4;
				mini_y = 4;
			}
			else if (building->GetType() == ASSIMILATOR)
			{
				mini_x = 4;
			}

			if (building->selected)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, mini_x, mini_y }, false, 255, 255, 255, 200);
			}
			else if (building->stats.player == PLAYER)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, mini_x, mini_y }, false, 16, 252, 24, 200);
			}
			else if (building->stats.player == COMPUTER)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, mini_x, mini_y }, false, 255, 0, 0, 200);
			}
		}

	}
}

void M_Minimap::DrawResource(Resource* resource)
{
	if (App->fogOfWar->IsVisible(resource->GetCollider().x, resource->GetCollider().y, 0))
	{
		iPoint toDraw = WorldToMinimap(resource->GetCollider().x, resource->GetCollider().y);
		App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 3, 2 }, false, 0, 228, 252, 200);
	}
}