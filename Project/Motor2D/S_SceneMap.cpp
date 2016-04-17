#include "S_SceneMap.h"

#include "j1App.h"
#include "M_Input.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_PathFinding.h"
#include "M_GUI.h"
#include "M_EntityManager.h"
#include "Entity.h"
#include "Unit.h"
#include "Resource.h"
#include "M_IA.h"
#include "M_CollisionController.h"
//#include "j1Gui.h"
//#include "UIElements.h"
//#include "M_Fonts.h"
#include "M_Console.h"
#include "M_GUI.h"
#include "Building.h"
#include "M_Map.h"
#include "S_SceneMenu.h"
#include "Stats panel.h"

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
	int w, h, scale;
	scale = App->win->GetScale();
	App->win->GetWindowSize(&w, &h);

	gameFinished = false;
	victory = false;
	defeat = false;

	//TMP ------------------------
	onEvent = true;
	action1 = action2 = action3 = action4 = false;
	//----------------------------

	quit_info_font = App->font->Load("fonts/StarCraft.ttf", 12);


	sfx_shuttle_drop = App->audio->LoadFx("sounds/sounds/shuttle_drop.wav");
	sfx_script_adquire = App->audio->LoadFx("sounds/sounds/button.wav");


	App->map->Enable();
	App->map->Load("graphic.tmx");

	App->pathFinding->Enable();
	App->pathFinding->LoadWalkableMap("maps/walkable.tmx");

	App->entityManager->Enable();
	App->collisionController->Enable();
	App->missiles->Enable();
	App->IA->Enable();

	//UI WEIRD STUFF ------------------------------------
	//It is not weird >///<

	numUnit = 0;
	LoadTextures();
	LoadGUI();

	//---------------------------------------------------

	App->audio->PlayMusic("sounds/sounds/ambient/protoss-3.wav", 2.0f);

	//---------------------------------------------------
	//Create quit menu
	quit_image = App->gui->CreateUI_Image({ (w/2 - 300)/ scale, (h / 2 - 350) / scale, 540 / scale, 300 / scale }, quit_tex, { 0, 0, 0, 0 });

	yes_label = App->gui->CreateUI_Label({ 110 / scale, 250 / scale, 0, 0 }, "Yes", quit_info_font, { -90 / scale, -20 / scale, 245 / scale, 60 / scale });
	yes_label->AddListener(this);
	yes_label->SetParent(quit_image);

	no_label = App->gui->CreateUI_Label({ 380 / scale, 250 / scale, 0, 0 }, "No", quit_info_font, { -100 / scale, -20 / scale, 245 / scale, 60 / scale });
	no_label->AddListener(this);
	no_label->SetParent(quit_image);

	quit_label = App->gui->CreateUI_Label({60 / scale, 100 / scale, 0, 0 }, "Are you sure you want to quit?", quit_info_font, { 0, 0, 0, 0 });
	quit_label->SetParent(quit_image);
	quit_image->SetActive(false);


	//---------------------------------------------------

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

	player.gas = 0;
	player.mineral = 80;
	App->render->camera.x = 215;
	App->render->camera.y = 5120;

	App->gui->SetCurrentGrid(G_NONE);

	if (!onEvent)
	{
		App->entityManager->CreateUnit(339, 2694, PROBE, PLAYER);
		App->entityManager->CreateUnit(320, 2747, PROBE, PLAYER);
		App->entityManager->CreateUnit(615, 2605, ZEALOT, PLAYER);
		App->entityManager->CreateUnit(625, 2560, DRAGOON, PLAYER);
		App->entityManager->CreateUnit(580, 2570, ZEALOT, PLAYER);
	}

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
	if (gameFinished)
		return true;

	// Scripts
	if (onEvent)
		FirstEventScript();

	ManageInput(dt);

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
			globalMouse->SetText(C_String("World: %i, %i", (x + App->render->camera.x / App->win->GetScale()), (y + App->render->camera.y / App->win->GetScale())));

			iPoint tile = App->pathFinding->WorldToMap(x + App->render->camera.x / App->win->GetScale(), y + App->render->camera.y / App->win->GetScale());
			tileMouse->SetText(C_String("Logic Tile: %i, %i", tile.x, tile.y));
		}
	}

	//Render current tile
	iPoint p = App->pathFinding->MapToWorld(currentTile_x, currentTile_y);
	currentTileSprite.position.x = p.x;
	currentTileSprite.position.y = p.y;
	App->render->AddSprite(&currentTileSprite, GUI);


	//UI WEIRD STUFF -------------------------------------
	//Update resources display
	char it_res_c[9];
	sprintf_s(it_res_c, 7, "%d", player.mineral);
	res_lab[0]->SetText(it_res_c);

	sprintf_s(it_res_c, 7, "%d", player.gas);
	res_lab[1]->SetText(it_res_c);

	sprintf_s(it_res_c, 9, "%d/%d", player.psi, player.maxPsi);
	res_lab[2]->SetText(it_res_c);

#pragma region Victory_Conditions
	if (gameFinished == false)
	{
		if (App->GetFrameCount() % 120 == 0)
		{
			if (zergSample->state == BS_DEAD)
			{
				defeat = true;
			}
			if (App->IA->basesList.empty() == true)
			{
				victory = true;
			}
			else
			{
				std::vector<Base*>::iterator it = App->IA->basesList.begin();
				while (it != App->IA->basesList.end())
				{
					if ((*it)->defeated == false)
					{
						break;
					}
					it++;
					if (it == App->IA->basesList.end())
					{
						victory = true;
					}
				}

			}
			if (victory || defeat)
				useConditions();

		}
	}
#pragma endregion

	//TMP updating UI
	int w, h, scale;
	scale = App->win->GetScale();
	App->win->GetWindowSize(&w, &h);
	//Stretching UI to it the screen. Screen size should not change during game, so this may stay commented.
	/*
	controlPanel->localPosition.y = h / scale - 178;
	controlPanel->localPosition.w = w / scale;
	controlPanel->collider.w = w / scale;

	map->localPosition.w = w * (130.0f / 1280.0f);
	map->collider.w = w *  (130.0f / 1280.0f);
	map->localPosition.x = w * (5.0f / 1280.0f);

	res_img[0]->localPosition.x = (w - 408) / scale;
	res_img[1]->localPosition.x = (w - 272) / scale;
	res_img[2]->localPosition.x = (w - 136) / scale;

	res_lab[0]->localPosition.x = (w - 376) / scale;
	res_lab[1]->localPosition.x = (w - 240) / scale;
	res_lab[2]->localPosition.x = (w - 104) / scale;
	*/

	//---------------------------------------------------
	//Update Minimap rect

	if (onEvent == false)
	{
		if (movingMap)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			iPoint pos = MinimapToWorld(x, y);

			App->render->camera.x = pos.x * App->win->GetScale() - App->render->camera.w / scale;
			App->render->camera.y = pos.y * App->win->GetScale() - App->render->camera.h / scale;
		}
	}
	int xMax, yMax;
	xMax = App->map->data.width * App->map->data.tile_width * scale;
	yMax = App->map->data.height * App->map->data.tile_height * scale;
	xMax -= w;
	yMax -= h - 100;

	CAP(App->render->camera.x, 0, xMax);
	CAP(App->render->camera.y, 0, yMax);

	iPoint pos = WorldToMinimap(App->render->camera.x / scale, App->render->camera.y / scale);
	App->render->AddDebugRect({ pos.x, pos.y, w * (56.0f / 1280.0f) / scale, h * (56.0f / 1280.0f) / scale }, false, 255, 255, 255, 255, false);

	//TMP
#pragma region //Drawing minimap units & buildings
	std::list<Unit*>::iterator it = App->entityManager->unitList.begin();
	while (it != App->entityManager->unitList.end())
	{
		if ((*it)->active && ((*it)->GetMovementState() != MOVEMENT_DIE || App->entityManager->debug))
		{
			iPoint toDraw = WorldToMinimap((*it)->GetPosition().x, (*it)->GetPosition().y);
			if ((*it)->selected)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 255, 255, 255, 200);
			}
			else if ((*it)->stats.player == PLAYER)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 0, 244, 5, 200);
			}
			else if ((*it)->stats.player == COMPUTER)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 255, 0, 0, 200);
			}
			/*else if ((*it)->stats.player == CINEMATIC)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 2, 2 }, false, 255, 255, 0, 200);
			}*/

		}
		it++;
	}

	std::list<Building*>::iterator it2 = App->entityManager->buildingList.begin();
	while (it2 != App->entityManager->buildingList.end())
	{
		if ((*it2)->active)
		{

			iPoint toDraw = App->pathFinding->MapToWorld((*it2)->GetPosition().x, (*it2)->GetPosition().y);
			toDraw = WorldToMinimap(toDraw.x, toDraw.y);
			if ((*it2)->selected)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 3, 3 }, false, 255, 255, 255, 200);
			}
			else if ((*it2)->stats.player == PLAYER)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 3, 3 }, false, 0, 244, 5, 200);
			}
			else if ((*it2)->stats.player == COMPUTER)
			{
				App->render->AddDebugRect(SDL_Rect{ toDraw.x, toDraw.y, 3, 3 }, false, 255, 0, 0, 200);
			}

		}
		it2++;
	}

#pragma endregion

	App->map->Draw();

	return true;
}

// Called each loop iteration
bool S_SceneMap::PostUpdate(float dt)
{
	bool ret = true;
	if (gameFinished)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			App->changeScene(App->sceneMenu, this);
		}

	}
	return ret;
}

// Called before quitting
bool S_SceneMap::CleanUp()
{
	LOG("Freeing scene");
	
	App->gui->SetCurrentGrid(NULL);
	//Free textures (Should be done with a private list)
	App->tex->UnLoad(uiIconsT);
	App->tex->UnLoad(orderIconsT);
	App->tex->UnLoad(atlasT);
	App->tex->UnLoad(controlPT);
	App->tex->UnLoad(uiWireframesT);
	App->tex->UnLoad(minimap);

	App->tex->UnLoad(victoryT);
	App->tex->UnLoad(defeatT);
	App->tex->UnLoad(debug_tex);
	App->tex->UnLoad(quit_tex);
	
	//Delete all unit elements
	App->gui->DeleteUIElement(screenMouse);
	App->gui->DeleteUIElement(globalMouse);
	App->gui->DeleteUIElement(tileMouse);

	App->gui->DeleteUIElement(controlPanel);
	App->gui->DeleteUIElement(map);
	App->gui->DeleteUIElement(finalScreen);
	App->gui->DeleteUIElement(yes_label);
	App->gui->DeleteUIElement(no_label);
	App->gui->DeleteUIElement(quit_image);
	App->gui->DeleteUIElement(quit_label);

	for (uint i = 0; i < 3; i++)
	{
		App->gui->DeleteUIElement(res_img[i]);
		App->gui->DeleteUIElement(res_lab[i]);
	}

	//Release all grids and the coords class
	RELEASE(coords);
	//We release it backwards because there are grids that use buttons that other grids
	//use. 
	for (std::vector<Grid3x3*>::reverse_iterator it1 = grids.rbegin(); it1 != grids.rend(); it1++)
	{
		delete *it1;
	}
	//No dangling pointers!
	grids.clear();

	App->map->Disable();

	App->pathFinding->Disable();

	App->entityManager->Disable();
	App->collisionController->Disable();
	App->missiles->Disable();
	App->IA->Disable();

	return true;
}

void S_SceneMap::ManageInput(float dt)
{
	if (App->input->GetInputState() == false)
	{

		if (onEvent == false)
		{
			if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
				App->render->camera.y -= (int)floor(CAMERA_SPEED * dt);

			if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
				App->render->camera.y += (int)floor(CAMERA_SPEED * dt);

			if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
				App->render->camera.x -= (int)floor(CAMERA_SPEED * dt);

			if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
				App->render->camera.x += (int)floor(CAMERA_SPEED * dt);
		}

		//Enable / Disable map render
		if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_UP)
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

		if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
			App->gui->debug = !App->gui->debug;

		if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
			App->pathFinding->displayPath = !App->pathFinding->displayPath;

		if (App->entityManager->debug)
		{
			UnitCreationInput();

			if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
				onEvent = !onEvent;

			if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
			{
				player.mineral += 1000;
			}
			if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
			{
				player.gas += 1000;
			}
			if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
			{
				player.realMaxPsi += 100;
				player.maxPsi = player.realMaxPsi;
				if (player.maxPsi > 200)
				{
					player.maxPsi = 200;
				}
			}

			if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
				victory = true;
			if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
				defeat = true;

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

		}
	}

	//UI WEIRD STUFF -----------------------------------------------------
		//Change Grids
		/*if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
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
		*/

		if (onEvent == false)
		{
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
		}
		
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			quit_image->SetActive(!quit_image->GetActive());
		}

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
		unit = App->entityManager->CreateUnit(p.x + 8, p.y + 8, DRAGOON, PLAYER);
	}

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->pathFinding->WorldToMap(p.x, p.y);
		p = App->pathFinding->MapToWorld(p.x, p.y);
		unit = App->entityManager->CreateUnit(p.x + 8, p.y + 8, ZEALOT, PLAYER);
	}

	if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->pathFinding->WorldToMap(p.x, p.y);
		p = App->pathFinding->MapToWorld(p.x, p.y);
		unit = App->entityManager->CreateUnit(p.x + 8, p.y + 8, ZERGLING, COMPUTER);
	}

	if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->pathFinding->WorldToMap(p.x, p.y);
		p = App->pathFinding->MapToWorld(p.x, p.y);
		unit = App->entityManager->CreateUnit(p.x + 8, p.y + 8, MUTALISK, COMPUTER);
	}

	if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->pathFinding->WorldToMap(p.x, p.y);
		p = App->pathFinding->MapToWorld(p.x, p.y);
		unit = App->entityManager->CreateUnit(p.x + 8, p.y + 8, HYDRALISK, COMPUTER);
	}

	if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->pathFinding->WorldToMap(p.x, p.y);
		p = App->pathFinding->MapToWorld(p.x, p.y);
		unit = App->entityManager->CreateUnit(p.x + 8, p.y + 8, ULTRALISK, COMPUTER);
	}
}

void S_SceneMap::LoadTextures()
{
	controlPT = App->tex->Load("gui/pconsole.png");
	orderIconsT = App->tex->Load("gui/cmdicons.png");
	atlasT = App->tex->Load("gui/pcmdbtns.png");
	uiIconsT = App->tex->Load("gui/icons.png");
	minimap = App->tex->Load("maps/graphic.png");
	uiWireframesT = App->tex->Load("gui/Wireframes.png");

	//Orders hover textures
	orderAssimilator_hover = App->tex->Load("graphics/ui/hover texts/assimilator_build.png");
	orderDragoon_hover = App->tex->Load("graphics/ui/hover texts/dragoon_create.png");
	orderForge_hover = App->tex->Load("graphics/ui/hover texts/forge_build.png");
	orderGateway_hover = App->tex->Load("graphics/ui/hover texts/gateway_build.png");
	orderNexus_hover = App->tex->Load("graphics/ui/hover texts/nexus_build.png");
	orderAdvancedStructure_hover = App->tex->Load("graphics/ui/hover texts/order_advancedStructure.png");
	orderAttack_hover = App->tex->Load("graphics/ui/hover texts/order_attack.png");
	orderCancel_hover = App->tex->Load("graphics/ui/hover texts/order_cancel.png");
	orderGather_hover = App->tex->Load("graphics/ui/hover texts/order_gather.png");
	orderMove_hover = App->tex->Load("graphics/ui/hover texts/order_move.png");
	orderRallypoint_hover = App->tex->Load("graphics/ui/hover texts/order_rallyPoint.png");
	orderReturnCargo_hover = App->tex->Load("graphics/ui/hover texts/order_returnCargo.png");
	orderStop_hover = App->tex->Load("graphics/ui/hover texts/order_stop.png");
	orderStructure_hover = App->tex->Load("graphics/ui/hover texts/order_structure.png");
	orderHold_hover = App->tex->Load("graphics/ui/hover texts/orderHold_hover.png");
	orderPatrol_hover = App->tex->Load("graphics/ui/hover texts/orderPatrol_hover.png");
	orderProbe_hover = App->tex->Load("graphics/ui/hover texts/probe_create.png");
	orderPylon_hover = App->tex->Load("graphics/ui/hover texts/pylon_build.png");
	orderZealot_hover = App->tex->Load("graphics/ui/hover texts/zealot_create.png");

	//Orders hover requirments
	orderCybernetics_requirement = App->tex->Load("graphics/ui/hover texts/cybernetics_requires.png");
	orderDragoon_requirement = App->tex->Load("graphics/ui/hover texts/dragoon_requires.png");
	orderPhotonCannon_requirement = App->tex->Load("graphics/ui/hover texts/photonCannon_requires.png");
	orderShieldBattery_requirement = App->tex->Load("graphics/ui/hover texts/shieldBattery_requires.png");
	orderTemplar_requirement = App->tex->Load("graphics/ui/hover texts/templar_requires.png");

	//Quit texture
	quit_tex = App->tex->Load("graphics/ui/readyt/pdpopup.png");

}

void S_SceneMap::LoadGUI()
{
	//UI WEIRD STUFF----------------------------------
	//Minerals Image
	int w, h, scale;
	App->win->GetWindowSize(&w, &h);
	scale = App->win->GetScale();

	res_img[0] = App->gui->CreateUI_Image({ (w - 408) / scale, 3, 0, 0 }, (SDL_Texture*)uiIconsT, { 0, 0, 14, 14 });

	res_img[1] = App->gui->CreateUI_Image({ (w - 272) / scale, 3, 0, 0 }, (SDL_Texture*)uiIconsT, { 0, 42, 14, 14 });

	res_img[2] = App->gui->CreateUI_Image({ (w - 136) / scale, 3, 0, 0 }, (SDL_Texture*)uiIconsT, { 0, 84, 14, 14 });

	res_lab[0] = App->gui->CreateUI_Label({ (w - 376) / scale, 4, 0, 0 }, "0");

	res_lab[1] = App->gui->CreateUI_Label({ (w - 240) / scale, 4, 0, 0 }, "0");

	res_lab[2] = App->gui->CreateUI_Label({ (w - 104) / scale, 4, 0, 0 }, "0");

	for (int n = 0; n < 2; n++)
	{
		res_img[n]->SetLayer(0);
		res_lab[n]->SetLayer(1);
	}

	// Inserting the control Panel Image

	controlPanel = App->gui->CreateUI_Image({ 0, h / App->win->GetScale() -178, w / App->win->GetScale(), 178 }, controlPT, { 0, 0, 0, 0 }, { 0, 60, 640, 118 });
	controlPanel->SetLayer(1);

	map = App->gui->CreateUI_Image({ w * (5.0f / 1280.0f), 45, w * (130.0f / 1280.0f), 130 }, minimap, { 0, 0, 0, 0 });
	map->collider = { -8, -8, map->localPosition.w + 16, map->localPosition.h + 16 };
	map->SetParent(controlPanel);
	map->SetLayer(1);
	map->AddListener(this);
	
#pragma region Stats Panel Single
	/*
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
	*/
#pragma endregion
	
#pragma region Stats Panel Multiple
	/*
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
	*/
#pragma endregion
#pragma region Grids
	coords = new Grid_Coords;

	//Image iterator
	UI_Image* image_it = NULL;
	UI_Button2* butt_it = NULL;

	//Grid 3x3 definition

	//Button Rect Measueres
	SDL_Rect idle{ 1, 0, 33, 34 };
	SDL_Rect clicked{ 74, 1, 33, 34 };

	//Initialize Grid 3x3 frame
	//coords->frame->SetActive(true);

	//Nexus
	Grid3x3* nexus = new Grid3x3(*coords, G_NEXUS);
	grids.push_back(nexus);
	gridTypes.push_back(nexus->type);

	//------------
	//Create probe button
	butt_it = nexus->setOrder(App->entityManager->o_GenProbe_toss, idle, clicked, 0, 0, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -26, 0, 0 }, orderProbe_hover, { 0, 0, 60, 26 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 468, 102, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	//------------
	//Rally point button
	butt_it = nexus->setOrder(App->entityManager->o_Set_rallyPoint, idle, clicked, 1, 2, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ -37, -13, 0, 0 }, orderRallypoint_hover, { 0, 0, 77, 13 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, orderIconsT, { 504, 544, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);



	butt_it->son = image_it;


	nexus->changeState(false);

	//Basic Unit
	Grid3x3* basic_u = new Grid3x3(*coords,G_BASIC_UNIT);

	grids.push_back(basic_u);
	gridTypes.push_back(basic_u->type);

	//gui->SetCurrentGrid(basic_u);

	butt_it = basic_u->setOrder(App->entityManager->o_Move, idle, clicked, 0, 0, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderMove_hover, { 0, 0, 29, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 252, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------
	butt_it = basic_u->setOrder(App->entityManager->o_Stop, idle, clicked, 0, 1, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -13, 0, 0 }, orderStop_hover, { 0, 0, 26, 13 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 288, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------
	butt_it = basic_u->setOrder(App->entityManager->o_Attack, idle, clicked, 0, 2, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderAttack_hover, { 0, 0, 36, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 324, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------
	butt_it = basic_u->setOrder(App->entityManager->o_Patrol, idle, clicked, 1, 0, *atlasT, true);

	//Hovering imagega
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderPatrol_hover, { 0, 0, 32, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 4, 1, 0, 0 }, orderIconsT, { 576, 474, 26, 29 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	image_it->sprite.tint.g = image_it->sprite.tint.b = image_it->sprite.tint.r = 90;

	butt_it->son = image_it;


	//------------

	butt_it = basic_u->setOrder(App->entityManager->o_Hold_pos, idle, clicked, 1, 1, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderHold_hover, { 0, 0, 69, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 2, 0, 0 }, orderIconsT, { 0, 509, 27, 29 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	image_it->sprite.tint.g = image_it->sprite.tint.b =	image_it->sprite.tint.r = 90;
	

	butt_it->son = image_it;

	basic_u->changeState(false);
	//------------

	Grid3x3* basicBuildings = new Grid3x3(*coords, G_BASIC_BUILDINGS);

	grids.push_back(basicBuildings);
	gridTypes.push_back(basicBuildings->type);

	// Nexus { 108, 304, 32, 32 }
	// Pylon  { 36, 304, 32, 32 }

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Pylon, idle, clicked, 0, 0, *atlasT);
	
	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -28, 0, 0 }, orderPylon_hover, { 0, 0, 72, 28 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 108, 304, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Nexus, idle, clicked, 0, 1, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -28, 0, 0 }, orderNexus_hover, { 0, 0, 76, 28 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 36, 304, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Gateaway, idle, clicked, 0, 2, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ -48, -28, 0, 0 }, orderGateway_hover, { 0, 0, 88, 28 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 252, 306, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Assimilator, idle, clicked, 1, 0, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -28, 0, 0 }, orderAssimilator_hover, { 0, 0, 100, 28 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 144, 306, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	butt_it = basicBuildings->setOrder(App->entityManager->o_Return_Builds_Menu, idle, clicked, 2, 2, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ -28, -11, 0, 0 }, orderCancel_hover, { 0, 0, 69, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 4, 0, 0 }, orderIconsT, { 540, 442, 26, 26 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	basicBuildings->changeState(false);
	
	//-----------
	Grid3x3* probeMenu = new Grid3x3(*coords, G_PROBE);
	grids.push_back(probeMenu);
	gridTypes.push_back(probeMenu->type);

//Copy the buttons from the basic unit -------------
	//I know this is like super bad but there where memory managment issues
	//So this is a temporary solution

	butt_it = probeMenu->setOrder(App->entityManager->o_Move, idle, clicked, 0, 0, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderMove_hover, { 0, 0, 29, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 252, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------
	butt_it = probeMenu->setOrder(App->entityManager->o_Stop, idle, clicked, 0, 1, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -13, 0, 0 }, orderStop_hover, { 0, 0, 29, 13 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 288, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------
	butt_it = probeMenu->setOrder(App->entityManager->o_Attack, idle, clicked, 0, 2, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderAttack_hover, { 0, 0, 36, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 324, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	
	butt_it = probeMenu->setOrder(App->entityManager->o_Gather, idle, clicked, 1, 1, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderGather_hover, { 0, 0, 37, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 5, 0, 0 }, orderIconsT, { 360, 442, 28, 25 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);
	image_it->sprite.tint.g = image_it->sprite.tint.b = image_it->sprite.tint.r = 90;

	butt_it->son = image_it;

	butt_it = probeMenu->setOrder(App->entityManager->o_Ret_Cargo, idle, clicked, 1, 2, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ -28, -13, 0, 0 }, orderReturnCargo_hover, { 0, 0, 68, 13 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 1, 0, 0 }, orderIconsT, { 429, 440, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);
	image_it->sprite.tint.g = image_it->sprite.tint.b = image_it->sprite.tint.r = 90;

	butt_it->son = image_it;
	
	butt_it = probeMenu->setOrder(App->entityManager->o_Basic_Builds, idle, clicked, 2, 0, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderStructure_hover, { 0, 0, 79, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 5, 0, 0 }, orderIconsT, { 0, 542, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	

	butt_it->son = image_it;

	probeMenu->changeState(false);

	//----------------
	Grid3x3* gateways = new Grid3x3(*coords, G_GATEWAY);
	grids.push_back(gateways);
	gridTypes.push_back(gateways->type);

	butt_it = gateways->setOrder(App->entityManager->o_Gen_Zealot, idle, clicked, 0, 0, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -28, 0, 0 }, orderZealot_hover, { 0, 0, 76, 28 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 324, 136, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	butt_it = gateways->setOrder(App->entityManager->o_Gen_Dragoon, idle, clicked, 0, 1, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ -2, -28, 0, 0 }, orderDragoon_hover, { 0, 0, 89, 28 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 360, 136, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it = gateways->setOrder(App->entityManager->o_Set_rallyPoint, idle, clicked, 1, 2, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ -37, -13, 0, 0 }, orderRallypoint_hover, { 0, 0, 77, 13 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, orderIconsT, { 504, 544, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	nexus->changeState(false);

	gateways->changeState(false);

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

	if (element == yes_label && event == UI_MOUSE_DOWN)
	{
		quit_image->SetActive(false);	
		App->changeScene(App->sceneMenu, this);
	}

	if (element == no_label && event == UI_MOUSE_DOWN)
	{
		quit_image->SetActive(false);
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
	App->entityManager->CreateResource(112, 124, MINERAL);
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
	//That's not a good idea, it should be done in the building by default
	zergSample = App->entityManager->CreateBuilding(25, 151, ZERG_SAMPLE, PLAYER);

	App->entityManager->CreateBuilding(26, 168, NEXUS, PLAYER);

	App->entityManager->CreateBuilding(42, 170, PYLON, PLAYER);
	player.psi = 8;
}

void S_SceneMap::FirstEventScript()
{
	if (action1 == false)
	{
		scripted_unit1 = App->entityManager->CreateUnit(10, 3000, CARRIER, CINEMATIC);
		scripted_unit2 = App->entityManager->CreateUnit(200, 3000, SCOUT, CINEMATIC);
		scripted_unit3 = App->entityManager->CreateUnit(65, 2880, SCOUT, CINEMATIC);

		scripted_shuttle1 = App->entityManager->CreateUnit(17, 2925, SHUTTLE, CINEMATIC);
		scripted_shuttle2 = App->entityManager->CreateUnit(105, 3005, SHUTTLE, CINEMATIC);

		action1 = true;
	}

	if (action1) // All Units appear from the corner
	{
		if (action4 == false)
		{
			App->render->camera.x = (scripted_unit1->GetPosition().x * App->win->GetScale()) - 540;
			App->render->camera.y = scripted_unit1->GetPosition().y * App->win->GetScale() - 480;
		}

		if (action2 == false)
		{
			scripted_unit1->SetTarget(585, 2650);
			scripted_unit2->SetTarget(600, 2820);
			scripted_unit3->SetTarget(400, 2610);

			scripted_shuttle1->SetTarget(330, 2725);
			scripted_shuttle2->SetTarget(605, 2575);

			action2 = true;
		}
	}

	if (action2) // Shuttles Drop Units and go Back
	{
		if (!action3 && scripted_shuttle1->GetMovementState() == MOVEMENT_IDLE)
		{
			App->entityManager->CreateUnit(339, 2694, PROBE, PLAYER);
			App->entityManager->CreateUnit(320, 2747, PROBE, PLAYER);

			App->audio->PlayFx(sfx_shuttle_drop, 0);

			scripted_shuttle1->SetTarget(17, 2925);
		}

		if (!action3 && scripted_shuttle2->GetMovementState() == MOVEMENT_IDLE)
		{
			App->entityManager->CreateUnit(615, 2605, ZEALOT, PLAYER);
			App->entityManager->CreateUnit(625, 2560, DRAGOON, PLAYER);
			App->entityManager->CreateUnit(580, 2570, ZEALOT, PLAYER);

			App->audio->PlayFx(sfx_shuttle_drop, 0);

			scripted_shuttle2->SetTarget(105, 3005);
			action3 = true;
		}
	}

	if (action3) // Scouts prepares formation and leave
	{
		if (!action4 && scripted_shuttle1->GetMovementState() == MOVEMENT_IDLE)
		{
			scripted_unit2->SetTarget(700, 2680);
			scripted_unit3->SetTarget(540, 2600);
		}

		if (!action4 && scripted_shuttle2->GetMovementState() == MOVEMENT_IDLE && scripted_shuttle1->GetMovementState() == MOVEMENT_IDLE)
		{
			scripted_shuttle2->Hit(1000000);
			scripted_shuttle1->Hit(1000000);

			scripted_unit1->SetTarget(1070, 2300);
			scripted_unit2->SetTarget(1140, 2300);
			scripted_unit3->SetTarget(1000, 2300);

			action4 = true;
		}
	}

	if (action4 && onEvent) // Clean Up the Script
	{
		if (scripted_unit1->GetMovementState() == MOVEMENT_IDLE)
		{
			scripted_unit1->Hit(1000000);
			scripted_unit2->Hit(1000000);
			scripted_unit3->Hit(1000000);

			App->audio->PlayFx(sfx_script_adquire);

			onEvent = false;
			action1 = action2 = action3 = action4 = false;
		}
	}
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

void S_SceneMap::useConditions()
{
	SDL_Texture* use = NULL;
	if (defeat)
	{
		gameFinished = true;
		use = victoryT = App->tex->Load("gui/defeatScreenTMP.png");
		App->audio->PlayMusic("sounds/sounds/ambient/defeat.wav", 1.0f);
	}
	//Else if
	if (victory)
	{
		gameFinished = true;
		use = defeatT = App->tex->Load("gui/victoryScreenTMP.png");
		App->audio->PlayMusic("sounds/sounds/ambient/victory.wav", 1.0f);
	}
	int w, h;
	App->win->GetWindowSize(&w, &h);
	finalScreen = App->gui->CreateUI_Image({ 0, 0, w / App->win->GetScale(), h / App->win->GetScale() }, use, { 0, 0, 0, 0 });
	finalScreen->SetLayer(3);
}
#pragma endregion