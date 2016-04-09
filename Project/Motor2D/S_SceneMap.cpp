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

#include "Stats panel.h"
#include <utility> //Pair, make_pair
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
		minimap = App->tex->Load("maps/graphic.png");
		uiWireframesT = App->tex->Load("gui/Wireframes.png");
		numUnit = 0;

		LoadGUI();

	//---------------------------------------------------

	App->pathFinding->LoadWalkableMap("maps/walkable.tmx");



	debug_tex = App->tex->Load("gui/current_tile.png");
	
	currentTileSprite.texture = App->tex->Load("gui/current_tile.png");;
	currentTileSprite.section = { 0, 0, 64, 64 };
	currentTileSprite.position = { 0, 0, 16, 16 };
	currentTileSprite.useCamera = true;
	currentTileSprite.layer = GUI_MAX_LAYERS;

	App->map->Load("graphic.tmx");

	App->input->UnFreezeInput();

	SpawnResources();
	SpawnStartingUnits();

	screenMouse = App->gui->CreateUI_Label(SDL_Rect{ 10, 10, 0, 0 }, "0");
	globalMouse = App->gui->CreateUI_Label(SDL_Rect{ 10, 30, 0, 0 }, "0");
	tileMouse = App->gui->CreateUI_Label(SDL_Rect{ 10, 50, 0, 0 }, "0");

	screenMouse->SetActive(App->entityManager->debug);
	globalMouse->SetActive(App->entityManager->debug);
	tileMouse->SetActive(App->entityManager->debug);

	player.gas = 3000;
	player.mineral = 3000;
	App->render->camera.x = 215;
	App->render->camera.y = 5120;

	App->gui->SetCurrentGrid(G_DEFAULT);

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

int debug = 0;
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
			iPoint tile = App->pathFinding->WorldToMap(x + App->render->camera.x, y + App->render->camera.y);
			tileMouse->SetText(C_String("Logic Tile: %i, %i", tile.x, tile.y));
		}
	}

	//Render current tile
	iPoint p = App->pathFinding->MapToWorld(currentTile_x, currentTile_y);
	currentTileSprite.position.x = p.x;
	currentTileSprite.position.y = p.y;
	App->render->AddSprite(&currentTileSprite, GUI);


	//UI WEIRD STUFF -------------------------------------
		//Update resource display
	char it_res_c [9];
	sprintf_s(it_res_c, 7, "%d", player.mineral);
	res_lab[0]->SetText(it_res_c);

	sprintf_s(it_res_c, 7, "%d", player.gas);
	res_lab[1]->SetText(it_res_c);

	sprintf_s(it_res_c, 9, "%d/%d", player.psi, player.maxPsi);
	res_lab[2]->SetText(it_res_c);

	if (App->input->GetKey(SDL_SCANCODE_H == KEY_DOWN))
	{
		statsPanel_m->setStatsWireframesMult(0,ZEALOT);
	}
	if (App->input->GetKey(SDL_SCANCODE_J == KEY_DOWN))
	{
		statsPanel_m->setStatsWireframesMult(1,DRAGOON);
	}
	if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
	{
		statsPanel_m->setStatsWireframesMult(debug, PROBE);
		++debug;
		if (debug > 11)
			debug = 0;
	}
	//---------------------------------------------------
		//Update Minimap rect
		iPoint pos = WorldToMinimap(App->render->camera.x / App->win->GetScale(), App->render->camera.y / App->win->GetScale());
		App->render->AddDebugRect({ pos.x, pos.y, 56 / App->win->GetScale(), 32 / App->win->GetScale() }, false, 255, 0, 0, 255, false);
		if (movingMap)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			iPoint pos = MinimapToWorld(x, y);
			App->render->camera.x = pos.x * App->win->GetScale();
			App->render->camera.y = pos.y * App->win->GetScale();
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
	RELEASE(statsPanel_s);
	RELEASE(statsPanel_m);
	return true;
}

void S_SceneMap::ManageInput(float dt)
{
	if (App->input->GetInputState() == false)
	{
		UnitCreationInput();

		//Enable / Disable map render
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
		{
			if (App->entityManager->debug)
			{
				labelUpdateTimer = 0.0f;
				screenMouse->SetActive(true);
				globalMouse->SetActive(true);
				tileMouse->SetActive(true);
			}
			else
			{
				screenMouse->SetActive(false);
				globalMouse->SetActive(false);
				tileMouse->SetActive(false);
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
			App->gui->debug = !App->gui->debug;

		if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
			App->pathFinding->displayPath = !App->pathFinding->displayPath;


		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			App->render->camera.y -= (int)floor(CAMERA_SPEED * dt);

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			App->render->camera.y += (int)floor(CAMERA_SPEED * dt);

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			App->render->camera.x -= (int)floor(CAMERA_SPEED * dt);

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			App->render->camera.x += (int)floor(CAMERA_SPEED * dt);

		if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
		{
			numUnit--;
		}
		else if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
		{
			numUnit++;
		}

		if (App->input->GetKey(SDL_SCANCODE_P))
		{
			App->gui->SetCurrentGrid(NULL);
		}
	}

	//UI WEIRD STUFF -----------------------------------------------------
		//Change Grids
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			App->gui->SetCurrentGrid(G_NEXUS);
		}
		else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			App->gui->SetCurrentGrid(G_BASIC_UNIT);
		}

		if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		{
			App->entityManager->StartBuildingCreation(ASSIMILATOR);
		}

		if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		{
			App->entityManager->StartBuildingCreation(GATEWAY);
		}

		if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_DOWN)
		{
			if (App->win->GetScale() > 1)
			{
				App->win->SetScale(App->win->GetScale() - 1);
				App->render->camera.x /= 2;
				App->render->camera.y /= 2;

			}
		}

		if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN)
		{
			App->win->SetScale(App->win->GetScale() + 1);
			App->render->camera.x *= 2;
			App->render->camera.y *= 2;
		}

		if (numUnit < 0)
		{
			numUnit = 11;
		}
		if (numUnit > 11)
		{
			numUnit = 0;
		}

		int x = 0, y = 0;
		App->input->GetMousePosition(x, y);
		bool movingLeft = false, movingRight = false, movingUp = false, movingDown = false;

		if (y < 5)
		{
			if (App->render->camera.y > 0)
			{
				App->render->camera.y -= (int)floor(CAMERA_SPEED * dt);
				movingUp = true;
			}
		}
		if (y > App->render->camera.h / App->win->GetScale() - 5)
		{
			if (App->render->camera.y < 2700 * App->win->GetScale())
			{
				App->render->camera.y += (int)floor(CAMERA_SPEED * dt);
				movingDown = true;
			}
		}
		if (x < 5)
		{
			if (App->render->camera.x > 0)
			{
				App->render->camera.x -= (int)floor(CAMERA_SPEED * dt);
				movingLeft = true;
			}
		}
		if (x > App->render->camera.w / App->win->GetScale() - 5)
		{
			if (App->render->camera.x < 2433 * App->win->GetScale())
			{
				App->render->camera.x += (int)floor(CAMERA_SPEED * dt);
				movingRight = true;
			}
		}
		Mouse_State newState = M_DEFAULT;
		int moveIndex = 8 * movingUp + 4 * movingDown + 2 * movingLeft + 1 * movingRight;
		switch (moveIndex)
		{
		case(1) :
			newState = M_RIGHT;
			break;
		case(2) :
			newState = M_LEFT;
			break;
		case(4) :
			newState = M_DOWN;
			break;
		case(5) :
			newState = M_RIGHT_DOWN;
			break;
		case(6) :
			newState = M_DOWN_LEFT;
			break;
		case(8) :
			newState = M_UP;
			break;
		case(9) :
			newState = M_UP_RIGHT;
			break;
		case(10) :
			newState = M_LEFT_UP;
			break;
		}

		App->entityManager->SetMouseState(newState, true);
	//---------------------------------------------------------------------
		CAP(App->render->camera.x, 0, 2433*App->win->GetScale());
		CAP(App->render->camera.y, 0, 2700 * App->win->GetScale());

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
		unit = App->entityManager->CreateUnit(p.x + 8, p.y + 8, DRAGOON, PLAYER);
	}

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->pathFinding->WorldToMap(p.x, p.y);
		p = App->pathFinding->MapToWorld(p.x, p.y);
		Unit_Type type = static_cast<Unit_Type>(numUnit);
		//		Unit_Type type = static_cast<Unit_Type>(rand()%14);
		unit = App->entityManager->CreateUnit(p.x + 8, p.y + 8, ZEALOT, COMPUTER);
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
	for (int n = 0; n <= 2; n++)
	{
		res_img[n]->SetLayer(n);
		res_lab[n]->SetLayer(n);
	}

	// Inserting the control Panel Image
	controlPanel = App->gui->CreateUI_Image({ 0, 301, 0, 0 }, controlPT, { 0, 0, 0, 0 }, { 0, 60, 640, 118 });
	controlPanel->SetLayer(1);

	map = App->gui->CreateUI_Image({ 5, 45, 130, 130 }, minimap, { 0, 0, 0, 0 });
	map->SetParent(controlPanel);
	map->SetLayer(1);
	map->AddListener(this);
	/*
#pragma region Stats Panel Single
	//Here we declare the images we'll use
	statsPanel_s = new Stats_Panel_Single();
	
	//Starting points of the frames
	int xF = 242, yF = 439;
	//Starting points 
	int xU = 245, yU = 442;
	
	//39 is the distance between the buttons and sprites
	//Using this loop you save code space, it looks cleaner
	for (uint i = 0; i < 3; i++)
	{
		statsPanel_s->upgrades_frames[i] = App->gui->CreateUI_Image({ xF, yF, 0, 0 }, atlasT, { 864, 0, 36, 36 });
		statsPanel_s->upgrades_frames[i]->SetLayer(2);

		statsPanel_s->upgrades_icons[i] = App->gui->CreateUI_Image({ xU, yU,  0, 0 }, orderIconsT, { 0, 0, 32, 32 });
		statsPanel_s->upgrades_icons[i]->SetLayer(1);

		xF += 39; xU += 39;
	}

	//Here we'll declare the rects in the textures
	statsPanel_s->upgradeIcons_rects.insert(std::make_pair<UPGRADES, SDL_Rect>(PLASMA_SHIELDS, { 144, 612, 32, 32 }));
	statsPanel_s->upgradeIcons_rects.insert(std::make_pair<UPGRADES, SDL_Rect>(GROUND_ARMOR, { 504, 578, 32, 32 }));
	statsPanel_s->upgradeIcons_rects.insert(std::make_pair<UPGRADES, SDL_Rect>(GROUND_WEAPONS, { 576, 578, 32, 32 }));
	statsPanel_s->upgradeIcons_rects.insert(std::make_pair<UPGRADES, SDL_Rect>(GROUND_WEAPONS_2, { 504, 680, 32, 32 }));
#pragma endregion
	*/
#pragma region Stats Panel Multiple

	statsPanel_m = new Stats_Panel_Mult();
	int xF_m = 168, yF_m = 396;
	//int xU = 245, yU = 442;

	for (uint j = 0; j < 2; j++)
	{
		for (uint i = 0, xF_m = 168; i < 6; i++)
		{
			uint index = i + (j * 6);
			statsPanel_m->unitSelect_frames[index] = App->gui->CreateUI_Image({ xF_m, yF_m, 0, 0 }, atlasT, { 936, 0, 33, 34 });
			statsPanel_m->unitSelect_frames[index]->SetLayer(1);

			statsPanel_m->unitSelect_wires[index] = App->gui->CreateUI_Image({ (xF_m + 1), (yF_m + 1), 0, 0 }, uiWireframesT, { 0, 0, 31, 32 });
			statsPanel_m->unitSelect_wires[index]->SetLayer(2);
			xF_m += 36;
		}
		yF_m += 37;
	}
	statsPanel_m->unitWireframe_rects.insert(std::make_pair<Unit_Type, SDL_Rect>(PROBE, { 4, 91, 31, 32 }));
	statsPanel_m->unitWireframe_rects.insert(std::make_pair<Unit_Type, SDL_Rect>(ZEALOT, { 44, 90, 31, 32 }));
	statsPanel_m->unitWireframe_rects.insert(std::make_pair<Unit_Type, SDL_Rect>(DRAGOON, { 86, 90, 24, 32 }));
#pragma endregion
#pragma region Grids
	coords = new Grid_Coords;

	//Image iterator
	UI_Image* image_it = NULL;
	UI_Button2* butt_it = NULL;

	//Makes the code cleaner
	M_EntityManager* ptr = App->entityManager;
	M_GUI* gui = App->gui;

	//Grid 3x3 definition

	//Button Rect Measueres
	SDL_Rect idle{ 1, 0, 33, 34 };
	SDL_Rect clicked{ 74, 1, 33, 34 };

	//Initialize Grid 3x3 frame
	coords->frame->SetActive(true);

	//Nexus
	Grid3x3* nexus = new Grid3x3(*coords, G_NEXUS);
	grids.push_back(nexus);
	gridTypes.push_back(nexus->type);

	//------------
	butt_it = nexus->setOrder(ptr->o_GenProbe_toss, idle, clicked, 0, 0, *atlasT);

	image_it = gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 468, 102, 32, 32 });
	image_it->SetParent(nexus->buttons[0]);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	//------------
	butt_it = nexus->setOrder(ptr->o_Set_rallyPoint, idle, clicked, 1, 2, *atlasT);

	image_it = gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, orderIconsT, { 504, 544, 32, 32 });
	image_it->SetParent(nexus->buttons[1]);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	nexus->changeState(false);

	//Basic Unit
	Grid3x3* basic_u = new Grid3x3(*coords,G_BASIC_UNIT);

	grids.push_back(basic_u);
	gridTypes.push_back(basic_u->type);

	gui->SetCurrentGrid(basic_u);

	butt_it = basic_u->setOrder(ptr->o_Move, idle, clicked, 0, 0, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 252, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[0]);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------
	butt_it = basic_u->setOrder(ptr->o_Stop, idle, clicked, 0, 1, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 288, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[1]);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------
	butt_it = basic_u->setOrder(ptr->o_Attack, idle, clicked, 0, 2, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 324, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[2]);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------
	basic_u->setOrder(ptr->o_Patrol, idle, clicked, 1, 0, *atlasT, true);
	//TODO: change image rect, used now for testing
	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 36, 304, 32, 32 });
	image_it->SetParent(basic_u->buttons[3]);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------

	basic_u->setOrder(ptr->o_Hold_pos, idle, clicked, 1, 1, *atlasT, true);

	//TODO: change image rect, used now for testing
	image_it = gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 108, 304, 32, 32 });
	image_it->SetParent(basic_u->buttons[4]);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	//------------

	Grid3x3* defaultGrid = new Grid3x3(*coords, G_DEFAULT);
	grids.push_back(defaultGrid);
	gridTypes.push_back(defaultGrid->type);
	
	//------------
	Grid3x3* basicBuildings = new Grid3x3(*coords, G_BASIC_BUILDINGS);

	grids.push_back(basicBuildings);
	gridTypes.push_back(basicBuildings->type);

	butt_it = basicBuildings->setOrder(ptr->o_Build_Assimilator, idle, clicked, 0, 0, *atlasT);

	image_it = gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 146, 306, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	basicBuildings->changeState(false);

	butt_it->son = image_it;

#pragma endregion
	//----------------------------------------------------------
}


void S_SceneMap::OnGUI(GUI_EVENTS event, UI_Element* element)
{
	if (element == map)
	{
		if (event == UI_MOUSE_DOWN)
		{
			movingMap = true;
		}
		else if(event == UI_MOUSE_EXIT || event == UI_MOUSE_UP || event == UI_LOST_FOCUS)
		{
			movingMap = false;
		}
	}
}

void S_SceneMap::SpawnResources()
{
	//Protoss base
	App->entityManager->CreateResource(12, 160, MINERAL);
	App->entityManager->CreateResource(14, 164, MINERAL);
	App->entityManager->CreateResource(11, 168, MINERAL);
	App->entityManager->CreateResource(13, 173, MINERAL);
	App->entityManager->CreateResource(18, 178, GAS);

	//Mid colonization zone
	App->entityManager->CreateResource(107, 121, MINERAL);
	App->entityManager->CreateResource(111, 124, MINERAL);
	App->entityManager->CreateResource(116, 126, MINERAL);
	App->entityManager->CreateResource(107, 125, MINERAL);
	App->entityManager->CreateResource(98, 134, GAS);

	//Zerg base
	App->entityManager->CreateResource(156, 6, MINERAL);
	App->entityManager->CreateResource(160, 8, MINERAL);
	App->entityManager->CreateResource(156, 11, MINERAL);
	App->entityManager->CreateResource(170, 18, MINERAL);
	App->entityManager->CreateResource(174, 20, MINERAL);
	App->entityManager->CreateResource(166, 42, GAS);
	App->entityManager->CreateResource(182, 30, GAS);

	//Upper base
	App->entityManager->CreateResource(3, 4, MINERAL);
	App->entityManager->CreateResource(13, 2, MINERAL);
	App->entityManager->CreateResource(9, 3, MINERAL);
	App->entityManager->CreateResource(26, 6, GAS);

	//Mid base
	App->entityManager->CreateResource(75, 80, MINERAL);
	App->entityManager->CreateResource(82, 84, MINERAL);
	App->entityManager->CreateResource(88, 87, MINERAL);
	App->entityManager->CreateResource(114, 62, GAS);

	//Bottom base
	App->entityManager->CreateResource(159, 140, MINERAL);
	App->entityManager->CreateResource(160, 144, MINERAL);
	App->entityManager->CreateResource(159, 150, MINERAL);
	App->entityManager->CreateResource(182, 144, GAS);	

}

void S_SceneMap::SpawnStartingUnits()
{

	App->entityManager->CreateBuilding(26, 168, NEXUS, PLAYER);

	App->entityManager->CreateBuilding(24, 150, PYLON, PLAYER);
	App->entityManager->CreateBuilding(42, 170, PYLON, PLAYER);

//	App->entityManager->CreateUnit(339, 2694, PROBE, PLAYER);
//	App->entityManager->CreateUnit(320, 2747, PROBE, PLAYER);
//	App->entityManager->CreateUnit(389, 2630, PROBE, PLAYER);
//	App->entityManager->CreateUnit(470, 2650, PROBE, PLAYER);
}

iPoint S_SceneMap::WorldToMinimap(int x, int y)
{
	SDL_Rect mapPos = map->GetWorldPosition();

	float currentX = x / (float)(App->map->data.width * App->map->data.tile_width);
	float currentY = y / (float)(App->map->data.height * App->map->data.tile_height);

	currentX = mapPos.x + currentX * mapPos.w;
	currentY = mapPos.y + currentY * mapPos.h;

	return iPoint(currentX, currentY);
}

iPoint S_SceneMap::MinimapToWorld(int x, int y)
{
	SDL_Rect mapPos = map->GetWorldPosition();

	float currentX = (x - mapPos.x) / (float)mapPos.w;
	float currentY = (y - mapPos.y) / (float)mapPos.h;

	currentX = currentX * (App->map->data.width * App->map->data.tile_width);
	currentY = currentY * (App->map->data.height * App->map->data.tile_height);

	return iPoint(currentX, currentY);
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