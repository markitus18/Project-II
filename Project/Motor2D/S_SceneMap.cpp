#include "S_SceneMap.h"

#include "j1App.h"
#include "M_Input.h"
#include "M_Textures.h"
//#include "M_Audio.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_PathFinding.h"
#include "M_GUI.h"
#include "M_EntityManager.h"
#include "Entity.h"
#include "Unit.h"
//#include "j1Gui.h"
//#include "UIElements.h"
//#include "M_Fonts.h"
#include "M_Console.h"
#include "M_GUI.h"
#include "M_Orders.h"
#include "Building.h"
#include "M_Map.h"

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
	pugi::xml_node config = App->GetConfig(name.GetString());

	App->pathFinding->LoadWalkableMap("maps/sc-jungle.tmx");

	consoleT = App->tex->Load("gui/pconsole.png");
	iconsT = App->tex->Load("gui/cmdicons.png");
	atlasT = App->tex->Load("gui/pcmdbtns.png");

	//LoadGUI();

	debug_tex = App->tex->Load("gui/current_tile.png");

	currentTileSprite.texture = debug_tex;
	currentTileSprite.section = { 0, 0, 64, 64 };
	currentTileSprite.position = { 0, 0, 8, 8 };
	currentTileSprite.useCamera = true;
	currentTileSprite.layer = GUI_MAX_LAYERS;

	App->map->Load("starcraftMap.tmx");
	//mapTexture = App->tex->Load("maps/MAP.bmp");

	App->input->UnFreezeInput();

	App->entityManager->CreateBuilding(32, 20, PYLON);
	App->entityManager->CreateBuilding(80, 32, PYLON);
	App->entityManager->CreateBuilding(96, 48, PYLON);
	App->entityManager->CreateBuilding(32, 80, PYLON);


	return true;
}

// Called each loop iteration
bool S_SceneMap::PreUpdate()
{
	//Getting current tile
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->pathFinding->WorldToMap(p.x, p.y);
	currentTile_x = p.x;
	currentTile_y = p.y;

	return true;
}

// Called each loop iteration
bool S_SceneMap::Update(float dt)
{
	ManageInput(dt);

	SDL_Rect rect1 = { 0, 0, 0, 0 };
	App->map->Draw();
	//App->render->Blit(mapTexture, &rect1, true);

	if (debugMap)
	{
		App->pathFinding->Draw();
	}

	//Render current tile
	iPoint p = App->pathFinding->MapToWorld(currentTile_x, currentTile_y);
	currentTileSprite.position.x = p.x;
	currentTileSprite.position.y = p.y;
	App->render->AddSprite(&currentTileSprite, GUI);

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
		UnitCreationInput();

		if (unit)
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
		}

		//Enable / Disable forces debug
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_UP)
		{
			renderForces = !renderForces;
		}

		//Enable / Disable map render
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
		{
			debugMap = !debugMap;
		}

		//Enable / Disable unit render
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_UP)
		{
			renderUnits = !renderUnits;
		}

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			App->render->camera.y += (int)floor(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			App->render->camera.y -= (int)floor(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			App->render->camera.x += (int)floor(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			App->render->camera.x -= (int)floor(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
			App->gui->debug = !App->gui->debug;




		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			App->entityManager->SendNewPath(currentTile_x, currentTile_y);
		}
	}

	CAP(App->render->camera.x, -2592, 0);
	CAP(App->render->camera.y, -2592, 0);

}

void S_SceneMap::UnitCreationInput()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		App->entityManager->StartBuildingCreation(NEXUS);
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		App->entityManager->StartBuildingCreation(PYLON);
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->pathFinding->WorldToMap(p.x, p.y);
		p = App->pathFinding->MapToWorld(p.x, p.y);
		Unit_Type type = static_cast<Unit_Type>(rand() % 14);
		unit = App->entityManager->CreateUnit(p.x + 4, p.y + 4, type);
	}
}

void S_SceneMap::LoadGUI()
{
	//Button Rect Measueres
	SDL_Rect idle{ 1, 0, 33, 34 };
	SDL_Rect clicked{ 74, 1, 34, 34 };

	// Inserting the console Image
	console = App->gui->CreateUI_Image({ 0, 200, 0, 0 }, consoleT, { 0, 0, 640, 480 });

	//Struct that all grids will use
	Grid_Coords coords;
	//Image iterator
	UI_Image* image_it = NULL;
	M_Orders* ptr = App->orders;
	M_GUI* gui = App->gui;

	//Nexus
	Grid3x3 nexus(coords);

	//------------
	nexus.setOrder(ptr->o_GenProbe_toss, idle, clicked, 0, 0, *atlasT);

	image_it = gui->CreateUI_Image({ 0, 0, 0, 0 }, iconsT, SDL_Rect{ 468, 102, 32, 32 });
	image_it->SetParent(nexus.buttons[0]);

	nexus.buttons[0]->AddListener((j1Module*)App->orders);

	//------------
	nexus.setOrder(ptr->o_Set_rallyPoint, idle, clicked, 1, 2, *atlasT);

	image_it = gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, iconsT, { 504, 544, 32, 32 });
	image_it->SetParent(nexus.buttons[1]);

	nexus.buttons[1]->AddListener((j1Module*)App->orders);

	nexus.changeState(false);

	//Basic Unit
	Grid3x3 basic_u(coords);
	currentGrid = &basic_u;
	basic_u.setOrder(ptr->o_Move, idle, clicked, 0, 0, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 252, 442, 32, 32 });
	image_it->SetParent(basic_u.buttons[0]);

	basic_u.buttons[0]->AddListener((j1Module*)App->orders);

	//------------
	basic_u.setOrder(ptr->o_Stop, idle, clicked, 0, 1, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 288, 442, 32, 32 });
	image_it->SetParent(basic_u.buttons[1]);

	basic_u.buttons[1]->AddListener((j1Module*)App->orders);

	//------------
	basic_u.setOrder(ptr->o_Attack, idle, clicked, 0, 2, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 324, 442, 32, 32 });
	image_it->SetParent(basic_u.buttons[2]);

	basic_u.buttons[2]->AddListener((j1Module*)App->orders);

	//------------
	basic_u.setOrder(ptr->o_Patrol, idle, clicked, 1, 0, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 576, 475, 32, 32 });
	image_it->SetParent(basic_u.buttons[3]);

	basic_u.buttons[3]->AddListener((j1Module*)App->orders);

	//------------
	basic_u.setOrder(ptr->o_Hold_pos, idle, clicked, 1, 1, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 0, 510, 32, 32 });
	image_it->SetParent(basic_u.buttons[4]);

	basic_u.buttons[4]->AddListener((j1Module*)App->orders);
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