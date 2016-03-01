#include "S_SceneMap.h"

#include "j1App.h"
#include "M_Input.h"
#include "M_Textures.h"
//#include "M_Audio.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_Map.h"
#include "M_PathFinding.h"
#include "M_GUI.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Unit.h"
//#include "j1Gui.h"
//#include "UIElements.h"
//#include "M_Fonts.h"
#include "M_Console.h"

S_SceneMap::S_SceneMap(bool start_enabled) : j1Module(start_enabled)
{
	name.create("scene_map");
}

// Destructor
S_SceneMap::~S_SceneMap()
{}

// Called before render is available
bool S_SceneMap::Awake(pugi::xml_node& node)
{

	LOG("Loading Scene");
	bool ret = true;

	App->SetCurrentScene(this);

	App->console->AddCommand(&c_SaveGame);
	App->console->AddCommand(&c_LoadGame);

	return ret;
}

// Called before the first frame
bool S_SceneMap::Start()
{
	pugi::xml_node config = App->GetConfig("scene");
	App->GetConfig("scene");

	App->map->Load("sc-jungle.tmx");

	//LoadGUI();

	debug_tex = App->tex->Load("textures/current_tile.png");

	return true;
}

// Called each loop iteration
bool S_SceneMap::PreUpdate()
{
	//Getting current tile
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	currentTile_x = p.x;
	currentTile_y = p.y;

	return true;
}

// Called each loop iteration
bool S_SceneMap::Update(float dt)
{
	ManageInput(dt);

	App->map->Draw();

	if (path.Count() > 0)
	{
		for (int i = 0; i < path.Count(); i++)
		{
			iPoint p1 = App->map->MapToWorld(path[i].point.x, path[i].point.y);
			SDL_Rect pos1 = { p1.x, p1.y, 8, 8 };
			SDL_Rect rect1 = { 0, 0, 64, 64 };
			App->render->Blit(debug_tex, &pos1, true, &rect1);
		}
	}
	//Render current tile
	iPoint p = App->map->MapToWorld(currentTile_x, currentTile_y);
	SDL_Rect pos = { p.x, p.y, 8, 8 };
	SDL_Rect rect = { 0, 0, 64, 64 };
	App->render->Blit(debug_tex, &pos, true, &rect);

	if (App->pathFinding->pathStarted)
	{
		for (uint i = 0; i < App->pathFinding->openList.count(); i++)
		{
			iPoint position = App->map->MapToWorld(App->pathFinding->openList[i]->tile.x, App->pathFinding->openList[i]->tile.y);
			App->render->Blit(debug_tex, position.x, position.y, true, new SDL_Rect{ 0, 0, 64, 64 });
		}
		for (uint i = 0; i < App->pathFinding->closedList.count(); i++)
		{
			iPoint position = App->map->MapToWorld(App->pathFinding->closedList[i]->tile.x, App->pathFinding->closedList[i]->tile.y);
			App->render->Blit(debug_tex, position.x, position.y, true, new SDL_Rect{ 0, 64, 64, 64 });
		}
	}

	//Drawing Start and End tiles
	iPoint startPosition = App->map->MapToWorld(App->pathFinding->startTile.x, App->pathFinding->startTile.y);
	iPoint endPosition = App->map->MapToWorld(App->pathFinding->endTile.x, App->pathFinding->endTile.y);
	if (App->pathFinding->startTileExists)
	{
		SDL_Rect pos = { startPosition.x, startPosition.y, 8, 8 };
		SDL_Rect rect = { 0, 0, 64, 64 };
		App->render->Blit(debug_tex, &pos, true, &rect);

	}
	if (App->pathFinding->endTileExists)
	{
		SDL_Rect pos = { endPosition.x, endPosition.y, 8, 8 };
		SDL_Rect rect = { 0, 0, 64, 64 };
		App->render->Blit(debug_tex, &pos, true, &rect);

	}
	return true;
}

// Called each loop iteration
bool S_SceneMap::PostUpdate()
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool S_SceneMap::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void S_SceneMap::ManageInput(float dt)
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

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
			App->gui_D->debug = !App->gui_D->debug;

		if (App->render->camera.x > 0)
			App->render->camera.x = 0;

		if (App->render->camera.y > 0)
			App->render->camera.y = 0;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			first++;

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			first--;

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
		
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			path.Clear();
			App->pathFinding->GetNewPath(App->pathFinding->startTile, App->pathFinding->endTile, path);
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_DOWN)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			iPoint p = App->render->ScreenToWorld(x, y);
			p = App->map->WorldToMap(p.x, p.y);
			p = App->map->MapToWorld(p.x, p.y);
			App->entityManager->CreateUnit(p.x, p.y, RED);
		}
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			iPoint p = App->render->ScreenToWorld(x, y);
			App->entityManager->SendNewPath(p.x, p.y);
		}
	}
}

void S_SceneMap::LoadGUI()
{
	UI_Label* lab = App->gui_D->CreateUI_Label({ 100, 100, 0, 0 }, "Hello");
	lab->SetColor(255, 0, 255);

	UI_ProgressBar* pro = App->gui_D->CreateUI_ProgressBar({ 250, 250, 600, 20 }, lab->GetTexture(),&second,  &first);

	UI_InputText* inp = App->gui_D->CreateUI_InputText(350, 350, "Hello! :D it's me", { 0, 0, 200, 200 }, 10, 10);
	inp->AddListener(this);
}

void S_SceneMap::OnGUI(GUI_EVENTS event, UI_Element* element)
{

}

void::S_SceneMap::C_SaveGame::function(const C_DynArray<C_String>* arg)
{
	App->SaveGame("save_game.xml");
}

void::S_SceneMap::C_LoadGame::function(const C_DynArray<C_String>* arg)
{
	App->LoadGame("save_game.xml");
}

#pragma endregion