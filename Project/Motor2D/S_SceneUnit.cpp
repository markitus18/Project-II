#include "j1App.h"
#include "M_Input.h"
#include "M_Textures.h"
//#include "M_Audio.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_Map.h"
#include "M_PathFinding.h"
#include "Unit.h"
#include "Entity.h"
#include "M_EntityManager.h"
#include "M_Fonts.h"
#include "UIElements.h"
#include "S_SceneUnit.h"

S_SceneUnit::S_SceneUnit(bool start_enabled) : j1Module(start_enabled)
{
	name.create("scene_unit");
}

// Destructor
S_SceneUnit::~S_SceneUnit()
{}

// Called before render is available
bool S_SceneUnit::Awake(pugi::xml_node& node)
{
	LOG("Loading Scene");
	App->SetCurrentScene(this);

	return true;
}

// Called before the first frame
bool S_SceneUnit::Start()
{
	App->map->Load("iso.tmx");

	debug_tex = App->tex->Load("textures/path.png");

	instructions = App->tex->Load("textures/instructions.png");
	instructions_title = App->tex->Load("textures/instructions_title.png");
	grid_tex = App->tex->Load("textures/grid.png");

	//Centering camera
	int width = App->map->data.width / 2;
	int height = App->map->data.height / 2;
	iPoint pos = App->map->MapToWorld(width, height);
	App->render->camera.x = pos.x + App->render->camera.w / 2;
	App->render->camera.y = pos.y - App->render->camera.h / 2;
	
	LoadGUI();
	return true;
}

// Called each loop iteration
bool S_SceneUnit::PreUpdate()
{
	//Getting current mouse tile
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	currentTile_x = p.x;
	currentTile_y = p.y;
	p = App->map->MapToWorld(p.x, p.y);

	SDL_Rect rect = { 0, 0, 64, 32 };
	App->render->Blit(debug_tex, p.x - 32, p.y - 16, true, &rect);

	return true;
}

// Called each loop iteration
bool S_SceneUnit::Update(float dt)
{
	ManageInput(dt);
	
	App->map->Draw();

	if (renderForces)
	{	
		//Drawing point 0, 0
		App->render->DrawCircle(0, 0, 10, true, 255, 255, 255);
		App->render->DrawLine(0, -20, 0, 20, true, 255, 0, 0);
		App->render->DrawLine(-20, 0, 20, 0, true, 255, 0, 0);
		// --- TO CHANGE: clean drawing start/end tiles and current tile ---
		//Drawing start and end
		iPoint startPosition = App->map->MapToWorld(App->pathFinding->startTile.x, App->pathFinding->startTile.y);
		iPoint endPosition = App->map->MapToWorld(App->pathFinding->endTile.x, App->pathFinding->endTile.y);
		if (App->pathFinding->startTileExists)
		{
			SDL_Rect rect = { 0, 32, 64, 32 };
			App->render->Blit(App->map->data.tilesets.start->next->data->texture, startPosition.x - 32, startPosition.y - 16, true, &rect);
		}

		if (App->pathFinding->endTileExists)
		{
			SDL_Rect rect = { 64, 32, 64, 32 };
			App->render->Blit(App->map->data.tilesets.start->next->data->texture, endPosition.x - 32, endPosition.y - 16, true, &rect);
		}

	}
	return true;
}

// Called each loop iteration
bool S_SceneUnit::PostUpdate(float dt)
{
	bool ret = true;
	
	//if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	//	ret = false;

	return ret;
}

// Called before quitting
bool S_SceneUnit::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void S_SceneUnit::ManageInput(float dt)
{
	if (App->input->GetInputState() == false)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			int hp = unit->GetHP();
			if (hp < 100)
				unit->SetHP(++hp);
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			int hp = unit->GetHP();
			if (hp > 0)
				unit->SetHP(--hp);
		}

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			App->render->camera.y += (int)(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			App->render->camera.y -= (int)(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			App->render->camera.x += (int)(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			App->render->camera.x -= (int)(200.0f * dt);

		LOG("Camera: x %i, y %i", App->render->camera.x, App->render->camera.y);
		//Enable / Disable forces debug
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_UP)
		{
			renderForces = !renderForces;
		}

		//Enable / Disable unit render
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_UP)
		{
			renderUnits = !renderUnits;
		}

		if (App->input->GetKey(SDL_SCANCODE_C) == KEY_UP)
		{
			App->render->camera.x = 512;
			App->render->camera.y = 0;
		}
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		App->entityManager->SendNewPath(p.x, p.y);
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->map->WorldToMap(p.x, p.y);
		p = App->map->MapToWorld(p.x, p.y);
		App->entityManager->CreateUnit(p.x, p.y, ARBITER);
	}

	int x, y;
	App->input->GetMousePosition(x, y);
	int w = App->render->camera.w;
	int h = App->render->camera.h;
	/*
	if (y < 20)
		App->render->camera.y += (int)(400.0f * dt);

	if (y > h - 20)
		App->render->camera.y -= (int)(400.0f * dt);

	if (x < 20)
		App->render->camera.x += (int)(400.0f * dt);

	if (x > w - 20)
		App->render->camera.x -= (int)(400.0f * dt);
	*/
	//Enable / Disable grid

	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_UP)
	{
		renderGrid = !renderGrid;
	}

}

void S_SceneUnit::LoadGUI()
{

}

void S_SceneUnit::OnGUI(GUI_EVENTS event, UI_Element* element)
{

}