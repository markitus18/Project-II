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
#include "Boss.h"

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

	for (int n = 0; n < 6; n++)
	{
		cameraPositions.push_back({ -1,-1 });
	}

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
	zergSample = NULL;

	gameFinished = false;
	victory = false;
	defeat = false;

	for (int n = 0; n < 10; n++)
	{
		startingUnits[n] = NULL;
	}

	std::vector<iPoint>::iterator camPos = cameraPositions.begin();
	while (camPos != cameraPositions.end())
	{
		camPos->x = -1;
		camPos->y = -1;
		camPos++;
	}

	//SCRIPT RESOURCES -----------
	onEvent = true;
	kerriganSpawn = false;
	onEventVictory = false;
	action = action_aux = false;
	interruptEvent = false;
	scriptTimer.Start();
	scriptTimer.Stop();

	loading_tex = App->tex->Load("graphics/map_loading_screen.png");
	loading_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, loading_tex, { 0, 0, 0, 0 });
	loading_image->SetActive(true);
	loading_image->SetLayer(3);

	spawnSplash.texture = App->tex->Load("graphics/zerg/boss/boss_spawn.png");
	spawnSplash.position = { 0, 0, 160, 192 };
	spawnSplash.section = { 0, 0, 160, 192 };

	bloodSplash.texture = App->tex->Load("graphics/zerg/boss/boss_blood.png");
	bloodSplash.position = { 0, 0, 128, 128 };
	bloodSplash.section = { 0, 0, 128, 128 };

	br_x = 80;
	br_y = 370;

	auxBriefTimer.Start();
	auxBriefTimer.Stop();

	inactiveProbe_tex = App->tex->Load("graphics/ui/inactive_probes.png");
		inactiveProbe = App->gui->CreateUI_Image({ 400, 0, 0, 0 }, inactiveProbe_tex, { 0, 0, 0, 0 }, { -2, 0, 21, 21 });
	inactiveProbe->AddListener(this);
	inactiveProbe->SetActive(false);

	quit_info_font = App->font->Load("fonts/StarCraft.ttf", 12);

	intro_text_name = App->gui->CreateUI_Label({ br_x / scale, br_y / scale, 0, 0 }, "ZERATUL:", quit_info_font);
	intro_text_name->SetActive(false);
	intro_text_1 = App->gui->CreateUI_Label({ (br_x + 20) / scale, (br_y + 30) / scale, 0, 0 }, "    Our situation is critical, young Templar. Zerg hordes", quit_info_font);
	intro_text_1->SetActive(false);
	intro_text_2 = App->gui->CreateUI_Label({ (br_x + 20) / scale, (br_y + 60) / scale, 0, 0 }, "    have surrounded our position. We must defeat their", quit_info_font);
	intro_text_2->SetActive(false);
	intro_text_3 = App->gui->CreateUI_Label({ (br_x + 20) / scale, (br_y + 90) / scale, 0, 0 }, "    main Base with urge.", quit_info_font);
	intro_text_3->SetActive(false);
	intro_text_4 = App->gui->CreateUI_Label({ (br_x + 20) / scale, (br_y + 30) / scale, 0, 0 }, "    All the remaining warriors have answered our call.", quit_info_font);
	intro_text_4->SetActive(false);
	intro_text_5 = App->gui->CreateUI_Label({ (br_x + 20) / scale, (br_y + 60) / scale, 0, 0 }, "    Our last hope, the Zerg Sample, is the key to win", quit_info_font);
	intro_text_5->SetActive(false);
	intro_text_6 = App->gui->CreateUI_Label({ (br_x + 20) / scale, (br_y + 90) / scale, 0, 0 }, "    the war against Zerg. Defend it at all cost.", quit_info_font);
	intro_text_6->SetActive(false);

	spawn_text_name_1 = App->gui->CreateUI_Label({ br_x / scale, (br_y + 60) / scale, 0, 0 }, "ZERATUL:", quit_info_font);
	spawn_text_name_1->SetActive(false);
	spawn_text_name_2 = App->gui->CreateUI_Label({ br_x / scale, (br_y + 60) / scale, 0, 0 }, "SCOUT:", quit_info_font);
	spawn_text_name_2->SetActive(false);
	spawn_text_name_3 = App->gui->CreateUI_Label({ br_x / scale, (br_y + 60) / scale, 0, 0 }, "KERRIGAN:", quit_info_font);
	spawn_text_name_3->SetActive(false);
	spawn_text_1 = App->gui->CreateUI_Label({ (br_x + 20) / scale, (br_y + 90) / scale, 0, 0 }, "    Reinforcements are on route now, young Templar.", quit_info_font);
	spawn_text_1->SetActive(false);
	spawn_text_2 = App->gui->CreateUI_Label({ (br_x + 20) / scale, (br_y + 90) / scale, 0, 0 }, "    I fear no enemy!", quit_info_font);
	spawn_text_2->SetActive(false);
	spawn_text_3 = App->gui->CreateUI_Label({ (br_x + 20) / scale, (br_y + 90) / scale, 0, 0 }, "    I will kill you myself.", quit_info_font);
	spawn_text_3->SetActive(false);

	win_text_name = App->gui->CreateUI_Label({ br_x / scale, (br_y + 60) / scale, 0, 0 }, "ARTANIS:", quit_info_font);
	win_text_name->SetActive(false);
	win_text_2 = App->gui->CreateUI_Label({ (br_x + 20) / scale, (br_y + 90) / scale, 0, 0 }, "    Let us depart from this stretched world and return to Shakuras!", quit_info_font);
	win_text_2->SetActive(false);
	win_text_3 = App->gui->CreateUI_Label({ (br_x + 20) / scale, (br_y + 90) / scale, 0, 0 }, "    ...", quit_info_font);
	win_text_3->SetActive(false);
	//----------------------------
	displayed_mineral = displayed_gas = 0;
	psi_reached_timer = 0;
	//----------------------------

	sfx_shuttle_drop = App->audio->LoadFx("sounds/protoss/units/shuttle_drop.ogg");
	sfx_script_adquire = App->audio->LoadFx("sounds/ui/adquire.ogg");
	sfx_script_beep = App->audio->LoadFx("sounds/ui/beep.ogg");
	brief_leave_planet = App->audio->LoadFx("sounds/protoss/briefing/leave_this_planet.ogg");
	brief_no_fear = App->audio->LoadFx("sounds/protoss/briefing/no_fear_rekt.ogg");
	brief_reinforcement = App->audio->LoadFx("sounds/protoss/briefing/reinforcement.ogg");
	boss_kill_you = App->audio->LoadFx("sounds/zerg/units/kerrigan/boss_kill_you.ogg");

	App->map->Enable();
	App->map->Load("graphic.tmx");

	scripted_unit1 = NULL;
	scripted_unit2 = NULL;
	scripted_unit3 = NULL;
	scripted_unit4 = NULL;
	scripted_unit5 = NULL;
	scripted_zergling = NULL;
	scripted_shuttle1 = NULL;
	scripted_shuttle2 = NULL;

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
	quit_image = App->gui->CreateUI_Image({ (w/2 - 275)/ scale, (h / 2 - 250) / scale, 540 / scale, 300 / scale }, quit_tex, { 0, 0, 0, 0 });

	cancel_label = App->gui->CreateUI_Label({ 220 / scale, 250 / scale, 0, 0 }, "Cancel", quit_info_font, { -100 / scale, -20 / scale, 245 / scale, 60 / scale });
	cancel_label->AddListener(this);
	cancel_label->SetParent(quit_image);

	save_label = App->gui->CreateUI_Label({198 / scale, 60 / scale, 0, 0 }, "Save game", quit_info_font, { 0, 0, 0, 0 });
	save_label->SetParent(quit_image);
	save_border = App->gui->CreateUI_Image({ -29, -8, 122, 30 }, border_tex, { 0, 0, 0, 0 });
	save_border->SetParent(save_label);
	save_border->AddListener(this);

	quit_label = App->gui->CreateUI_Label({ 198 / scale, 160 / scale, 0, 0 }, "Quit game", quit_info_font, { 0, 0, 0, 0 });
	quit_label->SetParent(quit_image);
	quit_border = App->gui->CreateUI_Image({ -29, -8, 122, 30 }, border_tex, { 0, 0, 0, 0 });
	quit_border->SetParent(quit_label);
	quit_border->AddListener(this);

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

		App->entityManager->FreezeInput();
		App->minimap->freezeMovement = true;

	App->entityManager->muteUnitsSounds = true;
	startingUnits[0] = App->entityManager->CreateUnit(-1000, -100, ZEALOT, PLAYER);
	startingUnits[1] = App->entityManager->CreateUnit(-1000, -100, ZEALOT, PLAYER);
	startingUnits[2] = App->entityManager->CreateUnit(-1000, -100, DRAGOON, PLAYER);
	App->entityManager->muteUnitsSounds = false;

	App->render->SetCameraLimits({ 1, 1 }, App->events->GetMapSizeScaled() - App->events->GetScreenSize());

	App->entityManager->debug = false;
	screenMouse->SetActive(false);
	globalMouse->SetActive(false);
	tileMouse->SetActive(false);

	App->explosion->debug = false;
	App->pathFinding->displayPath = false;
	App->gui->debug = false;
	App->fogOfWar->globalVision = false;



	App->player->SetPsi(0);

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

	if (App->entityManager->loading == false)
	{
		loading_image->SetActive(false);
	}
	else
	{
		loading_image->SetActive(true);
	}

		if (App->entityManager->inactiveProbe != NULL)
		{
			inactiveProbe->SetActive(true);
		}
		else
		{
			inactiveProbe->SetActive(false);
		}

	// Scripts
	if (App->IA->createBoss)
	{
		if (onEvent == false)
		{
			onEvent = true;
			kerriganSpawn = true;
		}
	}

	ManageInput(dt);

	if (onEvent)
	{
		if (!kerriganSpawn && !onEventVictory)
		{
			FirstEventScript();
		}
		else if (kerriganSpawn && !onEventVictory)
		{
			SecondEventScript();
		}
		else
		{
			VictoryEventScript();
		}
	}
	else
	{
		App->entityManager->UnfreezeInput();
		App->minimap->freezeMovement = false;
	}

	// Stop Briefing from Cinematics
	if (auxBriefTimer.ReadSec() > 4.2f)
	{
		spawn_text_name_1->SetActive(false);
		spawn_text_name_3->SetActive(false);
		spawn_text_1->SetActive(false);
		spawn_text_3->SetActive(false);

		auxBriefTimer.Stop();
	}

	if (App->entityManager->debug)
	{
		App->pathFinding->Draw();

		labelUpdateTimer += (1.0f * dt);
		if (labelUpdateTimer > 0.1f)
		{
			labelUpdateTimer = 0.0f;
			iPoint mouseOnScreen = App->events->GetMouseOnScreen();
			iPoint mouseOnWorld = App->events->GetMouseOnWorld();
			screenMouse->SetText("Screen: %i, %i", mouseOnScreen.x, mouseOnScreen.y);
			globalMouse->SetText("World: %i, %i", mouseOnWorld.x, mouseOnWorld.y);

			iPoint tile = App->pathFinding->WorldToMap(mouseOnWorld.x, mouseOnWorld.y);
			tileMouse->SetText("Logic Tile: %i, %i", tile.x, tile.y);
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
				if (zergSample->currHP <= 0)
				{
					defeat = true;
					App->render->MoveCamera(400, 4800);
					App->entityManager->FreezeInput();
				}
				if (App->IA->boss)
				{
					if (App->IA->boss->currHP <= 0)
					{
						App->render->MoveCamera(App->IA->boss->GetPosition().x * 2 - App->events->GetScreenSize().x / 2, App->IA->boss->GetPosition().y * 2 - App->events->GetScreenSize().y / 2);
						App->entityManager->FreezeInput();
						victory = true;
						App->explosion->ClearExplosions();
					}
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

	App->font->Unload(quit_info_font);

	//Free textures (Should be done with a private list)
	//Main Textures
	App->tex->UnLoad(uiIconsT);
	App->tex->UnLoad(orderIconsT);
	App->tex->UnLoad(atlasT);
	App->tex->UnLoad(controlPT);
	App->tex->UnLoad(uiWireframesT);
	App->tex->UnLoad(victoryT);
	App->tex->UnLoad(defeatT);
	App->tex->UnLoad(debug_tex);
	App->tex->UnLoad(quit_tex);
	App->tex->UnLoad(border_tex);
	App->tex->UnLoad(boss_base_barT);
	App->tex->UnLoad(boss_shield_barT);
	App->tex->UnLoad(boss_life_barT);
	//Hover Textures
	App->tex->UnLoad(buildings_hover);
	App->tex->UnLoad(units_hover);
	App->tex->UnLoad(orderAttack_hover);
	App->tex->UnLoad(orderCancel_hover);
	App->tex->UnLoad(orderMove_hover);
	App->tex->UnLoad(orderRallypoint_hover);
	App->tex->UnLoad(orderStop_hover);
	App->tex->UnLoad(orderStructure_hover);
	App->tex->UnLoad(progressBar_back);
	App->tex->UnLoad(progressBar_bar);
	App->tex->UnLoad(loading_tex);
	App->tex->UnLoad(inactiveProbe_tex);


	App->tex->UnLoad(spawnSplash.texture);
	App->tex->UnLoad(bloodSplash.texture);
	
	//Delete all unit elements
	App->gui->DeleteUIElement(screenMouse);
	App->gui->DeleteUIElement(globalMouse);
	App->gui->DeleteUIElement(tileMouse);

	App->gui->DeleteUIElement(controlPanel);
	App->gui->DeleteUIElement(finalScreen);
	App->gui->DeleteUIElement(cancel_label);
	App->gui->DeleteUIElement(save_label);
	App->gui->DeleteUIElement(save_border);
	App->gui->DeleteUIElement(quit_image);
	App->gui->DeleteUIElement(quit_label); 
	App->gui->DeleteUIElement(quit_border);

	App->gui->DeleteUIElement(inactiveProbe);
	App->gui->DeleteUIElement(loading_image);

	App->gui->DeleteUIElement(intro_text_name);
	App->gui->DeleteUIElement(intro_text_1);
	App->gui->DeleteUIElement(intro_text_2);
	App->gui->DeleteUIElement(intro_text_3);
	App->gui->DeleteUIElement(intro_text_4);
	App->gui->DeleteUIElement(intro_text_5);
	App->gui->DeleteUIElement(intro_text_6);
	App->gui->DeleteUIElement(spawn_text_name_1);
	App->gui->DeleteUIElement(spawn_text_name_2);
	App->gui->DeleteUIElement(spawn_text_name_3);
	App->gui->DeleteUIElement(spawn_text_1);
	App->gui->DeleteUIElement(spawn_text_2);
	App->gui->DeleteUIElement(spawn_text_3);
	App->gui->DeleteUIElement(win_text_name);
	App->gui->DeleteUIElement(win_text_2);
	App->gui->DeleteUIElement(win_text_3);
	//App->gui->DeleteUIElement(bossBlood);

	App->gui->DeleteUIElement(bossShield);
	App->gui->DeleteUIElement(bossLife);
	App->gui->DeleteUIElement(bossBase);

	App->gui->DeleteUIElement(res_img_0);
	App->gui->DeleteUIElement(res_img_1);
	App->gui->DeleteUIElement(res_img_2);

	App->gui->DeleteUIElement(res_lab_0);
	App->gui->DeleteUIElement(res_lab_1);
	App->gui->DeleteUIElement(res_lab_2);
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

	//Enable / Disable map render
	if (App->events->GetEvent(E_DEBUG_ENTITY_MANAGER) == EVENT_DOWN)
	{
		App->entityManager->debug = !App->entityManager->debug;
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

	if (App->explosion->debug)
	{
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
	}

	if (App->entityManager->debug)
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
	}

	if (App->events->GetEvent(E_OPEN_MENU) == EVENT_DOWN && onEvent)
	{
		interruptEvent = true;
	}

	if (onEvent == false)
	{

		if (App->events->IsInputFrozen() == false && onEvent == false && gameFinished == false)
		{
#pragma region //Camera saved positions
			if (App->events->GetEvent(E_CAM_POS_1) == EVENT_DOWN)
			{
				if (App->events->GetEvent(E_CAM_POS_EDIT) == EVENT_REPEAT)
				{
					cameraPositions[0].x = App->render->camera.x;
					cameraPositions[0].y = App->render->camera.y;
				}
				else if (cameraPositions[0].x != -1 && cameraPositions[0].y != -1)
				{
					App->render->camera.x = cameraPositions[0].x;
					App->render->camera.y = cameraPositions[0].y;
				}
			}
			if (App->events->GetEvent(E_CAM_POS_2) == EVENT_DOWN)
			{
				if (App->events->GetEvent(E_CAM_POS_EDIT) == EVENT_REPEAT)
				{
					cameraPositions[1].x = App->render->camera.x;
					cameraPositions[1].y = App->render->camera.y;
				}
				else if (cameraPositions[1].x != -1 && cameraPositions[1].y != -1)
				{
					App->render->camera.x = cameraPositions[1].x;
					App->render->camera.y = cameraPositions[1].y;
				}
			}
			if (App->events->GetEvent(E_CAM_POS_3) == EVENT_DOWN)
			{
				if (App->events->GetEvent(E_CAM_POS_EDIT) == EVENT_REPEAT)
				{
					cameraPositions[2].x = App->render->camera.x;
					cameraPositions[2].y = App->render->camera.y;
				}
				else if (cameraPositions[2].x != -1 && cameraPositions[2].y != -1)
				{
					App->render->camera.x = cameraPositions[2].x;
					App->render->camera.y = cameraPositions[2].y;
				}
			}
			if (App->events->GetEvent(E_CAM_POS_4) == EVENT_DOWN)
			{
				if (App->events->GetEvent(E_CAM_POS_EDIT) == EVENT_REPEAT)
				{
					cameraPositions[3].x = App->render->camera.x;
					cameraPositions[3].y = App->render->camera.y;
				}
				else if (cameraPositions[3].x != -1 && cameraPositions[3].y != -1)
				{
					App->render->camera.x = cameraPositions[3].x;
					App->render->camera.y = cameraPositions[3].y;
				}
			}
			if (App->events->GetEvent(E_CAM_POS_5) == EVENT_DOWN)
			{
				if (App->events->GetEvent(E_CAM_POS_EDIT) == EVENT_REPEAT)
				{
					cameraPositions[4].x = App->render->camera.x;
					cameraPositions[4].y = App->render->camera.y;
				}
				else if (cameraPositions[4].x != -1 && cameraPositions[4].y != -1)
				{
					App->render->camera.x = cameraPositions[4].x;
					App->render->camera.y = cameraPositions[4].y;
				}
			}
			if (App->events->GetEvent(E_CAM_POS_6) == EVENT_DOWN)
			{
				if (App->events->GetEvent(E_CAM_POS_EDIT) == EVENT_REPEAT)
				{
					cameraPositions[5].x = App->render->camera.x;
					cameraPositions[5].y = App->render->camera.y;
				}
				else if (cameraPositions[5].x != -1 && cameraPositions[5].y != -1)
				{
					App->render->camera.x = cameraPositions[5].x;
					App->render->camera.y = cameraPositions[5].y;
				}
			}

#pragma endregion

#pragma region //Camera on last ping

			if (App->events->GetEvent(E_CAM_LAST_PING) == EVENT_DOWN)
			{
				if (App->minimap->pingPos.x != 0 && App->minimap->pingPos.y != 0)
				{
					App->render->camera.x = App->minimap->MinimapToWorld(App->minimap->pingPos.x, App->minimap->pingPos.y).x * 2 - App->events->GetScreenSize().x / 2;
					App->render->camera.y = App->minimap->MinimapToWorld(App->minimap->pingPos.x, App->minimap->pingPos.y).y * 2 - App->events->GetScreenSize().y / 2;
				}
			}

#pragma endregion

#pragma region //Camera on selection / Sample

			if (App->events->GetEvent(E_CAM_ON_SELECTION) == EVENT_DOWN)
			{
				if (App->entityManager->selectedBuilding != NULL)
				{
					App->render->camera.x = App->entityManager->selectedBuilding->GetCollider().x * 2;
					App->render->camera.y = App->entityManager->selectedBuilding->GetCollider().y * 2;
				}
				else if (App->entityManager->selectedResource != NULL)
				{
					App->render->camera.x = App->entityManager->selectedResource->GetCollider().x * 2;
					App->render->camera.y = App->entityManager->selectedResource->GetCollider().y * 2;
				}
				else if (App->entityManager->selectedEnemyUnit != NULL)
				{
					App->render->camera.x = App->entityManager->selectedEnemyUnit->GetPosition().x * 2;
					App->render->camera.y = App->entityManager->selectedEnemyUnit->GetPosition().y * 2;
				}
				else if (App->entityManager->selectedUnits.empty() == false)
				{
					App->render->camera.x = App->entityManager->selectedUnits.front()->GetPosition().x * 2;
					App->render->camera.y = App->entityManager->selectedUnits.front()->GetPosition().y * 2;
				}
				else
				{
					App->render->camera.x = zergSample->GetCollider().x * 2;
					App->render->camera.y = zergSample->GetCollider().y * 2;
				}
				App->render->camera.x -= App->events->GetScreenSize().x / 2;
				App->render->camera.y -= App->events->GetScreenSize().y / 2;
			}

#pragma endregion

			if (onEvent == false && App->render->movingCamera == false && victory == false && defeat == false && App->entityManager->loading == false)
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



			if (onEvent == false && App->render->movingCamera == false && defeat == false && victory == false & App->entityManager->loading == false)
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
	}
}

void S_SceneMap::UnitCreationInput()
{

	if (App->events->GetEvent(E_SPAWN_NEXUS) == EVENT_DOWN)
	{
		Building* building = App->entityManager->CreateBuilding(currentTile_x, currentTile_y, NEXUS, PLAYER);
		if (building)
		{
			building->FinishSpawn();
		}
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
	orderMove_hover = App->tex->Load("graphics/ui/Hover_Texts/order_move.png");
	orderRallypoint_hover = App->tex->Load("graphics/ui/Hover_Texts/order_rallyPoint.png");
	orderStop_hover = App->tex->Load("graphics/ui/Hover_Texts/order_stop.png");
	orderStructure_hover = App->tex->Load("graphics/ui/Hover_Texts/order_structure.png");

	//Kerrigan Bars
	boss_life_barT = App->tex->Load("graphics/gui/Life_Bar_Health.png");
	boss_shield_barT = App->tex->Load("graphics/gui/Life_Bar_Shield.png");
	boss_base_barT = App->tex->Load("graphics/gui/Life_Bar_Base.png");
	//Progress Bar
	progressBar_back = App->tex->Load("graphics/ui/hpbarempt.png");
	progressBar_bar = App->tex->Load("graphics/ui/hpbarfull.png");
	//Quit texture
	quit_tex = App->tex->Load("graphics/ui/readyt/pdpopup2.png"); 
	border_tex = App->tex->Load("graphics/ui/readyt/tframeh4.png");

	

}

void S_SceneMap::LoadGUI()
{
	//UI WEIRD STUFF----------------------------------
#pragma region Misc

	res_img_0 = App->gui->CreateUI_Image({ 436 , 3, 0, 0 }, (SDL_Texture*)uiIconsT, { 0, 0, 14, 14 });
	res_img_1 = App->gui->CreateUI_Image({ 504, 3, 0, 0 }, (SDL_Texture*)uiIconsT, { 0, 42, 14, 14 });
	res_img_2 = App->gui->CreateUI_Image({ 572, 3, 0, 0 }, (SDL_Texture*)uiIconsT, { 0, 84, 14, 14 });

	res_lab_0 = App->gui->CreateUI_Label({ 452 , 4, 0, 0 }, "0");
	res_lab_1 = App->gui->CreateUI_Label({ 520, 4, 0, 0 }, "0");
	res_lab_2 = App->gui->CreateUI_Label({ 588, 4, 0, 0 }, "0");

	res_img_0->SetLayer(0);
	res_img_1->SetLayer(0);
	res_img_2->SetLayer(0);

	res_lab_0->SetLayer(1);
	res_lab_1->SetLayer(1);
	res_lab_2->SetLayer(1);

	// Inserting the control Panel Image

	controlPanel = App->gui->CreateUI_Image({ 0,301, 640, 179 }, controlPT, { 0, 0, 0, 0 }, { 0, 60, 640, 118 });
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

	//First 0.382813f % 0.8416666f %
	//Others 0.38125f % 0.9229166f %
	panel_queue = new UI_Panel_Queue();
	panel_queue->icon_rects = &ui_unit_sections;
	panel_queue->background = App->gui->CreateUI_Image({ 242.0f , 401.0f , 154.0f , 75.0f }, queue_backgroundT, { 0, 0, 0, 0 });
	panel_queue->background->SetLayer(1);
	
	panel_queue->progress_background = App->gui->CreateUI_Image({282.0f , 427.0f , 0, 0 }, progressBar_back, { 0, 0, 0, 0 });
	panel_queue->progress_background->SetLayer(1);

	panel_queue->progress_bar = App->gui->CreateUI_ProgressBar_F({ 282.0f , 427.0f, 0, 0 }, progressBar_bar, &panel_queue->bar_max, &panel_queue->bar_current);
	panel_queue->progress_background->SetActive(false);
	panel_queue->progress_bar->SetActive(false);
	panel_queue->progress_bar->SetLayer(1);
	//396 39
	//Diff 244, 443| 283, 404
	float x_q = 205;
	float y_q = 443;
	for (int i = 0; i < 5; i++)
	{
		panel_queue->icons[i] = App->gui->CreateUI_Image({ x_q, y_q, 32 , 32 }, orderIconsT, { 469, 345, 32, 32 });
		panel_queue->icons[i]->SetLayer(2);

		panel_queue->icons[i]->SetActive(false);

		panel_queue->icons[i]->AddListener(this);
		x_q += 39;
	}
	panel_queue->icons[0]->localPosition.x = 245;
	panel_queue->icons[0]->localPosition.y = 404;
	panel_queue->background->SetActive(false);
	
#pragma endregion

#pragma region Stats Panel Multiple

	statsPanel_m = new Stats_Panel_Mult();

	float yF_m = 397, xF_m = 169;
	//Row elements
	int r_e = 6;
	//26,4% 82,7%
	//32.0f% 90.3f%
	for (uint j = 0; j < 2; j++)
	{
		for (uint i = 0; i < r_e; i++)
		{
			uint index = i + (j * r_e);
			statsPanel_m->unitSelect_frames[index] = App->gui->CreateUI_Image({ xF_m,  yF_m, 33, 34 }, atlasT, { 936, 0, 33, 34 });
			statsPanel_m->unitSelect_frames[index]->SetLayer(1);

			statsPanel_m->unitSelect_wires[index].wireframe = App->gui->CreateUI_Image({ 1, 1, 31, 32  }, uiWireframesT, { 0, 0, 31, 32 });
			statsPanel_m->unitSelect_wires[index].wireframe->SetLayer(2);
			statsPanel_m->unitSelect_wires[index].wireframe->AddListener(this);
			statsPanel_m->unitSelect_wires[index].wireframe->SetParent(statsPanel_m->unitSelect_frames[index]);	
			
			xF_m += 36;
		}
		xF_m = 169;
		yF_m += 36;
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

	//------------
	//Create probe button
	butt_it = nexus->setOrder(App->entityManager->o_GenProbe_toss, idle, clicked, 0, 0, *atlasT);
	//butt_it->localPosition.w = 0;
	//butt_it->localPosition.h = height_frame;
	//Hovering image
	int y = 62;
	int h = 62;
	int w = App->events->GetScreenSize().x;

//	int w = 640;
	image_it = App->gui->CreateUI_Image({ w / 2.0f - 195, 300 - h, 0, 0 }, units_hover, { 0, y, 195, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0,0 }, orderIconsT, { 468, 102, 32, 32 });
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
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, orderIconsT, { 504, 544, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	nexus->changeState(false);

//  basic_u -----------

	Grid3x3* basic_u = new Grid3x3(*coords,G_BASIC_UNIT);

	grids.push_back(basic_u);
	gridTypes.push_back(basic_u->type);

	//gui->SetCurrentGrid(basic_u);

	butt_it = basic_u->setOrder(App->entityManager->o_Move, idle, clicked, 0, 0, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ 0, -11, 0, 0 }, orderMove_hover, { 0, 0, 29, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it, true);

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
	butt_it->SetHoverImage(image_it, true);

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
	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 324, 442, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	//------------


	//------------

	basic_u->changeState(false);
// basicBuildings ------------
	Grid3x3* basicBuildings = new Grid3x3(*coords, G_BASIC_BUILDINGS);

	grids.push_back(basicBuildings);
	gridTypes.push_back(basicBuildings->type);

	// Nexus { 108, 304, 32, 32 }
	// Pylon  { 36, 304, 32, 32 }

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Pylon, idle, clicked, 0, 1, *atlasT);
	
	//Hovering image
	y = 64;
	h = 77;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, buildings_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 108, 304, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Nexus, idle, clicked, 0, 0, *atlasT);

	//Hovering image
	y = 0;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, buildings_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 36, 304, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Gateaway, idle, clicked, 1, 0, *atlasT);

	//Hovering image
	y = 205;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, buildings_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 252, 306, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Assimilator, idle, clicked, 0, 2, *atlasT);

	//Hovering image
	//Hovering image
	y = 141;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, buildings_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 144, 306, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;


	butt_it = basicBuildings->setOrder(App->entityManager->o_Return_Builds_Menu, idle, clicked, 2,2, *atlasT);

	//Hovering image
	image_it = App->gui->CreateUI_Image({ -28, -11, 0, 0 }, orderCancel_hover, { 0, 0, 69, 11 });
	image_it->SetActive(false);
	image_it->SetLayer(2);

	butt_it->SetHoverImage(image_it, true);

	image_it = App->gui->CreateUI_Image({ 3, 4, 0, 0 }, orderIconsT, { 540, 442, 26, 26 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	
	//Cybernetics
	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Cybernetics, idle, clicked, 1, 1, *atlasT);

	//Hovering image
	y = 269;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, buildings_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 396, 306, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	//Robotics_Bay
	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Robotics_Support_Bay, idle, clicked, 2, 0, *atlasT);

	//Hovering image
	y = 397;
	h = 83;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, buildings_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 36, 340, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(CYBERNETICS_CORE);

	//Templar archives
	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Templar_Archives, idle, clicked, 2, 1, *atlasT);

	//Hovering image
	y = 480;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, buildings_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, { 432, 306, 32, 32 });
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(ROBOTICS_BAY);

	//Photon Cannon
	butt_it = basicBuildings->setOrder(App->entityManager->o_Build_Photon, idle, clicked, 1, 2, *atlasT);

	//Hovering image
	y = 333;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, buildings_hover, { 0, y, 200, h });
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
	
	// o_Basic_Builds 
	butt_it = probeMenu->setOrder(App->entityManager->o_Basic_Builds, idle, clicked, 2,0, *atlasT);

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
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Zealot, idle, clicked, 0, 0, *atlasT);

	//Hovering image
	y = 258;
	h = 51;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, units_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, ui_unit_sections[ZEALOT]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	//o_Gen_Dragoon
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Dragoon, idle, clicked, 0, 1, *atlasT);

	//Hovering image
	y = 449;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, units_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, ui_unit_sections[DRAGOON]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(CYBERNETICS_CORE);

	//o_Gen_D_Templar
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Dark_Templar, idle, clicked, 1, 0, *atlasT);

	//Hovering
	y = 385;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, units_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, ui_unit_sections[DARK_TEMPLAR]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(ROBOTICS_BAY);

	//o_Gen_Scout
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Scout, idle, clicked, 2, 1, *atlasT);

	y = 127;
	h = 68;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, units_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, ui_unit_sections[SCOUT]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it->InitRequiredBuilding(ROBOTICS_BAY);

	//o_Gen_Reaver
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Reaver, idle, clicked, 0, 2, *atlasT);

	//Hovering
	y = 194;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, units_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, ui_unit_sections[REAVER]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;
	butt_it->InitRequiredBuilding(TEMPLAR_ARCHIVES);

	//o_Gen_Observer
	butt_it = gateways->setOrder(App->entityManager->o_Gen_Observer, idle, clicked, 2, 0, *atlasT);

	//Hovering
	y = 0;
	h = 64;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, units_hover, { 0, y, 200, h });
	image_it->SetActive(false);
	image_it->SetLayer(1);
	butt_it->SetHoverImage(image_it);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, ui_unit_sections[OBSERVER]);
	image_it->SetParent(butt_it);
	image_it->SetLayer(1);

	butt_it->son = image_it;

	butt_it->InitRequiredBuilding(CYBERNETICS_CORE);

	//o_Gen_High_Templar
	butt_it = gateways->setOrder(App->entityManager->o_Gen_High_Templar, idle, clicked, 1, 1, *atlasT);

	//Hovering
	y = 309;
	h = 76;
	image_it = App->gui->CreateUI_Image({ w / 2 - 200, controlPanel->GetWorldPosition().y + 31 - h, 0, 0 }, units_hover, { 0, y, 200, h });
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
	butt_it = gateways->setOrder(App->entityManager->o_Set_rallyPoint, idle, clicked, 1, 2, *atlasT);

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
	if (element == inactiveProbe && event == UI_MOUSE_DOWN && App->render->movingCamera == false && onEvent == false && gameFinished == false)
	{
		if (App->entityManager->inactiveProbe != NULL)
		{
			App->entityManager->hoveringUnit = App->entityManager->inactiveProbe;
			App->entityManager->DoSingleSelection();
			App->entityManager->hoveringUnit = NULL;

			App->render->camera.x = App->entityManager->inactiveProbe->GetPosition().x * 2;
			App->render->camera.y = App->entityManager->inactiveProbe->GetPosition().y * 2;
			App->render->camera.x -= App->events->GetScreenSize().x / 2;
			App->render->camera.y -= App->events->GetScreenSize().y / 2;
		}
	}

	if (element == save_border && event == UI_MOUSE_DOWN)
	{
		App->SaveGame(App->player_name.c_str());
		quit_image->SetActive(false);
	}

	if (element == quit_border && event == UI_MOUSE_DOWN)
	{
		quit_image->SetActive(false);
		App->changeScene(App->sceneMenu, this);
	}
	 
	if (element == cancel_label && event == UI_MOUSE_DOWN)
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
	App->player->stats.mineral = 150;
	App->player->stats.gas = 40;
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

#pragma region //interruption
	if (interruptEvent)
	{
		App->entityManager->muteUnitsSounds = true;
		startingUnits[2]->SetPosition(625, 2560);
		startingUnits[2]->SetTarget(630, 2560);

		startingUnits[1]->SetPosition(580, 2570);
		startingUnits[1]->SetTarget(585, 2570);

		startingUnits[0]->SetPosition(615, 2605);
		startingUnits[0]->SetTarget(620, 2605);

		if (startingUnits[3] == NULL)
		{
			App->entityManager->CreateUnit(286, 2710, PROBE, PLAYER);
		}
		if (startingUnits[4] == NULL)
		{
			App->entityManager->CreateUnit(339, 2694, PROBE, PLAYER);
		}
		if (startingUnits[5] == NULL)
		{
			App->entityManager->CreateUnit(320, 2747, PROBE, PLAYER);
		}

		if (scripted_zergling)
		{
			scripted_zergling->Hit(100);
		}
		App->player->SetPsi(9);
	}
	App->entityManager->muteUnitsSounds = false;
#pragma endregion
	// First Time Line
	if (time >= (1.0f * 3.0f / 4.0f) && time < (1.2f * 3.0f / 4.0f))
	{
		intro_text_name->SetActive(true);
		intro_text_1->SetActive(true);
		intro_text_2->SetActive(true);
		intro_text_3->SetActive(true);
		if (!action )
		{
			App->audio->PlayFx(sfx_script_beep);
			action = true;
		}
	}
	if (time >= (1.1f * 3.0f / 4.0f) && action && time < (1.2f * 3.0f / 4.0f))
	{
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
		App->audio->PlayFx(sfx_script_beep);
		intro_text_name->SetActive(true);
		intro_text_4->SetActive(true);
		intro_text_5->SetActive(true);
		intro_text_6->SetActive(true);

		scripted_unit2->SetTarget(600, 2820);
		action = false;
	}
	// Shuttle 1 Drops the first Probe
	else if (time >(21.0f * 3.0f / 4.0f) && !action && time < (21.5f * 3.0f / 4.0f))
	{
		startingUnits[5] = App->entityManager->CreateUnit(320, 2747, PROBE, PLAYER);
		App->player->AddPsi(1);
		

		App->audio->PlayFx(sfx_shuttle_drop, 0);
		action = true;
	}
	// Shuttle 1 Drops the second Probe
	else if (time >(23.5f * 3.0f / 4.0f) && action && time < (23.9f * 3.0f / 4.0f))
	{
		startingUnits[4] = App->entityManager->CreateUnit(339, 2694, PROBE, PLAYER);
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
		startingUnits[0]->SetPosition(615, 2605);
		startingUnits[0]->SetTarget(620, 2605);
		
		App->player->AddPsi(2);
		// Carrier Leaves
		scripted_unit1->SetTarget(1070, 2300);

		App->audio->PlayFx(sfx_shuttle_drop, 0);
		action = true;
	}
	// Shuttle 1 Drop the third Probe
	else if (time >= (26.0f * 3.0f / 4.0f) && action && time < (26.5f * 3.0f / 4.0f))
	{
		startingUnits[3] = App->entityManager->CreateUnit(286, 2710, PROBE, PLAYER);
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
		startingUnits[1]->SetPosition(580, 2570);
		startingUnits[1]->SetTarget(585, 2570);

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
		startingUnits[2]->SetPosition(625, 2560);
		startingUnits[2]->SetTarget(630, 2560);

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
		intro_text_name->SetActive(false);
		intro_text_1->SetActive(false);
		intro_text_2->SetActive(false);
		intro_text_3->SetActive(false);

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
	else if (time > 36.5f  * 3.0f / 4.0f)
	{
		intro_text_name->SetActive(false);
		intro_text_4->SetActive(false);
		intro_text_5->SetActive(false);
		intro_text_6->SetActive(false);
	}

	// FirstEventScript - DESTRUCTOR
	if (time >= 37.0f * 3.0f / 4.0f || interruptEvent)
	{
		intro_text_name->SetActive(false);
		intro_text_1->SetActive(false);
		intro_text_2->SetActive(false);
		intro_text_3->SetActive(false);
		intro_text_4->SetActive(false);
		intro_text_5->SetActive(false);
		intro_text_6->SetActive(false);

		App->audio->PlayFx(sfx_script_adquire);
		App->entityManager->muteUnitsSounds = true;
		if (scripted_unit1)
		{
			scripted_unit1->SetPosition(1, 1);
			scripted_unit1->Hit(1000000);
		}
		if (scripted_unit2)
		{
			scripted_unit2->SetPosition(1, 1);
			scripted_unit2->Hit(1000000);
		}
		if (scripted_unit3)
		{
			scripted_unit3->SetPosition(1, 1);
			scripted_unit3->Hit(1000000);
		}
		if (scripted_unit4)
		{
			scripted_unit4->SetPosition(1, 1);
			scripted_unit4->Hit(1000000);
		}
		if (scripted_unit5)
		{
			scripted_unit5->SetPosition(1, 1);
			scripted_unit5->Hit(1000000);
		}
			
		if (scripted_shuttle1)
		{
			scripted_shuttle1->Hit(1000000);
			scripted_shuttle2->SetPosition(1, 1);
		}
		if (scripted_shuttle2)
		{
			scripted_shuttle2->Hit(1000000);
			scripted_shuttle1->SetPosition(1, 1);
		}

		// Reset Variables
		scriptTimer.Stop();
		onEvent = false;
		action = action_aux = false;
		interruptEvent = false; 
		App->render->camera.x = 190 * App->events->GetScale();
		App->render->camera.y = 2450 * App->events->GetScale();
		App->render->movingCamera = false;


		App->entityManager->muteUnitsSounds = false;
		LOG("Introduction Cinematic Completed.");
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

		scripted_unit1 = App->entityManager->CreateUnit(2920, 150, CARRIER, CINEMATIC);
		scripted_unit2 = App->entityManager->CreateUnit(2600, 5, SCOUT_CIN, CINEMATIC);
		scripted_unit3 = App->entityManager->CreateUnit(3100, 400, SCOUT_CIN, CINEMATIC);
		scripted_unit4 = App->entityManager->CreateUnit(2970, 5, SCOUT_CIN, CINEMATIC);
		scripted_unit5 = App->entityManager->CreateUnit(3070, 70, SCOUT_CIN, CINEMATIC);
		scripted_shuttle1 = App->entityManager->CreateUnit(2775, 5, SCOUT_CIN, CINEMATIC);
		scripted_shuttle2 = App->entityManager->CreateUnit(3060, 200, SCOUT_CIN, CINEMATIC);

		if (auxBriefTimer.IsStopped())
		{
			auxBriefTimer.Start();
		}

		spawn_text_name_1->SetActive(true);
		spawn_text_1->SetActive(true);

		App->audio->PlayFx(brief_reinforcement);

		// First Horror
		App->entityManager->Horror(2681, 464, 350, PLAYER);

		action_aux = true;
	}

	if (App->IA->createBoss == true && App->render->movingCamera == false)
	{
		//bossBlood = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, boss_bloodT, { 0, 0, 0, 0 });
		//bossBlood->sprite.tint = { 190, 190, 190, 255 };
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
		scripted_unit2->SetTarget(2550, 510);
		scripted_unit3->SetTarget(2665, 585);
		scripted_unit4->SetTarget(2685, 360);
		scripted_unit5->SetTarget(2800, 480);
		scripted_shuttle1->SetTarget(2575, 415);
		scripted_shuttle2->SetTarget(2750, 550);

		App->IA->createBoss = false;
		App->IA->StartBossPhase();
		App->entityManager->Horror(2681, 464, 500, PLAYER);
		App->entityManager->Horror(2681, 464, 500, COMPUTER);
		App->gui->AddBossBar();
	}
	// No Fear Warcry
	if (scriptTimer.ReadSec() >= 4.0f && !action && scriptTimer.ReadSec() < 4.5f)
	{
		App->entityManager->stopLoop = true;
		App->audio->PlayFx(brief_no_fear);

		spawn_text_name_2->SetActive(true);
		spawn_text_2->SetActive(true);

		action = true;
	}

	else if (scriptTimer.ReadSec() >= 4.6f && action && scriptTimer.ReadSec() < 5.0f)
	{
		action = false;
	}
	// Order to Attack Kerrigan
	else if (scriptTimer.ReadSec() >= 5.9f && !action && scriptTimer.ReadSec() < 5.95f)
	{
		scripted_unit3->SetAttack(App->IA->boss);
		scripted_unit4->SetAttack(App->IA->boss);
		scripted_shuttle1->SetAttack(App->IA->boss);

		action = true;
	}
	else if (scriptTimer.ReadSec() >= 6.0f && action && scriptTimer.ReadSec() < 6.5f)
	{
		scripted_unit2->SetAttack(App->IA->boss);
		scripted_unit5->SetAttack(App->IA->boss);
		scripted_shuttle2->SetAttack(App->IA->boss);

		action = false;
	}
	// I'll kill you myself!
	else if (scriptTimer.ReadSec() >= 7.0f && !action && scriptTimer.ReadSec() < 7.5f)
	{
		spawn_text_name_2->SetActive(false);
		spawn_text_2->SetActive(false);

		action = true;
	}

	// ???
	if (scriptTimer.ReadSec() >= 15.0f)
	{
		//bossBlood->sprite.tint = { 190, 190, 190, 150 };	
	}
	
	// SecondEventScript - DESTRUCTOR
	if (scriptTimer.ReadSec() >= 12.0f)
	{	
		App->audio->PlayFx(boss_kill_you);

		spawn_text_name_3->SetActive(true);
		spawn_text_3->SetActive(true);

		auxBriefTimer.Start();

		scriptTimer.Stop();
		onEvent = false;
		kerriganSpawn = false;
		action = action_aux = false;
		App->entityManager->stopLoop = false;

		LOG("Kerrigan Spawn Cinematic Completed.");
	}
}

void S_SceneMap::VictoryEventScript()
{
	if (App->IA->boss)
	{
		if (App->IA->boss->GetState() != STATE_DIE)
		{
			scriptTimer.Start();
			if (App->IA->boss)
			{
				App->IA->boss->StartDeath();
			}
		}
	}
	if (scriptTimer.ReadSec() >= 2.5f &&  App->render->movingCamera == false)
	{
		App->entityManager->stopLoop = true;
		spawn_text_name_3->SetActive(false);
		win_text_3->SetActive(false);
	}

	if (scriptTimer.ReadSec() >= 3.5f && !action && scriptTimer.ReadSec() < 4.0f)
	{
		App->render->camera.x = 190 * App->events->GetScale();
		App->render->camera.y = 2400 * App->events->GetScale();

		action = true;
	}
	else if (scriptTimer.ReadSec() >= 5.0f && action && scriptTimer.ReadSec() < 5.5f)
	{
		win_text_name->SetActive(true);
		win_text_2->SetActive(true);
		App->audio->PlayFx(brief_leave_planet);

		action = false;
	}

	else if (scriptTimer.ReadSec() >= 6.0f && !action && scriptTimer.ReadSec() < 6.5f)
	{
		scripted_unit1 = App->entityManager->CreateUnit(300, 2400, SCOUT_CIN, CINEMATIC);
		scripted_unit2 = App->entityManager->CreateUnit(450, 2390, SCOUT_CIN, CINEMATIC);
		scripted_shuttle1 = App->entityManager->CreateUnit(360, 2390, SHUTTLE, CINEMATIC);

		scripted_unit1->SetTarget(420, 2550);
		scripted_unit2->SetTarget(600, 2550);
		scripted_shuttle1->SetTarget(510, 2550);

		action = true;
	}
	else if (scriptTimer.ReadSec() >= 9.0f && scripted_shuttle1->GetMovementState() == MOVEMENT_IDLE)
	{
		App->audio->PlayFx(sfx_shuttle_drop);
		scripted_unit1->SetTarget(600, 3000);
		scripted_unit2->SetTarget(750, 3000);
		scripted_shuttle1->SetTarget(680, 3000);
	}
	else if (scriptTimer.ReadSec() > 11.0f)
	{
		win_text_name->SetActive(false);
		win_text_2->SetActive(false);

		onEvent = false;
	}
}

void S_SceneMap::UpdateDisplayedResources(char* it_res_c)
{
	// Mineral Update
	if (App->player->stats.mineral > displayed_mineral)
	{
		displayed_mineral++;
		if (App->player->stats.mineral > displayed_mineral)
		{
			displayed_mineral++;
		}
	}
	else if (App->player->stats.mineral < displayed_mineral)
	{
		displayed_mineral--;
		if (App->player->stats.mineral < displayed_mineral)
		{
			displayed_mineral--;
		}
	}

	// Gas Update
	if (App->player->stats.gas > displayed_gas)
	{
		displayed_gas++;
		if (App->player->stats.gas > displayed_gas)
		{
			displayed_gas++;
		}
	}
	else if (App->player->stats.gas < displayed_gas)
	{
		displayed_gas--;
		if (App->player->stats.gas < displayed_gas)
		{
			displayed_gas--;
		}
	}

	// Print it
	sprintf_s(it_res_c, 7, "%d", displayed_mineral);
	res_lab_0->SetText(it_res_c);

	sprintf_s(it_res_c, 7, "%d", displayed_gas);
	res_lab_1->SetText(it_res_c);
}
void S_SceneMap::UpdateDisplayedPsiReached(float dt, char* it_res_c)
{
	// Animation
	if (App->player->stats.psi == App->player->stats.realMaxPsi && psi_reached_timer >= 50)
	{
		sprintf_s(it_res_c, 9, "%d/%d", App->player->stats.psi, App->player->stats.maxPsi);
		res_lab_2->SetText(it_res_c, 255,0,0);
	}
	// Normal Display
	else
	{
		sprintf_s(it_res_c, 9, "%d/%d", App->player->stats.psi, App->player->stats.maxPsi);
		res_lab_2->SetText(it_res_c);
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
	spawn_text_name_3->SetActive(true);
	win_text_3->SetActive(true);
	SDL_Texture* use = NULL;

	if (defeat && App->render->movingCamera == false)
	{
		if (zergSample->state != BS_DEAD)
		{
			scriptTimer.Start();
			zergSample->StartDeath();
		}

		if (scriptTimer.ReadSec() > 3)
		{
			App->entityManager->stopLoop = true;
			//App->entityManager->FreezeInput();
			App->minimap->Disable();
			gameFinished = true;
			use = victoryT = App->tex->Load("graphics/gui/defeatScreenTMP.png");
			App->audio->PlayMusic("sounds/music/ambient/defeat.ogg", 1.0f);
		}
	}
	//Else if
	if (victory && App->render->movingCamera == false)
	{
		onEvent = onEventVictory = true;
		if (onEventVictory && scriptTimer.ReadSec() > 11.0f)
		{
			App->entityManager->stopLoop = true;
			//App->entityManager->FreezeInput();
			App->minimap->Disable();
			gameFinished = true;
			App->audio->PlayMusic("sounds/music/ambient/victory.ogg", 1.0f);
			use = defeatT = App->tex->Load("graphics/gui/victoryScreenTMP.png");
		}
	}
	if (gameFinished)
	{
		finalScreen = App->gui->CreateUI_Image({ 0, 0,640, 480 }, use, { 0, 0, 0, 0 });
		finalScreen->SetLayer(3);
		finalScreen->AddListener(this);
	}


}
void S_SceneMap::AddBossBar()
{
	const Boss* kerr = App->entityManager->boss;
	//const UnitStatsData* stats_k = App->entityManager->GetUnitStats(KERRIGAN);
	bossLife = App->gui->CreateUI_ProgressBar({ 0, 0, 0, 0 }, boss_life_barT, (int*)&kerr->maxHP, (int*)&kerr->currHP);

	bossShield = App->gui->CreateUI_ProgressBar({ 0, 0, 0, 0 }, boss_shield_barT, (int*)&kerr->stats.maxShield, (int*)&kerr->stats.shield);

	bossBase = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, boss_base_barT, { 0, 0, 0, 0 });
	bossShield->SetLayer(3);
	bossLife->SetLayer(3);
	bossBase->SetLayer(3);
	
}

