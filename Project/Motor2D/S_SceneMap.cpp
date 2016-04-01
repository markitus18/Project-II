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
#include "Resource.h"

//#include "j1Gui.h"
//#include "UIElements.h"
//#include "M_Fonts.h"
#include "M_Console.h"
#include "M_GUI.h"
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
	//UI WEIRD STUFF ------------------------------------
	//It is not weird >///<
		controlPT = App->tex->Load("gui/pconsole.png");
		orderIconsT = App->tex->Load("gui/cmdicons.png");
		atlasT = App->tex->Load("gui/pcmdbtns.png");
		uiIconsT = App->tex->Load("gui/icons.png");

		numUnit = 0;

		LoadGUI();

	//---------------------------------------------------

	App->pathFinding->LoadWalkableMap("maps/sc-jungle.tmx");



	debug_tex = App->tex->Load("gui/current_tile.png");
	
	currentTileSprite.texture = App->tex->Load("gui/current_tile.png");;
	currentTileSprite.section = { 0, 0, 64, 64 };
	currentTileSprite.position = { 0, 0, 8, 8 };
	currentTileSprite.useCamera = true;
	currentTileSprite.layer = GUI_MAX_LAYERS;

	App->map->Load("starcraftMap.tmx");

	App->input->UnFreezeInput();

	App->entityManager->CreateBuilding(16, 12, PYLON);
	App->entityManager->CreateBuilding(96, 48, PYLON);
	App->entityManager->CreateBuilding(52, 12, NEXUS);

	App->entityManager->CreateResource(56, 32, GAS);
	App->entityManager->CreateResource(32, 32, MINERAL);
	App->entityManager->CreateResource(22, 24, MINERAL);
	App->entityManager->CreateResource(36, 24, MINERAL);


	screenMouse = App->gui->CreateUI_Label(SDL_Rect{ 10, 10, 0, 0 }, "0");
	globalMouse = App->gui->CreateUI_Label(SDL_Rect{ 10, 30, 0, 0 }, "0");

	screenMouse->SetActive(App->entityManager->debug);
	globalMouse->SetActive(App->entityManager->debug);


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

	if (App->entityManager->debug)
	{
		App->pathFinding->Draw();

		labelUpdateTimer += (1.0f * dt);
		if (labelUpdateTimer > 0.1f)
		{
			labelUpdateTimer = 0.0f;
			int x, y;
			App->input->GetMousePosition(x, y);
			screenMouse->SetText(C_String("Screen: %i, %i", x, y));
			globalMouse->SetText(C_String("World: %i, %i", (x + App->render->camera.x), (y + App->render->camera.y)));
		}
	}

	//Render current tile
	iPoint p = App->pathFinding->MapToWorld(currentTile_x, currentTile_y);
	currentTileSprite.position.x = p.x;
	currentTileSprite.position.y = p.y;
	App->render->AddSprite(&currentTileSprite, GUI);


	//UI WEIRD STUFF -------------------------------------
		//Update resource display
		sprintf_s(it_res_c, 7, "%d", player.mineral);
		res_lab[0]->SetText(it_res_c);

		sprintf_s(it_res_c, 7, "%d", player.gas);
		res_lab[1]->SetText(it_res_c);

		sprintf_s(it_res_c, 9, "%d/%d", player.psi, player.maxPsi);
		res_lab[2]->SetText(it_res_c);
		
	//---------------------------------------------------
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


		//Enable / Disable map render
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
		{
			if (App->entityManager->debug)
			{
				labelUpdateTimer = 0.0f;
				screenMouse->SetActive(true);
				globalMouse->SetActive(true);
			}
			else
			{
				screenMouse->SetActive(false);
				globalMouse->SetActive(false);
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			App->render->camera.y -= (int)floor(CAMERA_SPEED * dt);

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			App->render->camera.y += (int)floor(CAMERA_SPEED * dt);

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			App->render->camera.x -= (int)floor(CAMERA_SPEED * dt);

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			App->render->camera.x += (int)floor(CAMERA_SPEED * dt);

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
			App->gui->debug = !App->gui->debug;
	}

	if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
	{
		numUnit--;
	}
	else if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
	{
		numUnit++;
	}
	//UI WEIRD STUFF -----------------------------------------------------
		//Change Grids
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			changeCurrentGrid(grids[0]);
		}
		else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			changeCurrentGrid(grids[1]);
		}

		if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		{
			App->entityManager->StartBuildingCreation(ASSIMILATOR);
		}

		if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		{
			App->entityManager->StartBuildingCreation(GATEWAY);
		}

		if (numUnit < 0)
		{
			numUnit = 13;
		}
		if (numUnit > 13)
		{
			numUnit = 0;
		}

	//---------------------------------------------------------------------
		CAP(App->render->camera.x, 0, 2592);
		CAP(App->render->camera.y, 0, 2592);

}

void S_SceneMap::UnitCreationInput()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->pathFinding->WorldToMap(p.x, p.y);
		p = App->pathFinding->MapToWorld(p.x, p.y);
		Unit_Type type = static_cast<Unit_Type>(numUnit);
//		Unit_Type type = static_cast<Unit_Type>(rand()%14);
		unit = App->entityManager->CreateUnit(p.x + 4, p.y + 4, PROBE);
	}
}

void S_SceneMap::LoadGUI()
{
	//UI WEIRD STUFF----------------------------------
	//Minerals Image
	res_img[0] = App->gui->CreateUI_Image({ 436, 3, 0, 0 }, uiIconsT, { 0, 0, 14, 14 });

	res_img[1] = App->gui->CreateUI_Image({ 504, 3, 0, 0 }, uiIconsT, { 0, 42, 14, 14 });

	res_img[2] = App->gui->CreateUI_Image({ 572, 3, 0, 0 }, uiIconsT, { 0, 84, 14, 14 });

	res_lab[0] = App->gui->CreateUI_Label({ 452, 4, 0, 0 }, "0");

	res_lab[1] = App->gui->CreateUI_Label({ 520, 4, 0, 0 }, "0");

	res_lab[2] = App->gui->CreateUI_Label({ 588, 4, 0, 0 }, "0");

#pragma region Grids
	loaded = true;
	coords = new Grid_Coords;

	// Inserting the control Panel Image
	controlPanel = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, controlPT, { 0, 0, 640, 480 });

	//Image iterator
	UI_Image* image_it = NULL;
	UI_Button2* butt_it = NULL;

	//Makes the code cleaner
	M_EntityManager *ptr = App->entityManager;
	M_GUI* gui = App->gui;

	//Grid 3x3 definition

	//Button Rect Measueres
	SDL_Rect idle{ 1, 0, 33, 34 };
	SDL_Rect clicked{ 74, 1, 33, 34 };

	//Initialize Grid 3x3 frame
	coords->frame->SetActive(true);

	//Nexus
	Grid3x3* nexus = new Grid3x3(*coords);
	grids.push_back(nexus);
	//------------
	butt_it = nexus->setOrder(ptr->o_GenProbe_toss, idle, clicked, 0, 0, *atlasT);

	image_it = gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, SDL_Rect{ 468, 102, 32, 32 });
	image_it->SetParent(nexus->buttons[0]);

	butt_it->son = image_it;

	//------------
	butt_it = nexus->setOrder(ptr->o_Set_rallyPoint, idle, clicked, 1, 2, *atlasT);

	image_it = gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, orderIconsT, { 504, 544, 32, 32 });
	image_it->SetParent(nexus->buttons[1]);

	butt_it->son = image_it;


	nexus->changeState(false);

	//Basic Unit
	Grid3x3* basic_u = new Grid3x3(*coords);

	grids.push_back(basic_u);
	currentGrid = basic_u;

	butt_it = basic_u->setOrder(ptr->o_Move, idle, clicked, 0, 0, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 252, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[0]);

	butt_it->son = image_it;


	//------------
	butt_it = basic_u->setOrder(ptr->o_Stop, idle, clicked, 0, 1, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 288, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[1]);

	butt_it->son = image_it;


	//------------
	butt_it = basic_u->setOrder(ptr->o_Attack, idle, clicked, 0, 2, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 324, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[2]);

	butt_it->son = image_it;


	//------------
	basic_u->setOrder(ptr->o_Patrol, idle, clicked, 1, 0, *atlasT, true);
	//TODO: change image rect, used now for testing
	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 36, 304, 32, 32 });
	image_it->SetParent(basic_u->buttons[3]);

	butt_it->son = image_it;


	//------------
	basic_u->setOrder(ptr->o_Hold_pos, idle, clicked, 1, 1, *atlasT, true);
	//TODO: change image rect, used now for testing
	image_it = gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 108, 304, 32, 32 });
	image_it->SetParent(basic_u->buttons[4]);

	butt_it->son = image_it;

#pragma endregion
	//----------------------------------------------------------
}

bool S_SceneMap::changeCurrentGrid(Grid3x3 * newCurrent)
{
	if (currentGrid == newCurrent)
	return false;

	if (currentGrid != NULL)
	{
		currentGrid->changeState(false);
	}
	if (newCurrent != NULL)
	{
		newCurrent->changeState(true);
	}
	currentGrid = newCurrent;

	return true;
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