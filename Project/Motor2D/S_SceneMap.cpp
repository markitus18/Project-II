#include "S_SceneMap.h"

#include "j1App.h"

#include "M_InputManager.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_Render.h"
#include "M_PathFinding.h"

#include "M_EntityManager.h"

#include "Resource.h"
#include "M_IA.h"
#include "M_CollisionController.h"
#include "M_Console.h"
#include "Building.h"
#include "M_Map.h"
#include "S_SceneMenu.h"
#include "Stats panel.h"
#include "M_FogOfWar.h"
#include "M_Explosion.h"
#include "UI_Panel_Queue.h"
#include "M_Particles.h"
#include "M_InputManager.h"
#include "M_Player.h"
#include "M_Minimap.h"

//TO CHANGE: scene is including unit_type enum from somewhere

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

	App->console->AddCommand(&c_SaveGame);
	App->console->AddCommand(&c_LoadGame);

	return ret;
}

// Called before the first frame
bool S_SceneMap::Start()
{
	App->SetCurrentScene(this);
	int w, h, scale;
	scale = App->events->GetScale();
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;

	gameFinished = false;
	victory = false;
	defeat = false;

	//SCRIPT RESOURCES -----------
	onEvent = true;
	kerriganSpawn = false;
	action = action_aux = false;
	interruptEvent = false;
	scriptTimer.Start();
	scriptTimer.Stop();

	spawnSplash.texture = App->tex->Load("graphics/zerg/boss/boss_spawn.png");
	spawnSplash.position = { 0, 0, 160, 192 };
	spawnSplash.section = { 0, 0, 160, 192 };

	bloodSplash.texture = App->tex->Load("graphics/zerg/boss/boss_blood.png");
	bloodSplash.position = { 0, 0, 128, 128 };
	bloodSplash.section = { 0, 0, 128, 128 };

	intro_text = App->gui->CreateUI_Label({ 100 / scale, 400 / scale, 0, 0 }, "Incoming Transmission...", quit_info_font);
	intro_text->SetActive(false);
	//----------------------------
	displayed_mineral = displayed_gas = 0;
	psi_reached_timer = 0;
	//----------------------------

	quit_info_font = App->font->Load("fonts/StarCraft.ttf", 12);

	sfx_shuttle_drop = App->audio->LoadFx("sounds/protoss/units/shuttle_drop.ogg");
	sfx_script_adquire = App->audio->LoadFx("sounds/ui/adquire.ogg");
	sfx_script_beep = App->audio->LoadFx("sounds/ui/beep.ogg");

	App->map->Enable();
	App->map->Load("graphic.tmx");

	App->pathFinding->Enable();

	App->player->Enable();
	App->entityManager->Enable();
	App->collisionController->Enable();
	App->explosion->Enable();
	App->missiles->Enable();
	App->particles->Enable();


	//UI WEIRD STUFF ------------------------------------
	//It is not weird >///<

	numUnit = 0;
	LoadTextures();
	LoadGUI();

	//-----------------

	App->minimap->Enable();
	App->IA->Enable();

	//---------------------------------------------------

	App->fogOfWar->Enable();
	App->fogOfWar->SetUp(App->map->data.tile_width * App->map->data.width, App->map->data.tile_height * App->map->data.height, 96, 96, 3);
	App->fogOfWar->maps[1]->maxAlpha = 125;
	App->fogOfWar->maps[2]->draw = false;
	SDL_Rect minimapSize = App->minimap->map->GetWorldPosition();
	App->fogOfWar->SetMinimap(minimapSize.x, minimapSize.y, minimapSize.w, minimapSize.h, 2);

	App->audio->PlayMusic("sounds/music/ambient/protoss-3.ogg", 2.0f);

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
	
	debug_tex = App->tex->Load("graphics/gui/current_tile.png");
	
	currentTileSprite.texture = App->tex->Load("graphics/gui/current_tile.png");;
	currentTileSprite.section = { 0, 0, 64, 64 };
	currentTileSprite.position = { 0, 0, 16, 16 };
	currentTileSprite.useCamera = true;
	currentTileSprite.layer = GUI_MAX_LAYERS;

	App->events->UnfreezeInput();

	SpawnResources();
	SpawnStartingUnits();

	screenMouse = App->gui->CreateUI_Label(SDL_Rect{ 10, 10, 0, 0 }, "0");
	globalMouse = App->gui->CreateUI_Label(SDL_Rect{ 10, 30, 0, 0 }, "0");
	tileMouse = App->gui->CreateUI_Label(SDL_Rect{ 10, 50, 0, 0 }, "0");

	screenMouse->SetActive(App->entityManager->debug);
	globalMouse->SetActive(App->entityManager->debug);
	tileMouse->SetActive(App->entityManager->debug);

	App->render->camera.x = 215;
	App->render->camera.y = 5120;

	App->gui->SetCurrentGrid(G_NONE);

	if (!onEvent)
	{
		App->entityManager->CreateUnit(320, 2747, PROBE, PLAYER);
		App->entityManager->CreateUnit(300, 2647, PROBE, PLAYER);
		App->entityManager->CreateUnit(339, 2694, PROBE, PLAYER);
		App->entityManager->CreateUnit(615, 2605, ZEALOT, PLAYER);
		App->entityManager->CreateUnit(625, 2560, DRAGOON, PLAYER);
		App->entityManager->CreateUnit(580, 2570, ZEALOT, PLAYER);
		App->player->AddPsi(10);
	}
	else
	{
		App->entityManager->FreezeInput();
		App->minimap->freezeMovement = true;
	}

	App->render->SetCameraLimits({ 1, 1 }, App->events->GetMapSizeScaled() - App->events->GetScreenSize());

	return true;
}

// Called each loop iteration
bool S_SceneMap::PreUpdate()
{
	//Getting current tile
	iPoint p = App->events->GetMouseOnWorld();
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
	if (App->IA->createBoss)
	{
		onEvent = true;
		kerriganSpawn = true;
	}

	if (onEvent)
	{
		if (!kerriganSpawn)
		{
			FirstEventScript();
		}
		else if (kerriganSpawn)
		{
			SecondEventScript();
		}
	}
	else
	{
		App->entityManager->UnfreezeInput();
		App->minimap->freezeMovement = false;
	}

	ManageInput(dt);

	if (App->entityManager->debug)
	{
		App->pathFinding->Draw();

		labelUpdateTimer += (1.0f * dt);
		if (labelUpdateTimer > 0.1f)
		{
			labelUpdateTimer = 0.0f;
			iPoint mouseOnScreen = App->events->GetMouseOnScreen();
			iPoint mouseOnWorld = App->events->GetMouseOnWorld();
			screenMouse->SetText(C_String("Screen: %i, %i", mouseOnScreen.x, mouseOnScreen.y));
			globalMouse->SetText(C_String("World: %i, %i", mouseOnWorld.x, mouseOnWorld.y));

			iPoint tile = App->pathFinding->WorldToMap(mouseOnWorld.x, mouseOnWorld.y);
			tileMouse->SetText(C_String("Logic Tile: %i, %i", tile.x, tile.y));
		}
	}
	
	//Render current tile
	iPoint p = App->pathFinding->MapToWorld(currentTile_x, currentTile_y);
	//currentTileSprite.position.x = p.x;
	//currentTileSprite.position.y = p.y;
	//App->render->AddSprite(&currentTileSprite, GUI);


	//UI WEIRD STUFF -------------------------------------
	//Update resources display

	char it_res_c[9];
	UpdateDisplayedResources(it_res_c);
	UpdateDisplayedPsiReached(dt, it_res_c);

	//Update Production Queue
	panel_queue->UpdateQueue();
	
#pragma region Victory_Conditions
	if (gameFinished == false)
	{
		if (App->GetFrameCount() % 120 == 0)
		{
			if (defeat == false && victory == false)
			{
				if (zergSample->state == BS_DEAD)
				{
					defeat = true;
					App->render->MoveCamera(400, 4800);
				}
				if (App->IA->bossDefeated == true)
				{
					victory = true;
				}
			}
			else
			{
				useConditions();
			}

		}
	}
#pragma endregion

	
	
	//Update Minimap rect
	if (App->IA->bossPhase == false)
	{
		App->fogOfWar->DrawCircle(2681, 464, 200);
	}

	App->map->Draw();
	App->fogOfWar->Draw();

	return true;
}

// Called each loop iteration
bool S_SceneMap::PostUpdate(float dt)
{
	bool ret = true;
	if (gameFinished)
	{
		if (App->events->GetEvent(E_LEFT_CLICK) == EVENT_DOWN)
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
	
	App->gui->SetCurrentGrid(G_NONE);
	App->entityManager->UnselectAllUnits();

	//Free textures (Should be done with a private list)
	App->tex->UnLoad(uiIconsT);
	App->tex->UnLoad(orderIconsT);
	App->tex->UnLoad(atlasT);
	App->tex->UnLoad(controlPT);
	App->tex->UnLoad(uiWireframesT);

	App->tex->UnLoad(victoryT);
	App->tex->UnLoad(defeatT);
	App->tex->UnLoad(debug_tex);
	App->tex->UnLoad(quit_tex);
	

	App->tex->UnLoad(spawnSplash.texture);
	App->tex->UnLoad(bloodSplash.texture);
	
	//Delete all unit elements
	App->gui->DeleteUIElement(screenMouse);
	App->gui->DeleteUIElement(globalMouse);
	App->gui->DeleteUIElement(tileMouse);

	App->gui->DeleteUIElement(controlPanel);
	App->gui->DeleteUIElement(finalScreen);
	App->gui->DeleteUIElement(yes_label);
	App->gui->DeleteUIElement(no_label);
	App->gui->DeleteUIElement(quit_image);
	App->gui->DeleteUIElement(quit_label);
	App->gui->DeleteUIElement(intro_text);
	

	for (uint i = 0; i < 3; i++)
	{
		App->gui->DeleteUIElement(res_img[i]);
		App->gui->DeleteUIElement(res_lab[i]);
	}

	//Release all grids and the coords class
	RELEASE(coords);
	//Delete panels
	RELEASE(statsPanel_m);
	RELEASE(panel_queue);
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
	App->explosion->Disable();
	App->particles->Disable();
	App->fogOfWar->Disable();
	App->minimap->Disable();

	return true;
}


// Load Game State
bool S_SceneMap::Load(pugi::xml_node& data)
{
	onEvent = false;
	panel_queue->disableQueue();
	for (std::vector<Building>::iterator it = App->entityManager->buildingList.begin(); it != App->entityManager->buildingList.end(); it++)
	{
		if (it->GetType() == ZERG_SAMPLE)
		{
			zergSample = &(*it);
			break;
		}
	}

	return true;
}

// Save Game State
bool S_SceneMap::Save(pugi::xml_node& data) const
{

	return true;
}

void S_SceneMap::ManageInput(float dt)
{
	if (App->events->IsInputFrozen() == false)
	{

		if (onEvent == false && App->render->movingCamera == false)
		{
			if (App->events->GetEvent(E_CAMERA_UP) == EVENT_REPEAT)
				App->render->camera.y -= (int)floor(CAMERA_SPEED * dt);

			if (App->events->GetEvent(E_CAMERA_DOWN) == EVENT_REPEAT)
				App->render->camera.y += (int)floor(CAMERA_SPEED * dt);

			if (App->events->GetEvent(E_CAMERA_LEFT) == EVENT_REPEAT)
				App->render->camera.x -= (int)floor(CAMERA_SPEED * dt);

			if (App->events->GetEvent(E_CAMERA_RIGHT) == EVENT_REPEAT)
				App->render->camera.x += (int)floor(CAMERA_SPEED * dt);
		}

		//Enable / Disable map render
		if (App->events->GetEvent(E_DEBUG_ENTITY_MANAGER) == EVENT_DOWN)
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

		if (App->events->GetEvent(E_DEBUG_UI) == EVENT_DOWN)
			App->gui->debug = !App->gui->debug;

		if (App->events->GetEvent(E_DEBUG_PATHFINDING) == EVENT_DOWN)
			App->pathFinding->displayPath = !App->pathFinding->displayPath;

		if (App->events->GetEvent(E_DEBUG_FOW) == EVENT_DOWN)
			App->fogOfWar->globalVision = !App->fogOfWar->globalVision;

		if (App->events->GetEvent(E_DEBUG_EXPLOSIONS) == EVENT_DOWN)
			App->explosion->debug = !App->explosion->debug;

		if (true)//App->entityManager->debug)
		{
			UnitCreationInput();

			if (App->events->GetEvent(E_DEBUG_ADD_MINERAL) == EVENT_DOWN)
			{
				App->player->AddMineral(1000);
			}
			if (App->events->GetEvent(E_DEBUG_ADD_GAS) == EVENT_DOWN)
			{
				App->player->AddGas(1000);
			}
			if (App->events->GetEvent(E_DEBUG_ADD_PSI) == EVENT_DOWN)
			{
				App->player->AddMaxPsi(100);
			}

			if (App->events->GetEvent(E_DEBUG_WIN) == EVENT_DOWN)
				victory = true;
			if (App->events->GetEvent(E_DEBUG_LOOSE) == EVENT_DOWN)
				defeat = true;

			if (App->events->GetEvent(E_DEBUG_ZOOM_OUT) == EVENT_DOWN)
			{
				if (App->events->GetScale() > 1)
				{
					App->events->SetScale(App->events->GetScale() - 1);
					App->render->camera.x /= 2;
					App->render->camera.y /= 2;

				}
			}

			if (App->events->GetEvent(E_DEBUG_ZOOM_IN) == EVENT_DOWN)
			{
				App->events->SetScale(App->events->GetScale() + 1);
				App->render->camera.x *= 2;
				App->render->camera.y *= 2;
			}

			if (App->events->GetEvent(E_DEBUG_ADD_EXPLOSION) == EVENT_DOWN)
			{
				App->explosion->AddExplosion(App->events->GetMouseOnWorld(), 150, 1000, 1.0f, 1, CINEMATIC);
			}
			if (App->events->GetEvent(E_DEBUG_ADD_EXPLOSION_SYSTEM1) == EVENT_DOWN)
			{
				App->explosion->AddSystem(App->explosion->testingSystem, App->events->GetMouseOnWorld());
			}
			if (App->events->GetEvent(E_DEBUG_ADD_EXPLOSION_SYSTEM2) == EVENT_DOWN)
			{
				App->explosion->AddSystem(App->explosion->testingSystem2, App->events->GetMouseOnWorld());
			}
			if (App->events->GetEvent(E_DEBUG_ADD_EXPLOSION_SYSTEM3) == EVENT_DOWN)
			{
				App->explosion->AddSystem(App->explosion->spinSystem, App->events->GetMouseOnWorld());
			}
			if (App->events->GetEvent(E_DEBUG_ADD_EXPLOSION_SYSTEM4) == EVENT_DOWN)
			{
				App->explosion->AddSystem(App->explosion->crossSystem, App->events->GetMouseOnWorld());
			}
			if (App->events->GetEvent(E_SPAWN_NEXUS) == EVENT_DOWN)
			{
				Building* building = App->entityManager->CreateBuilding(currentTile_x, currentTile_y, NEXUS, PLAYER);
				if (building)
				{
					building->FinishSpawn();
				}
			}
		}
	}

	//UI WEIRD STUFF -----------------------------------------------------
		//Change Grids
		/*if (App->input->GetKey(SDL_SCANCODE_KP_0) == KEY_DOWN)
		{
			panel_queue->disableQueue();
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_DOWN)
		{
			panel_queue->addSlot(PROBE);
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_DOWN)
		{
			panel_queue->addSlot(DRAGOON);
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_3) == KEY_DOWN)
		{
			panel_queue->addSlot(ZEALOT);
		}*/
		
		if (onEvent == false && App->render->movingCamera == false)
		{
			int x = 0, y = 0;
			x = App->events->GetMouseOnScreen().x;
			y = App->events->GetMouseOnScreen().y;
			bool movingLeft = false, movingRight = false, movingUp = false, movingDown = false;

			if (y < 5)
			{
				if (App->render->camera.y > 0)
				{
					if (!App->entityManager->startSelection)
					{
						App->render->camera.y -= (int)floor(CAMERA_SPEED * dt);
					}
					movingUp = true;
				}
			}
			if (y > App->render->camera.h / App->events->GetScale() - 5)
			{
				if (App->render->camera.y < 2700 * App->events->GetScale())
				{
					if (!App->entityManager->startSelection)
					{
						App->render->camera.y += (int)floor(CAMERA_SPEED * dt);
					}
					movingDown = true;
				}
			}
			if (x < 5)
			{
				if (App->render->camera.x > 0)
				{
					if (!App->entityManager->startSelection)
					{
						App->render->camera.x -= (int)floor(CAMERA_SPEED * dt);
					}
					movingLeft = true;
				}
			}
			if (x > App->render->camera.w / App->events->GetScale() - 5)
			{
				if (App->render->camera.x < 2433 * App->events->GetScale())
				{
					if (!App->entityManager->startSelection)
					{
						App->render->camera.x += (int)floor(CAMERA_SPEED * dt);
					}
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
		
		if (App->events->GetEvent(E_OPEN_MENU) == EVENT_DOWN && !onEvent) 
		{
			quit_image->SetActive(!quit_image->IsActive());
		}

	//---------------------------------------------------------------------

}

void S_SceneMap::UnitCreationInput()
{
	if (App->events->GetEvent(E_MID_CLICK) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, PROBE, PLAYER);
	}
	if (App->events->GetEvent(E_SPAWN_CARRIER) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, CARRIER, PLAYER);
	}
	if (App->events->GetEvent(E_SPAWN_SHUTTLE) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, SHUTTLE, PLAYER);
	}
	if (App->events->GetEvent(E_SPAWN_ZEALOT) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, ZEALOT, PLAYER);
	}

	if (App->events->GetEvent(E_SPAWN_DRAGOON) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, DRAGOON, PLAYER);
	}

	if (App->events->GetEvent(E_SPAWN_SCOUT) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, SCOUT, PLAYER);
	}

	if (App->events->GetEvent(E_SPAWN_REAVER) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, REAVER, PLAYER);
	}

	if (App->events->GetEvent(E_SPAWN_OBSERVER) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, OBSERVER, PLAYER);
	}

	if (App->events->GetEvent(E_SPAWN_HIGH_TEMPLAR) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, HIGH_TEMPLAR, PLAYER);
	}

	if (App->events->GetEvent(E_SPAWN_DARK_TEMPLAR) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, DARK_TEMPLAR, PLAYER);
	}

	if (App->events->GetEvent(E_SPAWN_ZERGLING) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, ZERGLING, COMPUTER);
	}

	if (App->events->GetEvent(E_SPAWN_MUTALISK) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, MUTALISK, COMPUTER);
	}

	if (App->events->GetEvent(E_SPAWN_HYDRALISK) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, HYDRALISK, COMPUTER);
	}

	if (App->events->GetEvent(E_SPAWN_KERRIGAN) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, KERRIGAN, COMPUTER);
	}

	if (App->events->GetEvent(E_SPAWN_INFESTED_TERRAN) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, INFESTED_TERRAN, COMPUTER);
	}

	if (App->events->GetEvent(E_SPAWN_ULTRALISK) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, ULTRALISK, COMPUTER);
	}
	if (App->events->GetEvent(E_SPAWN_GODMODE) == EVENT_DOWN)
	{
		unit = App->entityManager->CreateUnit(App->events->GetMouseOnWorld().x, App->events->GetMouseOnWorld().y, GODMODE, PLAYER);
	}
	if (App->events->GetEvent(E_OPEN_MENU) == EVENT_DOWN)
	{
		interruptEvent = true;
	}
}

void S_SceneMap::LoadTextures()
{
	controlPT = App->tex->Load("graphics/gui/pconsole.png");
	orderIconsT = App->tex->Load("graphics/gui/cmdicons.png");
	atlasT = App->tex->Load("graphics/gui/pcmdbtns.png");
	uiIconsT = App->tex->Load("graphics/gui/icons.png");
	uiWireframesT = App->tex->Load("graphics/gui/Wireframes.png");
	queue_backgroundT = App->tex->Load("graphics/gui/UI_Queue.png");

	//Orders hover textures
	buildings_hover = App->tex->Load("graphics/ui/Hover_Texts/Buildings.png");
	units_hover = App->tex->Load("graphics/ui/Hover_Texts/Units.png");


	orderAttack_hover = App->tex->Load("graphics/ui/Hover_Texts/order_attack.png");
	orderCancel_hover = App->tex->Load("graphics/ui/Hover_Texts/order_cancel.png");
	orderGather_hover = App->tex->Load("graphics/ui/Hover_Texts/order_gather.png");
	orderMove_hover = App->tex->Load("graphics/ui/Hover_Texts/order_move.png");
	orderRallypoint_hover = App->tex->Load("graphics/ui/Hover_Texts/order_rallyPoint.png");
	orderReturnCargo_hover = App->tex->Load("graphics/ui/Hover_Texts/order_returnCargo.png");
	orderStop_hover = App->tex->Load("graphics/ui/Hover_Texts/order_stop.png");
	orderStructure_hover = App->tex->Load("graphics/ui/Hover_Texts/order_structure.png");
	orderHold_hover = App->tex->Load("graphics/ui/Hover_Texts/orderHold_hover.png");
	orderPatrol_hover = App->tex->Load("graphics/ui/Hover_Texts/orderPatrol_hover.png");

	//Progress Bar
	progressBar_back = App->tex->Load("graphics/ui/hpbarempt.png");
	progressBar_bar = App->tex->Load("graphics/ui/hpbarfull.png");
	//Quit texture
	quit_tex = App->tex->Load("graphics/ui/readyt/pdpopup.png");

	

}

void S_SceneMap::LoadGUI()
{
	//UI WEIRD STUFF----------------------------------
#pragma region Misc
	int w, h, scale;
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;
	scale = App->events->GetScale();
	int use_w = w / scale;
	int use_h = h / scale;


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

	controlPanel = App->gui->CreateUI_Image({ 0,use_h * 0.628f, use_w, use_h * 0.372916666f }, controlPT, { 0, 0, 0, 0 }, { 0, 60, 640, 118 });
	controlPanel->SetLayer(1);

#pragma endregion

	//TMP CREATING ALL BUILDINGS && UNITS
	/*Building* building = NULL;
	for (int n = 0; n <= 20; n++)
	{
		building = App->entityManager->CreateBuilding(3 + 9 * (n % 10), 45 + 7 * (n / 10), static_cast<Building_Type>(n), PLAYER, true);
		if (building)
			building->state = BS_DEFAULT;
	}
	for (int n = 0; n <= 14; n++)
	{
		App->entityManager->CreateUnit(1230 + 80 * (n % 5), 250 + 80 * (n / 5), static_cast<Unit_Type>(n), PLAYER);
	}*/

//Load Icon rects

	ui_unit_sections.insert(std::make_pair<Unit_Type, SDL_Rect&>(PROBE, SDL_Rect{ 468, 102, 32, 32 }));
	ui_unit_sections.insert(std::make_pair<Unit_Type, SDL_Rect&>(ZEALOT, SDL_Rect{ 324, 136, 32, 32 }));
	ui_unit_sections.insert(std::make_pair<Unit_Type, SDL_Rect&>(DRAGOON, SDL_Rect{ 360, 136, 32, 32 }));
	ui_unit_sections.insert(std::make_pair<Unit_Type, SDL_Rect&>(HIGH_TEMPLAR, SDL_Rect{ 396, 136, 32, 32 }));
	ui_unit_sections.insert(std::make_pair<Unit_Type, SDL_Rect&>(DARK_TEMPLAR, SDL_Rect{ 252, 136, 32, 32 }));
	ui_unit_sections.insert(std::make_pair<Unit_Type, SDL_Rect&>(SCOUT, SDL_Rect{ 72, 136, 32, 32 }));
	ui_unit_sections.insert(std::make_pair<Unit_Type, SDL_Rect&>(REAVER, SDL_Rect{ 468, 136, 32, 32 }));
	ui_unit_sections.insert(std::make_pair<Unit_Type, SDL_Rect&>(OBSERVER, SDL_Rect{ 576, 136, 32, 32 }));
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

#pragma region Production Panel
	
	float div_w = 640.0f;
	float div_h = 480.0f;
	//First 0.382813f % 0.8416666f %
	//Others 0.38125f % 0.9229166f %
	panel_queue = new UI_Panel_Queue();
	panel_queue->icon_rects = &ui_unit_sections;
	panel_queue->background = App->gui->CreateUI_Image({ use_w * (242.0f / div_w), use_h * (401.0f / div_h), use_w*(154.0f / div_w), use_h*(75.0f / div_h) }, queue_backgroundT, { 0, 0, 0, 0 });
	panel_queue->background->SetLayer(1);
	
	panel_queue->progress_background = App->gui->CreateUI_Image({ use_w * (282.0f / div_w), use_h *(427.0f / div_h), 0, 0 }, progressBar_back, { 0, 0, 0, 0 });
	panel_queue->progress_background->SetLayer(1);

	panel_queue->progress_bar = App->gui->CreateUI_ProgressBar_F({ use_w * (282.0f / div_w), use_h *(427.0f / div_h), 0, 0 }, progressBar_bar, &panel_queue->bar_max, &panel_queue->bar_current);
	panel_queue->progress_background->SetActive(false);
	panel_queue->progress_bar->SetActive(false);
	panel_queue->progress_bar->SetLayer(1);
	//396 39
	//Diff 244, 443| 283, 404
	float x_q = 0.38125f - 0.060938f;
	float y_q = 0.9229166f;
	for (int i = 0; i < 5; i++)
	{
		panel_queue->icons[i] = App->gui->CreateUI_Image({ use_w * x_q, use_h * y_q, use_w*(32 / div_w), use_h*(32 / div_h) }, orderIconsT, { 469, 345, 32, 32 });
		panel_queue->icons[i]->SetLayer(2);

		panel_queue->icons[i]->SetActive(false);

		panel_queue->icons[i]->AddListener(this);
		x_q += 0.060938f;
	}
	panel_queue->icons[0]->localPosition.x = use_w *0.38125f;
	panel_queue->icons[0]->localPosition.y = use_h *0.8416666f;
	panel_queue->background->SetActive(false);
	
#pragma endregion

#pragma region Stats Panel Multiple

	statsPanel_m = new Stats_Panel_Mult();

	float yF_m = 0.827f, xF_m = 0.264f;
	//Row elements
	int r_e = 6;
	//26,4% 82,7%
	//32.0f% 90.3f%
	for (uint j = 0; j < 2; j++)
	{
		for (uint i = 0; i < r_e; i++)
		{
			uint index = i + (j * r_e);
			statsPanel_m->unitSelect_frames[index] = App->gui->CreateUI_Image({ (use_w * xF_m), (use_h * yF_m), use_w*(33 / div_w), use_h*(34 / div_h) }, atlasT, { 936, 0, 33, 34 });
			statsPanel_m->unitSelect_frames[index]->SetLayer(1);

			statsPanel_m->unitSelect_wires[index].wireframe = App->gui->CreateUI_Image({ 1, 1, use_w*(31 / div_w), use_h*(32 / div_h) }, uiWireframesT, { 0, 0, 31, 32 });
			statsPanel_m->unitSelect_wires[index].wireframe->SetLayer(2);
			statsPanel_m->unitSelect_wires[index].wireframe->AddListener(this);
			statsPanel_m->unitSelect_wires[index].wireframe->SetParent(statsPanel_m->unitSelect_frames[index]);	
			
			xF_m += 0.056;
		}
		xF_m = 0.264f;
		yF_m += 0.076f;
	}
//Load Icon Rects
	statsPanel_m->unitWireframe_rects.insert(std::make_pair<Unit_Type, SDL_Rect>(PROBE, { 4, 91, 32, 32 }));
	statsPanel_m->unitWireframe_rects.insert(std::make_pair<Unit_Type, SDL_Rect>(ZEALOT, { 44, 90, 31, 32 }));
	statsPanel_m->unitWireframe_rects.insert(std::make_pair<Unit_Type, SDL_Rect>(DRAGOON, { 86, 90, 24, 32 }));
	statsPanel_m->unitWireframe_rects.insert(std::make_pair<Unit_Type, SDL_Rect>(SCOUT, { 394, 90, 32, 31 }));
	statsPanel_m->unitWireframe_rects.insert(std::make_pair<Unit_Type, SDL_Rect>(REAVER, { 355, 90, 32, 32 }));
	statsPanel_m->unitWireframe_rects.insert(std::make_pair<Unit_Type, SDL_Rect>(OBSERVER, { 315, 90, 32, 32 }));
	statsPanel_m->unitWireframe_rects.insert(std::make_pair<Unit_Type, SDL_Rect>(HIGH_TEMPLAR, { 122, 91, 32, 32 }));
	statsPanel_m->unitWireframe_rects.insert(std::make_pair<Unit_Type, SDL_Rect>(DARK_TEMPLAR, { 198, 91, 32, 32 }));

	statsPanel_m->setSelectNone();
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

//  nexus -----------
	Grid3x3* nexus = new Grid3x3(*coords, G_NEXUS);
	grids.push_back(nexus);
	gridTypes.push_back(nexus->type);

	float width_frame = use_w*(33/div_w);
	float height_frame = use_h*(34/div_h);

	//------------
	//Create probe button
	butt_it = nexus->setOrder(App->entityManager->o_GenProbe_toss, idle, clicked, 0, 0, *atlasT);
	//butt_it->localPosition.w = width_frame;
	//butt_it->localPosition.h = height_frame;
	//Hovering image
	int y = 62;
	h = 62;
	image_it = App->gui->CreateUI_Image({w/2 - 195, 300 - h, 0, 0 }, units_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0,0 }, orderIconsT, { 468, 102, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	//------------
	//Rally point button
	butt_it = nexus->setOrder(App->entityManager->o_Set_rallyPoint, idle, clicked, 1, 2, *atlasT,width_frame,height_frame);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ -37, -13, 0, 0 }, orderRallypoint_hover, { 0, 0, 77, 13 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image(SDL_Rect{ 3, 3, width_frame, 0 }, orderIconsT, { 504, 544, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	nexus->changeState(false);

//  basic_u -----------

	Grid3x3* basic_u = new Grid3x3(*coords,G_BASIC_UNIT);

	grids.push_back(basic_u);
	gridTypes.push_back(basic_u->type);

	//gui->SetCurrentGrid(basic_u);

	butt_it = basic_u->setOrder(App->entityManager->o_Move, idle, clicked, 0, 0, *atlasT, width_frame, height_frame);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderMove_hover, { 0, 0, 29, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 3, 3, width_frame, 0 }, orderIconsT, { 252, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------
	butt_it = basic_u->setOrder(App->entityManager->o_Stop, idle, clicked, 0, 1, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -13, 0, 0 }, orderStop_hover, { 0, 0, 26, 13 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 3, 3, width_frame, 0 }, orderIconsT, { 288, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------
	butt_it = basic_u->setOrder(App->entityManager->o_Attack, idle, clicked, 0, 2, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderAttack_hover, { 0, 0, 36, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

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
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 4, 1, 0, 0 }, orderIconsT, { 576, 474, 26, 29 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	image_it->sprite.tint = {90,90,90,255};

	butt_it->son = image_it;


	//------------

	butt_it = basic_u->setOrder(App->entityManager->o_Hold_pos, idle, clicked, 1, 1, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderHold_hover, { 0, 0, 69, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 3, 2, 0, 0 }, orderIconsT, { 0, 509, 27, 29 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	image_it->sprite.tint.g = image_it->sprite.tint.b =	image_it->sprite.tint.r = 90;

	butt_it->son = image_it;

	basic_u->changeState(false);
// basicBuildings ------------
	Grid3x3* basicBuildings = new Grid3x3(*coords, G_BASIC_BUILDINGS);

	grids.push_back(basicBuildings);
	gridTypes.push_back(basicBuildings->type);

	// Nexus { 108, 304, 32, 32 }
	// Pylon  { 36, 304, 32, 32 }

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Pylon, idle, clicked, 0, 1, *atlasT,width_frame,height_frame);
	
	//Hovering image
	y = 62;
	h = 76;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, buildings_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 108, 304, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Nexus, idle, clicked, 0, 0, *atlasT,width_frame,height_frame);

	//Hovering image
	y = 0;
	h = 63;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, buildings_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 36, 304, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Gateaway, idle, clicked, 1, 0, *atlasT,width_frame,height_frame);

	//Hovering image
	y = 200;
	h = 62;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, buildings_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 252, 306, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Assimilator, idle, clicked, 0, 2, *atlasT,width_frame,height_frame);

	//Hovering image
	//Hovering image
	y = 137;
	h = 63;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, buildings_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 144, 306, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	butt_it = basicBuildings->setOrder(App->entityManager->o_Return_Builds_Menu, idle, clicked, 2,2, *atlasT,width_frame,height_frame);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ -28, -11, 0, 0 }, orderCancel_hover, { 0, 0, 69, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);

	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 3, 4, 0, 0 }, orderIconsT, { 540, 442, 26, 26 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	
	//Cybernetics
	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Cybernetics, idle, clicked, 1, 1, *atlasT,width_frame,height_frame);

	//Hovering image
	y = 262;
	h = 62;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, buildings_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 396, 306, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	//Robotics_Bay
	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Robotics_Support_Bay, idle, clicked, 2, 0, *atlasT, width_frame, height_frame);

	//Hovering image
	y = 662;
	h = 73;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, buildings_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 36, 340, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(CYBERNETICS_CORE);

	//Templar archives
	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Templar_Archives, idle, clicked, 2, 1, *atlasT, width_frame, height_frame);

	//Hovering image
	y = 600;
	h = 62;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, buildings_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 432, 306, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(ROBOTICS_BAY);

	//Photon Cannon
	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Photon, idle, clicked, 1, 2, *atlasT, width_frame, height_frame);

	//Hovering image
	y = 324;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, buildings_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 324, 305, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(CYBERNETICS_CORE);

	basicBuildings->changeState(false);
	

//  probeMenu -----------

	Grid3x3* probeMenu = new Grid3x3(*coords, G_PROBE);
	grids.push_back(probeMenu);
	gridTypes.push_back(probeMenu->type);

//Copy the buttons from the basic unit -------------
	//I know this is like super bad but there where memory managment issues
	//So this is a temporary solution

// o_Move ------------
	butt_it = probeMenu->setOrder(App->entityManager->o_Move, idle, clicked, 0, 0, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderMove_hover, { 0, 0, 29, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 252, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	// o_Stop 
	butt_it = probeMenu->setOrder(App->entityManager->o_Stop, idle, clicked, 0, 1, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -13, 0, 0 }, orderStop_hover, { 0, 0, 29, 13 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 288, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	// o_Attack 
	butt_it = probeMenu->setOrder(App->entityManager->o_Attack, idle, clicked, 0, 2, *atlasT, true);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderAttack_hover, { 0, 0, 36, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 324, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	// o_Gather 
	butt_it = probeMenu->setOrder(App->entityManager->o_Gather, idle, clicked, 1, 1, *atlasT,width_frame,height_frame);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderGather_hover, { 0, 0, 37, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 3, 5, 0, 0 }, orderIconsT, { 360, 442, 28, 25 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);
	image_it->sprite.tint.g = image_it->sprite.tint.b = image_it->sprite.tint.r = 90;

	butt_it->son = image_it;
	// o_Ret_Cargo 
	butt_it = probeMenu->setOrder(App->entityManager->o_Ret_Cargo, idle, clicked, 1, 2, *atlasT,width_frame,height_frame);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ -28, -13, 0, 0 }, orderReturnCargo_hover, { 0, 0, 68, 13 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 0, 1, 0, 0 }, orderIconsT, { 429, 440, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);
	image_it->sprite.tint.g = image_it->sprite.tint.b = image_it->sprite.tint.r = 90;

	butt_it->son = image_it;
	// o_Basic_Builds 
	butt_it = probeMenu->setOrder(App->entityManager->o_Basic_Builds, idle, clicked, 2,0, *atlasT,width_frame,height_frame);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderStructure_hover, { 0, 0, 79, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 3, 5, 0, 0 }, orderIconsT, { 0, 544, 29, 24 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	probeMenu->changeState(false);

//  Gateways -----------
	Grid3x3* gateways = new Grid3x3(*coords, G_GATEWAY);
	grids.push_back(gateways);
	gridTypes.push_back(gateways->type);
	//o_Gen_Zealot
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Zealot, idle, clicked, 0, 0, *atlasT,width_frame,height_frame);

	//Hovering image
	y = 239;
	h = 50;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, units_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, ui_unit_sections[ZEALOT]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	//o_Gen_Dragoon
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Dragoon, idle, clicked, 0, 1, *atlasT,width_frame,height_frame);

	//Hovering image
	y = 425;
	h = 63;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, units_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, ui_unit_sections[DRAGOON]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(CYBERNETICS_CORE);

	//o_Gen_D_Templar
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Dark_Templar, idle, clicked, 1, 0, *atlasT,width_frame,height_frame);

	//Hovering
	y = 362;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, units_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, ui_unit_sections[DARK_TEMPLAR]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(ROBOTICS_BAY);

	//o_Gen_Scout
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Scout, idle, clicked, 2, 1, *atlasT, width_frame, height_frame);

	y = 124;
	h = 54;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, units_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, ui_unit_sections[SCOUT]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it->InitRequiredBuilding(ROBOTICS_BAY);

	//o_Gen_Reaver
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Reaver, idle, clicked, 0, 2, *atlasT, width_frame, height_frame);

	//Hovering
	y = 177;
	h = 63;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, units_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, ui_unit_sections[REAVER]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(TEMPLAR_ARCHIVES);

	//o_Gen_Observer
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Observer, idle, clicked, 2, 0, *atlasT, width_frame, height_frame);

	//Hovering
	y = 0;
	h = 63;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, units_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, ui_unit_sections[OBSERVER]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it->InitRequiredBuilding(CYBERNETICS_CORE);

	//o_Gen_High_Templar
	butt_it = gateways->setOrder(App->entityManager->o_Gen_High_Templar, idle, clicked, 1, 1, *atlasT, width_frame, height_frame);

	//Hovering
	y = 288;
	h = 74;
	image_it = App->gui->CreateUI_Image({ w / 2 - 195, 300 - h, 0, 0 }, units_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, ui_unit_sections[HIGH_TEMPLAR]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(TEMPLAR_ARCHIVES);

	gateways->changeState(false);

	//o_Set_rallyPoint
	butt_it = gateways->setOrder(App->entityManager->o_Set_rallyPoint, idle, clicked, 1, 2, *atlasT,width_frame,height_frame);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ -37, -13, 0, 0 }, orderRallypoint_hover, { 0, 0, 77, 13 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, orderIconsT, { 504, 544, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	nexus->changeState(false);

	gateways->changeState(false);

	
#pragma endregion
}


void S_SceneMap::OnGUI(GUI_EVENTS event, UI_Element* element)
{
	if (element == yes_label && event == UI_MOUSE_DOWN)
	{
		quit_image->SetActive(false);	
		App->changeScene(App->sceneMenu, this);
	}

	if (element == no_label && event == UI_MOUSE_DOWN)
	{
		quit_image->SetActive(false);
	}

	if (event == UI_MOUSE_DOWN)
	{
		//Production Queue
		for (uint i = 0; i < QUEUE_SLOTS; i++)
		{
			if (element == panel_queue->icons[i])
			{
				App->entityManager->selectedBuilding->RemoveFromQueue(i);
				panel_queue->removeSlot(i);
				break;
			}
		}
		//Multiple selection
		for (uint i2 = 0; i2 < MAX_UNITS_M; i2++)
		{
			if (element == statsPanel_m->unitSelect_wires[i2].wireframe)
			{
				App->entityManager->UnselectUnit((Unit*)statsPanel_m->unitSelect_wires[i2].unit);
				break;
			}
		}
	}
	if (gameFinished)
	{
		if (event == UI_MOUSE_DOWN && element == finalScreen)
		{
			App->changeScene(App->sceneMenu, this);
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

	App->entityManager->CreateResource(17, 161, MINERAL);
	App->entityManager->CreateResource(10, 165, MINERAL);
	App->entityManager->CreateResource(8, 171, MINERAL);
	App->entityManager->CreateResource(7, 174, MINERAL);

	App->entityManager->CreateResource(18, 178, GAS);

	//Mid colonization zone
	App->entityManager->CreateResource(107, 121, MINERAL);
	App->entityManager->CreateResource(112, 124, MINERAL);
	App->entityManager->CreateResource(116, 126, MINERAL);
	App->entityManager->CreateResource(107, 125, MINERAL);
	App->entityManager->CreateResource(100, 132, GAS);

	//Zerg base
	App->entityManager->CreateResource(152, 4, MINERAL);
	App->entityManager->CreateResource(160, 8, MINERAL);
	App->entityManager->CreateResource(156, 11, MINERAL);
	App->entityManager->CreateResource(167, 18, MINERAL);
	App->entityManager->CreateResource(174, 20, MINERAL);
	App->entityManager->CreateResource(172, 42, GAS);
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
	zergSample = App->entityManager->CreateBuilding(29, 159, ZERG_SAMPLE, PLAYER);

	Building* building = NULL;
	building = App->entityManager->CreateBuilding(26, 168, NEXUS, PLAYER);
	building->FinishSpawn();
	building = App->entityManager->CreateBuilding(42, 170, PYLON, PLAYER);
	building->FinishSpawn();

	App->player->stats.maxPsi = App->player->stats.realMaxPsi = 12;
	App->player->stats.mineral = 100;
	App->player->stats.gas = 100;
}

void S_SceneMap::FirstEventScript()
{ 
	float time = scriptTimer.ReadSec();

	// Set Up for Script
	// Camera on Main Zerg Base
	App->fogOfWar->DrawCircle(320, 2747, 300);
	if (!action_aux && scriptTimer.IsStopped())
	{
		scriptTimer.Start();

		App->render->camera.x = 4775;
		App->render->camera.y = 600;

		action_aux = true;
	}

	// IF INTERRUPT
	if (interruptEvent)
	{
		intro_text->SetActive(false);
		App->entityManager->muteUnitsSounds = true;
		if (time < (30.0f * 3.0f / 4.0f))
		{

			App->entityManager->CreateUnit(625, 2560, DRAGOON, PLAYER);
			App->player->AddPsi(2);
			if (time < (27.5f * 3.0f / 4.0f))
			{
				App->entityManager->CreateUnit(580, 2570, ZEALOT, PLAYER);
				App->player->AddPsi(2);
				if (time < (26.0f * 3.0f / 4.0f))
				{
					App->entityManager->CreateUnit(300, 2647, PROBE, PLAYER);
					App->player->AddPsi(1);
					if (time < (25.0f * 3.0f / 4.0f))
					{
						App->entityManager->CreateUnit(615, 2605, ZEALOT, PLAYER);
						App->player->AddPsi(2);
						if (time < (23.5f * 3.0f / 4.0f))
						{
							App->entityManager->CreateUnit(339, 2694, PROBE, PLAYER);
							App->player->AddPsi(1);
							if (time < (21.0f * 3.0f / 4.0f))
							{
								App->entityManager->CreateUnit(320, 2747, PROBE, PLAYER);
								App->player->AddPsi(1);
								if (time < (18.0f * 3.0f / 4.0f))
								{
									scripted_zergling->Hit(100);
								}
							}
						}
					}
				}
			}
		}
		App->entityManager->muteUnitsSounds = false;
	}
	
	// First Time Line
	if (time >= (1.0f * 3.0f / 4.0f))
	{
		intro_text->SetActive(true);
		if (!action && time < (1.2f * 3.0f / 4.0f))
		{
			App->audio->PlayFx(sfx_script_beep);
			action = true;
		}
	}
	if (time >= (1.1f * 3.0f / 4.0f) && action && time < (1.2f * 3.0f / 4.0f))
	{
		App->audio->PlayFx(sfx_script_beep);
		action = false;
	}
	// Create All the Cinematic Units
	if (time >= (2.5f * 3.0f / 4.0f) && !action && time < (3.0f * 3.0f / 4.0f))
	{
		scripted_unit1 = App->entityManager->CreateUnit(10, 3000, CARRIER, CINEMATIC);
		scripted_unit2 = App->entityManager->CreateUnit(200, 3030, SCOUT_CIN, CINEMATIC);
		scripted_unit3 = App->entityManager->CreateUnit(65, 2880, SCOUT_CIN, CINEMATIC);
		scripted_unit4 = App->entityManager->CreateUnit(25, 2715, SHUTTLE, CINEMATIC);
		scripted_unit5 = App->entityManager->CreateUnit(60, 2740, SCOUT_CIN, CINEMATIC);

		// Zergling Appears and Attacks Nexus
		scripted_zergling = App->entityManager->CreateUnit(500, 2800, ZERGLING, COMPUTER);

		// "Balance" Zerg to get rekt 420 nonscope
		scripted_zergling->stats.attackDmg = 1;
		scripted_zergling->SetHP(1);

		scripted_shuttle1 = App->entityManager->CreateUnit(17, 2925, SHUTTLE, CINEMATIC);
		scripted_shuttle2 = App->entityManager->CreateUnit(105, 3005, SHUTTLE, CINEMATIC);

		action = true;
	}
	// Camera Movement Transition from Main Zerg Base to Protoss Base
	else if (time >= (3.0f * 3.0f / 4.0f) && action && time < (3.5f * 3.0f / 4.0f))
	{
		App->render->MoveCamera(10 * App->events->GetScale() - 540, 3000 * App->events->GetScale() - 690);
		action = false;
	
	}
	// Scout Attacks Zergling
	else if (time >= (18.0f * 3.0f / 4.0f) && !action && time < (18.5f * 3.0f / 4.0f))
	{
		scripted_unit2->SetAttack(scripted_zergling);
		action = true;
	}
	// Scout continues its route
	else if (time >= (20.0f * 3.0f / 4.0f) && action && time < (20.5f * 3.0f / 4.0f))
	{
		scripted_unit2->SetTarget(600, 2820);
		action = false;
	}
	// Shuttle 1 Drops the first Probe
	else if (time >(21.0f * 3.0f / 4.0f) && !action && time < (21.5f * 3.0f / 4.0f))
	{
		App->entityManager->CreateUnit(320, 2747, PROBE, PLAYER);
		App->player->AddPsi(1);
		

		App->audio->PlayFx(sfx_shuttle_drop, 0);
		action = true;
	}
	// Shuttle 1 Drops the second Probe
	else if (time >(23.5f * 3.0f / 4.0f) && action && time < (23.9f * 3.0f / 4.0f))
	{
		App->entityManager->CreateUnit(339, 2694, PROBE, PLAYER);
		App->player->AddPsi(1);
		// Scout 2 & 3 Formation
		scripted_unit2->SetTarget(690, 2690);
		scripted_unit3->SetTarget(540, 2600);

		App->audio->PlayFx(sfx_shuttle_drop, 0);

		action = false;
	}
	// Scouts 4 & 5 Leave
	else if (time >= (24.0f * 3.0f / 4.0f) && action_aux && time < (24.5f * 3.0f / 4.0f))
	{
		scripted_unit4->SetTarget(1130, 1955);
		scripted_unit5->SetTarget(1165, 1980);
		action_aux = false;
	}
	// Shuttle 2 Drops the first Zealot
	else if (time >= (25.0f * 3.0f / 4.0f) && !action && time < (25.5f * 3.0f / 4.0f))
	{
		App->entityManager->CreateUnit(615, 2605, ZEALOT, PLAYER);
		App->player->AddPsi(2);
		// Carrier Leaves
		scripted_unit1->SetTarget(1070, 2300);

		App->audio->PlayFx(sfx_shuttle_drop, 0);
		action = true;
	}
	// Shuttle 1 Drop the third Probe
	else if (time >= (26.0f * 3.0f / 4.0f) && action && time < (26.5f * 3.0f / 4.0f))
	{
		App->entityManager->CreateUnit(300, 2647, PROBE, PLAYER);
		App->player->AddPsi(1);
		action = false;
	}
	// Shuttle 1 Leaves
	else if (time >= (27.0f * 3.0f / 4.0f) && !action && time < (27.4f * 3.0f / 4.0f))
	{
		scripted_shuttle1->SetTarget(600, 2300); // Old Coord: (17, 2925)
		action = true;
	}
	// Shuttle 2 Drops the second Zealot
	else if (time >= (27.5f * 3.0f / 4.0f) && action && time < (28.5f * 3.0f / 4.0f))
	{
		App->entityManager->CreateUnit(580, 2570, ZEALOT, PLAYER);
		App->player->AddPsi(2);
		// Scouts 2 & 3 Leave
		scripted_unit2->SetTarget(1140, 2300);
		scripted_unit3->SetTarget(1000, 2300);

		// Shuttle 1 Corrects Route
		scripted_shuttle1->SetTarget(750, 2300);

		App->audio->PlayFx(sfx_shuttle_drop, 0);
		action = false;
	}
	// Shuttle 2 Drops Last Unit (Dragoon)
	else if (time >= (30.0f * 3.0f / 4.0f) && !action && time < (31.0f * 3.0f / 4.0f))
	{
		App->entityManager->CreateUnit(625, 2560, DRAGOON, PLAYER);
		App->player->AddPsi(2);
		App->audio->PlayFx(sfx_shuttle_drop, 0);

		action = true;
	}
	// Shuttle 2 Leaves
	else if (time >= (31.5f * 3.0f / 4.0f) && time < (32.0f * 3.0f / 4.0f))
	{
		scripted_shuttle2->SetTarget(750, 2300); // Old Coord: (105, 3005)
	}
	// Shuttle 2 Corrects Route
	else if (time >= (33.0f * 3.0f / 4.0f) && time < (33.5f * 3.0f / 4.0f))
	{
		scripted_shuttle2->SetTarget(900, 2300);
	}

	// Second Timeline
	// Protoss Fleet Comes into the Base 
	if (time > (15.0f * 3.0f / 4.0f) && time < (16.0f * 3.0f / 4.0f))
	{
		scripted_unit1->SetTarget(585, 2650);
		scripted_unit2->SetTarget(600, 2820);
		scripted_unit3->SetTarget(400, 2610);

		scripted_shuttle1->SetTarget(330, 2725);
		scripted_shuttle2->SetTarget(605, 2575);
	}
	// Camera Follows Carrier
	else if (time > (19.0f * 3.0f / 4.0f) && time <= 27.0f  * 3.0f / 4.0f)
	{
		App->render->camera.x = scripted_unit1->GetPosition().x * App->events->GetScale() - 540;
		App->render->camera.y = scripted_unit1->GetPosition().y * App->events->GetScale() - 480;
	}

	// FirstEventScript - DESTRUCTOR
	if (time >= 37.0f * 3.0f / 4.0f || interruptEvent)
	{
		intro_text->SetActive(false);
		App->audio->PlayFx(sfx_script_adquire);
		App->entityManager->muteUnitsSounds = true;

		scripted_unit1->Hit(1000000);
		scripted_unit2->Hit(1000000);
		scripted_unit3->Hit(1000000);
		scripted_unit4->Hit(1000000);
		scripted_unit5->Hit(1000000);

		scripted_shuttle2->Hit(1000000);
		scripted_shuttle1->Hit(1000000);

		// Reset Variables
		scriptTimer.Stop();
		onEvent = false;
		action = action_aux = false;
		interruptEvent = false;
		App->render->camera.x = 230 * App->events->GetScale();
		App->render->camera.y = 2450 * App->events->GetScale();
		App->render->movingCamera = false;


		App->entityManager->muteUnitsSounds = false;
		LOG("Introduction Completed.");
	}
}

void S_SceneMap::SecondEventScript()
{
	// Set Up for Script
	// Camera on Main Zerg Base
	if (!action_aux)
	{
		App->entityManager->freezeInput = true;

		App->render->MoveCamera(4700, 600);

		scripted_unit1 = App->entityManager->CreateUnit(2900, 120, CARRIER, CINEMATIC);
		scripted_unit2 = App->entityManager->CreateUnit(2775, 5, SCOUT_CIN, CINEMATIC);
		scripted_unit3 = App->entityManager->CreateUnit(3060, 200, SCOUT_CIN, CINEMATIC);
		scripted_unit4 = App->entityManager->CreateUnit(2970, 5, SCOUT_CIN, CINEMATIC);
		scripted_unit5 = App->entityManager->CreateUnit(3070, 70, SCOUT_CIN, CINEMATIC);

		action_aux = true;
	}

	if (App->IA->createBoss == true && App->render->movingCamera == false)
	{
		if (scriptTimer.IsStopped())
		{
			scriptTimer.Start();
		}

		// Blood Explosion
		bloodSplash.position.x = 2681 - 64;
		bloodSplash.position.y = 464 - 64;
		bloodSplash.position.w = 128;
		bloodSplash.position.h = 128;
		App->particles->AddParticle(bloodSplash, 14, 0.1f);
		// Building-like Explosion
		spawnSplash.position.x = 2681 - 80;
		spawnSplash.position.y = 464 - 120;
		spawnSplash.position.w = 160;
		spawnSplash.position.h = 192;
		App->particles->AddParticle(spawnSplash, 4, 0.1f);

		// Units Move around Kerrigan
		scripted_unit1->SetTarget(2770, 380);
		scripted_unit2->SetTarget(2540, 500);
		scripted_unit3->SetTarget(2670, 600);
		scripted_unit4->SetTarget(2675, 360);
		scripted_unit5->SetTarget(2800, 480);

		App->IA->createBoss = false;
		App->IA->StartBossPhase();
	}
	
	// Kerrigan Starts Explosion
	if (scriptTimer.ReadSec() >= 6.0f && !action && scriptTimer.ReadSec() < 6.5f)
	{
		// Temporal Trigger
		//App->IA->boss->Hit(2500);
		action = true;
	}
	// Order to Attack Kerrigan
	else if (scriptTimer.ReadSec() >= 8.0f && action && scriptTimer.ReadSec() < 8.5f)
	{
		scripted_unit2->SetAttack(App->IA->boss);
		scripted_unit3->SetAttack(App->IA->boss);
		scripted_unit4->SetAttack(App->IA->boss);
		scripted_unit5->SetAttack(App->IA->boss);
		action = false;
	}
	// SecondEventScript - DESTRUCTOR
	if (scriptTimer.ReadSec() >= 15.0f)
	{
		scriptTimer.Stop();
		onEvent = false;
		kerriganSpawn = false;
		action = action_aux = false;
	}
}

void S_SceneMap::UpdateDisplayedResources(char* it_res_c)
{
	// Mineral Update
	if (App->player->stats.mineral > displayed_mineral)
	{
		displayed_mineral++;
	}
	else if (App->player->stats.mineral < displayed_mineral)
	{
		displayed_mineral--;
	}

	// Gas Update
	if (App->player->stats.gas > displayed_gas)
	{
		displayed_gas++;
	}
	else if (App->player->stats.gas < displayed_gas)
	{
		displayed_gas--;
	}

	// Print it
	sprintf_s(it_res_c, 7, "%d", displayed_mineral);
	res_lab[0]->SetText(it_res_c);

	sprintf_s(it_res_c, 7, "%d", displayed_gas);
	res_lab[1]->SetText(it_res_c);
}
void S_SceneMap::UpdateDisplayedPsiReached(float dt, char* it_res_c)
{
	// Animation
	if (App->player->stats.psi == App->player->stats.realMaxPsi && psi_reached_timer >= 50)
	{
		sprintf_s(it_res_c, 9, "%d/%d", App->player->stats.psi, App->player->stats.maxPsi);
		res_lab[2]->SetText(it_res_c, 255,0,0);
	}
	// Normal Display
	else
	{
		sprintf_s(it_res_c, 9, "%d/%d", App->player->stats.psi, App->player->stats.maxPsi);
		res_lab[2]->SetText(it_res_c);
	}
	if (psi_reached_timer >= 100)
		psi_reached_timer = 0;
	psi_reached_timer++;
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
	if (defeat && App->render->movingCamera == false)
	{
		App->entityManager->stopLoop = true;
		//App->entityManager->FreezeInput();
		App->minimap->Disable();
		gameFinished = true;
		use = victoryT = App->tex->Load("graphics/gui/defeatScreenTMP.png");
		App->audio->PlayMusic("sounds/music/ambient/defeat.ogg", 1.0f);
	}
	//Else if
	if (victory)
	{
		App->entityManager->stopLoop = true;
		//App->entityManager->FreezeInput();
		App->minimap->Disable();
		gameFinished = true;
		use = defeatT = App->tex->Load("graphics/gui/victoryScreenTMP.png");
		App->audio->PlayMusic("sounds/music/ambient/victory.ogg", 1.0f);
	}
	int w, h;
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;
	finalScreen = App->gui->CreateUI_Image({ 0, 0, w / App->events->GetScale(), h / App->events->GetScale() }, use, { 0, 0, 0, 0 });
	finalScreen->SetLayer(3);
	finalScreen->AddListener(this);
}
#pragma endregion