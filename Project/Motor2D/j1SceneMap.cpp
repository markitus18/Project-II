#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Gui.h"
#include "UIElements.h"
#include "j1SceneMap.h"
#include "j1Fonts.h"
#include "j1Console.h"

j1SceneMap::j1SceneMap(bool start_enabled) : j1Module(start_enabled)
{
	name.create("scene_map");
}

// Destructor
j1SceneMap::~j1SceneMap()
{}

// Called before render is available
bool j1SceneMap::Awake(pugi::xml_node& node)
{

	LOG("Loading Scene");
	bool ret = true;

	App->SetCurrentScene(this);

	App->console->AddCommand(&c_SaveGame);
	App->console->AddCommand(&c_LoadGame);

	return ret;
}

// Called before the first frame
bool j1SceneMap::Start()
{
	pugi::xml_node config = App->GetConfig("scene");
	App->GetConfig("scene");

	App->map->Load("iso.tmx");

	//LoadGUI();

	debug_tex = App->tex->Load("maps/path.png");

	return true;
}

// Called each loop iteration
bool j1SceneMap::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1SceneMap::Update(float dt)
{
	// -------
	ManageInput(dt);

	App->map->Draw();

	//Getting current mouse tile

	//Mouse position
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debug_tex, p.x, p.y, new SDL_Rect{ 0, 0, 64, 64 });

//	App->render->Blit(debug_tex, currentTile.x, currentTile.y, new SDL_Rect{ 0, 0, 64, 64 });

	if (App->pathFinding->pathFinished)
	{
		for (uint i = 0; i < App->pathFinding->path.Count(); i++)
		{
			iPoint position = App->map->MapToWorld(App->pathFinding->path[i].point.x, App->pathFinding->path[i].point.y);
			App->render->Blit(debug_tex, position.x, position.y, new SDL_Rect{ 0, 0, 64, 64 });
		}
	}

	else if (App->pathFinding->pathStarted)
	{
		for (uint i = 0; i < App->pathFinding->openList.count(); i++)
		{
			iPoint position = App->map->MapToWorld(App->pathFinding->openList[i]->tile.x, App->pathFinding->openList[i]->tile.y);
			App->render->Blit(debug_tex, position.x, position.y, new SDL_Rect{ 0, 0, 64, 64 });
		}
		for (uint i = 0; i < App->pathFinding->closedList.count(); i++)
		{
			iPoint position = App->map->MapToWorld(App->pathFinding->closedList[i]->tile.x, App->pathFinding->closedList[i]->tile.y);
			App->render->Blit(debug_tex, position.x, position.y, new SDL_Rect{ 0, 64, 64, 64 });
		}
	}

	//Drawing Start and End tiles
	iPoint startPosition = App->map->MapToWorld(App->pathFinding->startTile.x, App->pathFinding->startTile.y);
	iPoint endPosition = App->map->MapToWorld(App->pathFinding->endTile.x, App->pathFinding->endTile.y);
	if (App->pathFinding->startTileExists)
		App->render->Blit(App->map->data.tilesets.start->next->data->texture, startPosition.x, startPosition.y, new SDL_Rect{ 0, 64, 64, 64 });
	if (App->pathFinding->endTileExists)
		App->render->Blit(App->map->data.tilesets.start->next->data->texture, endPosition.x, endPosition.y, new SDL_Rect{ 64, 64, 64, 64 });

	return true;
}

// Called each loop iteration
bool j1SceneMap::PostUpdate()
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool j1SceneMap::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void j1SceneMap::ManageInput(float dt)
{
	if (App->input->GetInputState() == false)
	{
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			App->render->camera.y += (int)floor(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			App->render->camera.y -= (int)floor(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			App->render->camera.x += (int)floor(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			App->render->camera.x -= (int)floor(200.0f * dt);

		//change pahtfinding start tile
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_UP)
		{
			if (App->pathFinding->startTile.x != currentTile_x || App->pathFinding->startTile.y != currentTile_y)
			{
				LOG("-- Pathfinding: Starting tile updated");
				App->pathFinding->startTile.x = currentTile_x;
				App->pathFinding->startTile.y = currentTile_y;
				App->pathFinding->startTileExists = true;
			}
			else
			{
				if (!App->pathFinding->startTileExists)
					App->pathFinding->startTileExists = true;
				else
					App->pathFinding->startTileExists = false;
			}


		}
		//Change pathfinding end tile
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_UP)
		{
			if (App->pathFinding->endTile.x != currentTile_x || App->pathFinding->endTile.y != currentTile_y)
			{
				LOG("-- Pathfinding: End tile updated");
				App->pathFinding->endTile.x = currentTile_x;
				App->pathFinding->endTile.y = currentTile_y;
				App->pathFinding->endTileExists = true;
			}
			else
			{
				if (!App->pathFinding->endTileExists)
					App->pathFinding->endTileExists = true;
				else
					App->pathFinding->endTileExists = false;
			}
		}



	}
}

void j1SceneMap::LoadGUI()
{

}




void j1SceneMap::OnGUI(UI_Event _event, UIElement* _element)
{

}

void::j1SceneMap::C_SaveGame::function(const p2DynArray<p2SString>* arg)
{
	App->SaveGame("save_game.xml");
}

void::j1SceneMap::C_LoadGame::function(const p2DynArray<p2SString>* arg)
{
	App->LoadGame("save_game.xml");
}

#pragma endregion