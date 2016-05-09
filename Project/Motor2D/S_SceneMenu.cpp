#include "S_SceneMenu.h"

#include "j1App.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_GUI.h"
#include "M_InputManager.h"
#include "S_SceneMap.h"

S_SceneMenu::S_SceneMenu(bool at_start) : j1Module(at_start)
{
	name.create("scene_menu");
}

// Destructor
S_SceneMenu::~S_SceneMenu()
{}

// Called before render is available
bool S_SceneMenu::Awake(pugi::xml_node& node)
{
	LOG("Loading Scene");
	App->SetCurrentScene(this);

	return true;
}

bool S_SceneMenu::Start()
{
	cursorTimer = 0.0f;
	cursorTexture = App->tex->Load("graphics/ui/cursors/arrow.png");
	cursor = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, cursorTexture, { 63, 63, 20, 20 });
	cursor->SetLayer(N_GUI_LAYERS - 1);

	title_tex = App->tex->Load("graphics/ui/title.png");
	background_menu_tex = App->tex->Load("graphics/ui/Menu background without title.png");
	info_tex = App->tex->Load("graphics/ui/readyt/plistsml.png");
	map_tex = App->tex->Load("maps/graphic.png");
	map_info_tex = App->tex->Load("graphics/ui/readyt/pinfo2.png");
	ok_tex = App->tex->Load("graphics/ui/readyt/pok.png");
	cancel_tex = App->tex->Load("graphics/ui/readyt/pcancel.png");
	info_font = App->font->Load("fonts/StarCraft.ttf", 14);
	frame = App->tex->Load("graphics/ui/readyt/terrframe.png");
	description = App->tex->Load("graphics/ui/readyt/pchat2.png");
	enter_name_tex = App->tex->Load("graphics/ui/readyt/pstatus.png");
	border_tex = App->tex->Load("graphics/ui/readyt/tframeh4.png");
	dark_tex = App->tex->Load("graphics/ui/readyt/dark.png");
	//controlls texture
	controls_tex = App->tex->Load("graphics/ui/readyt/pdpopup2.png");
	border_tex2 = App->tex->Load("graphics/ui/readyt/tutbtn.png");
	//We load all the textures on memory once, then we'll delete them at the end of the application
	LoadMenu1();

	App->audio->PlayMusic("sounds/music/menu/main-menu.wav");
	App->events->EnableCursorImage(false);

	startTimerDelay.Start();
	create = false;
	wantToQuit = false;

	return true;
}

void S_SceneMenu::ManageInput(float dt)
{
	if (App->events->GetEvent(E_DEBUG_UI) == EVENT_DOWN)
	{
		App->gui->debug = !App->gui->debug;
	}

	
}
void S_SceneMenu::LoadMenu1()
{
#pragma region //Title

	//Background Image
	int w, h, scale;
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;
	scale = App->events->GetScale();

	title_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, title_tex, { 0, 0, 0, 0 });
	title_image->AddListener(this);
	title_image->SetActive(true);

#pragma endregion

#pragma region //Information menu

	//Background Image
	background_menu_1_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });

	//Info Image
	info_image = App->gui->CreateUI_Image({ -400, 0, 0, 0 }, info_tex, { 0, 0, 0, 0 });
	info_image->SetParent(background_menu_1_image);

	//Computer Label
	computer = App->gui->CreateUI_Label({ 60, 130, 0, 0 }, "Computer", info_font, { 0, 0, 0, 0 });
	computer->SetParent(info_image);

	//Player Label
	player = App->gui->CreateUI_Label({ 60, 70, 0, 0 }, "Player", info_font, { 0, 0, 0, 0 });
	player->SetParent(info_image);

	//Zerg Label
	zerg = App->gui->CreateUI_Label({ 105, 150, 0, 0 }, "Zerg", info_font, { 0, 0, 0, 0 });
	zerg->SetParent(info_image);

	//Protoss Label
	protoss = App->gui->CreateUI_Label({ 105, 90, 0, 0 }, "Protoss", info_font, { 0, 0, 0, 0 });
	protoss->SetParent(info_image); 



	//Map Info image
	map_info_image = App->gui->CreateUI_Image({w/scale, 0, 0, 0 }, map_info_tex, { 0, 0, 0, 0 });
	map_info_image->SetParent(background_menu_1_image);

	//Map Image
	map_image = App->gui->CreateUI_Image({ 40, 70, 140, 140 }, map_tex, { 0, 0, 0, 0 });
	map_image->AddListener(this);
	map_image->SetParent(map_info_image);

	//Map size
	map_size = App->gui->CreateUI_Label({ 10, 10, 0, 0 }, "192x192", info_font, { 0, 0, 0, 0 });
	map_size->SetParent(map_image);
	map_size->SetActive(false);

	//Map border
	map_border = App->gui->CreateUI_Image({ 30, 60, 165, 160 }, frame, { 0, 0, 0, 0 });
	map_border->SetParent(map_info_image);

	// Info
	vs_1 = App->gui->CreateUI_Label({ 40, 165, 0, 0 }, "1 vs 1", info_font, { 0, 0, 0, 0 });
	vs_1->SetParent(map_image);

	//Map name Label
	map_name = App->gui->CreateUI_Label({ 5, 195, 0, 0 }, "Void's Comeback", info_font, { 0, 0, 0, 0 });
	map_name->SetParent(map_image);


	//Cancel image and button
	cancel_image = App->gui->CreateUI_Image({ w / scale, h / scale - 100, 0, 0 }, cancel_tex, { 0, 0, 0, 0 });
	cancel_image->SetParent(background_menu_1_image);

	//Cancel Label
	cancel = App->gui->CreateUI_Label({ 50, 65, 50, 20 }, "Quit", info_font, { -45, -3, 140, 25 });
	cancel->AddListener(this);
	cancel->SetParent(cancel_image);


	//OK Image and button
	ok_image = App->gui->CreateUI_Image({ w / scale, h / scale - 125, 0, 0 }, ok_tex, { 0, 0, 0, 0 });
	ok_image->SetParent(background_menu_1_image);

	//OK Label
	ok = App->gui->CreateUI_Label({ 20, 48, 50, 20 }, "Launch game", info_font, { -20, -5, 160, 25 });
	ok->AddListener(this);
	ok->SetParent(ok_image);

	//Description panel
	description_panel = App->gui->CreateUI_Image({ -400, h / scale - 180, 392, 189 }, description, { 0, 0, 0, 0 });
	description_panel->SetParent(background_menu_1_image);
#pragma endregion

#pragma region //Enter name menu

	//Background Image
	background_menu_2_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });

	//Enter name image
	enter_name_image = App->gui->CreateUI_Image({ 200, 120, 1200 / scale - 400, 900 / scale - 330 }, enter_name_tex, { 0, 0, 0, 0 });
	enter_name_image->SetParent(background_menu_2_image);

	//Enter name done label
	done = App->gui->CreateUI_Label({ 80, 92, 50, 20 }, "Done", info_font, { -23, -4, 85, 22 });
	done->AddListener(this);
	done->SetParent(enter_name_image);

	//Enter name input box
	enter_name_text = App->gui->CreateUI_InputText(230, 156, "Enter your name here", { -15, -2, 170, 19 }, 0, 0);
	enter_name_text->SetParent(background_menu_2_image);
#pragma endregion
	
#pragma region //new game & load game menu
	//Background
	background_menu_3_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });

	//Image
	load_new_game_image = App->gui->CreateUI_Image({ -400, 0, 0, 0 }, info_tex, { 0, 0, 0, 0 });
	load_new_game_image->SetParent(background_menu_3_image);

	//New game label
	new_game_label = App->gui->CreateUI_Label({ 70, 130, 50, 20 }, "New game", info_font, { -8, -8, 110, 28 });
	new_game_label->AddListener(this);
	new_game_label->SetParent(load_new_game_image);

	//New game border
	new_game_image = App->gui->CreateUI_Image({ -8, -8, 110, 28 }, border_tex, { 0, 0, 0, 0 });
	new_game_image->SetParent(new_game_label);

	//Load game label
	load_label = App->gui->CreateUI_Label({ 200, 130, 50, 20 }, "Load game", info_font, { -8, -8, 120, 28 });
	load_label->AddListener(this);
	load_label->SetParent(load_new_game_image);

	//Load game border
	load_game_image = App->gui->CreateUI_Image({ -8, -8, 120, 28 }, border_tex, { 0, 0, 0, 0 });
	load_game_image->SetParent(load_label);

	//Load game undone
	dark_image = App->gui->CreateUI_Image({ 0, 0, 128, 36 }, dark_tex, { 0, 0, 0, 0 });
	dark_image->SetParent(load_game_image);

	//Soon label
	soon_label = App->gui->CreateUI_Label({ 230, 130, 50, 20 }, "Soon", info_font, { 0, 0, 0, 0 });
	soon_label->SetActive(false);

	//Open input manager image
	input_manager_image = App->gui->CreateUI_Image({ w / scale, h / scale - 125, 0, 0 }, ok_tex, { 0, 0, 0, 0 });
	input_manager_image->SetParent(background_menu_3_image);

	//Open input manager label
	input_manager_label = App->gui->CreateUI_Label({ 4, 50, 50, 20 }, "Change main keys", info_font, { -2, -5, 160, 25 });
	input_manager_label->AddListener(this);
	input_manager_label->SetParent(input_manager_image);

	//Cancel image and button
	back_image = App->gui->CreateUI_Image({ w / scale, h / scale - 100, 0, 0 }, cancel_tex, { 0, 0, 0, 0 });
	back_image->SetParent(background_menu_3_image);

	//Cancel Label
	back_label = App->gui->CreateUI_Label({ 50, 65, 50, 20 }, "Quit", info_font, { -45, -3, 140, 25 });
	back_label->AddListener(this);
	back_label->SetParent(back_image);

#pragma endregion

#pragma region //Menu load games
	//Background
	background_menu_4_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });

	//Image
	select_game_image = App->gui->CreateUI_Image({ -400, 0, 0, 0 }, info_tex, { 0, 0, 0, 0 });
	select_game_image->SetParent(background_menu_4_image);

	//Labels
	name_label_1 = App->gui->CreateUI_Label({ 70, 80, 50, 20 }, "Game 1", info_font, { 0, 0, 60, 15 });
	name_label_1->AddListener(this);
	name_label_1->SetParent(select_game_image);

	name_label_2 = App->gui->CreateUI_Label({ 70, 110, 50, 20 }, "Game 2", info_font, { 0, 0, 60, 15 });
	name_label_2->AddListener(this);
	name_label_2->SetParent(select_game_image);

	name_label_3 = App->gui->CreateUI_Label({ 70, 140, 50, 20 }, "Game 3", info_font, { 0, 0, 60, 15 });
	name_label_3->AddListener(this);
	name_label_3->SetParent(select_game_image);

	name_label_4 = App->gui->CreateUI_Label({ 70, 170, 50, 20 }, "Game 4", info_font, { 0, 0, 60, 15 });
	name_label_4->AddListener(this);
	name_label_4->SetParent(select_game_image);

	//Cancel image and button
	back_image_4 = App->gui->CreateUI_Image({ w / scale, h / scale - 100, 0, 0 }, cancel_tex, { 0, 0, 0, 0 });
	back_image_4->SetParent(background_menu_4_image);

	//Cancel Label
	back_label_4 = App->gui->CreateUI_Label({ 50, 65, 50, 20 }, "Back", info_font, { -45, -3, 140, 25 });
	back_label_4->AddListener(this);
	back_label_4->SetParent(back_image_4);
#pragma endregion

#pragma region //Change controls menu
	//Create change controls menu
	controls_image = App->gui->CreateUI_Image({ (w / 2 - 300) / scale, (h / 2 - 250) / scale, 700 / scale, 400 / scale }, controls_tex, { 0, 0, 0, 0 });

	open_menu = App->gui->CreateUI_Label({ 35 / scale, 30 / scale, 0, 0 }, "Open Menu", info_font, { 0, 0, 0, 0 });
	open_menu->SetParent(controls_image);

	save_game = App->gui->CreateUI_Label({ 35 / scale, 60 / scale, 0, 0 }, "Save Game", info_font, { 0, 0, 0, 0 });
	save_game->SetParent(controls_image);

	load_game = App->gui->CreateUI_Label({ 35 / scale, 90 / scale, 0, 0 }, "Load Game", info_font, { 0, 0, 0, 0 });
	load_game->SetParent(controls_image);

	open_console = App->gui->CreateUI_Label({ 35 / scale, 120 / scale, 0, 0 }, "Open Console", info_font, { 0, 0, 0, 0 });
	open_console->SetParent(controls_image);

	activate_debug = App->gui->CreateUI_Label({ 35 / scale, 150 / scale, 0, 0 }, "Activate Debug Mode", info_font, { 0, 0, 0, 0 });
	activate_debug->SetParent(controls_image);

	camera_up = App->gui->CreateUI_Label({ 35 / scale, 180 / scale, 0, 0 }, "Move camera up", info_font, { 0, 0, 0, 0 });
	camera_up->SetParent(controls_image);

	camera_down = App->gui->CreateUI_Label({ 35 / scale, 210 / scale, 0, 0 }, "Move camera down", info_font, { 0, 0, 0, 0 });
	camera_down->SetParent(controls_image);

	camera_right = App->gui->CreateUI_Label({ 35 / scale, 240 / scale, 0, 0 }, "Move camera right", info_font, { 0, 0, 0, 0 });
	camera_right->SetParent(controls_image);

	camera_left = App->gui->CreateUI_Label({ 35 / scale, 270 / scale, 0, 0 }, "Move camera left", info_font, { 0, 0, 0, 0 });
	camera_left->SetParent(controls_image);

	ok_label = App->gui->CreateUI_Label({ 310 / scale, 335 / scale, 0, 0 }, "OK", info_font, { -60, -9, 150, 29 });
	ok_label->AddListener(this);
	ok_label->SetParent(controls_image);
#pragma endregion

#pragma region //event labels

	//Event labels
	open_menu_event = App->gui->CreateUI_Label({ 420 / scale, 30 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_OPEN_MENU), info_font, { -5, 0, 110, 13 });
	open_menu_event->AddListener(this);
	open_menu_event->SetParent(controls_image);
	open_menu_event_border = App->gui->CreateUI_Image({ -5, 0, 110, 13 }, border_tex2, { 0, 0, 0, 0 });
	open_menu_event_border->SetParent(open_menu_event);

	save_game_event = App->gui->CreateUI_Label({ 420 / scale, 60 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_SAVE_GAME), info_font, { -5, 0, 110, 12 });
	save_game_event->AddListener(this);
	save_game_event->SetParent(controls_image);
	save_game_event_border = App->gui->CreateUI_Image({ -5, 0, 110, 13 }, border_tex2, { 0, 0, 0, 0 });
	save_game_event_border->SetParent(save_game_event);

	load_game_event = App->gui->CreateUI_Label({ 420 / scale, 90 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_LOAD_GAME), info_font, { -5, 0, 110, 12 });
	load_game_event->AddListener(this);
	load_game_event->SetParent(controls_image);
	load_game_event_border = App->gui->CreateUI_Image({ -5, 0, 110, 13 }, border_tex2, { 0, 0, 0, 0 });
	load_game_event_border->SetParent(load_game_event);

	open_console_event = App->gui->CreateUI_Label({ 420 / scale, 120 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_OPEN_CONSOLE), info_font, { -5, 0, 110, 12 });
	open_console_event->AddListener(this);
	open_console_event->SetParent(controls_image);
	open_console_event_border = App->gui->CreateUI_Image({ -5, 0, 110, 13 }, border_tex2, { 0, 0, 0, 0 });
	open_console_event_border->SetParent(open_console_event);

	activate_debug_event = App->gui->CreateUI_Label({ 420 / scale, 150 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_DEBUG_ENTITY_MANAGER), info_font, { -5, 0, 110, 12 });
	activate_debug_event->AddListener(this);
	activate_debug_event->SetParent(controls_image);
	activate_debug_event_border = App->gui->CreateUI_Image({ -5, 0, 110, 13 }, border_tex2, { 0, 0, 0, 0 });
	activate_debug_event_border->SetParent(activate_debug_event);

	camera_up_event = App->gui->CreateUI_Label({ 420 / scale, 180 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_CAMERA_UP), info_font, { -5, 0, 110, 12 });
	camera_up_event->AddListener(this);
	camera_up_event->SetParent(controls_image);
	camera_up_event_border = App->gui->CreateUI_Image({ -5, 0, 110, 13 }, border_tex2, { 0, 0, 0, 0 });
	camera_up_event_border->SetParent(camera_up_event);

	camera_down_event = App->gui->CreateUI_Label({ 420 / scale, 210 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_CAMERA_DOWN), info_font, { -5, 0, 110, 12 });
	camera_down_event->AddListener(this);
	camera_down_event->SetParent(controls_image);
	camera_down_event_border = App->gui->CreateUI_Image({ -5, 0, 110, 13 }, border_tex2, { 0, 0, 0, 0 });
	camera_down_event_border->SetParent(camera_down_event);

	camera_right_event = App->gui->CreateUI_Label({ 420 / scale, 240 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_CAMERA_RIGHT), info_font, { -5, 0, 110, 12 });
	camera_right_event->AddListener(this);
	camera_right_event->SetParent(controls_image);
	camera_right_event_border = App->gui->CreateUI_Image({ -5, 0, 110, 13 }, border_tex2, { 0, 0, 0, 0 });
	camera_right_event_border->SetParent(camera_right_event);

	camera_left_event = App->gui->CreateUI_Label({ 420 / scale, 270 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_CAMERA_LEFT), info_font, { -5, 0, 110, 12 });
	camera_left_event->AddListener(this);
	camera_left_event->SetParent(controls_image);
	camera_left_event_border = App->gui->CreateUI_Image({ -5, 0, 110, 13 }, border_tex2, { 0, 0, 0, 0 });
	camera_left_event_border->SetParent(camera_left_event);

	//---------------------------------------------------
#pragma endregion



	controls_image->SetActive(false);
	background_menu_1_image->SetActive(false);
	background_menu_2_image->SetActive(false);
	background_menu_3_image->SetActive(false);
	background_menu_4_image->SetActive(false);
}

bool S_SceneMenu::Update(float dt)
{
	int w, h, scale;
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;
	scale = App->events->GetScale();

	cursorTimer += dt;
	if (cursorTimer >= 0.15f)
	{
		cursorTimer = 0.0f;
		SDL_Rect tmp = cursor->getRect();
		tmp.y += 128;
		if (tmp.y > 600)
		{
			tmp.y = 63;
		}
		cursor->SetRect(tmp);
	}

	cursor->localPosition.x = App->events->GetMouseOnScreen().x;
	cursor->localPosition.y = App->events->GetMouseOnScreen().y;

	//Active the Menu 1 after 6 seconds from the start
	if (create == false && startTimerDelay.ReadSec() >= seconds)
	{
		title_image->SetActive(false);
		background_menu_3_image->SetActive(true);
		create = true;
	}

	if (!m_play && create)
	{
		// No suena musica en la pantalla de carga. Almenos no esta

		m_play = true;
	}		

#pragma region //Move images in the menu

	//The way to move the player info to the right at the menu 1
	if (create2 == true && info_image->localPosition.x < 0)
	{
		if (info_image->localPosition.x < -200)
		{
			info_image->localPosition.x += 9;
		}
		else if (info_image->localPosition.x < -70)
		{
			info_image->localPosition.x += 7;
		}
		else 	if (info_image->localPosition.x < -40)
		{
			info_image->localPosition.x += 6;
		}
		else if (info_image->localPosition.x < -20)
		{
			info_image->localPosition.x += 5;
		}
		else if (info_image->localPosition.x < -5)
		{
			info_image->localPosition.x += 2;
		}
		else
		{
			info_image->localPosition.x++;
		}
	}

	if (create2 == true && description_panel->localPosition.x < 0)
	{
		if (description_panel->localPosition.x < -200)
		{
			description_panel->localPosition.x += 9;
		}
		else if (description_panel->localPosition.x < -70)
		{
			description_panel->localPosition.x += 7;
		}
		else if (description_panel->localPosition.x < -40)
		{
			description_panel->localPosition.x += 6;
		}
		else if (description_panel->localPosition.x < -20)
		{
			description_panel->localPosition.x += 5;
		}
		else if (description_panel->localPosition.x < -5)
		{
			description_panel->localPosition.x += 2;
		}
		else
		{
			description_panel->localPosition.x++;
		}
	}
	
	if (create2 == true && map_info_image->localPosition.x > w / scale - 230)
	{
	 if (map_info_image->localPosition.x > -200)
		{
			map_info_image->localPosition.x -= 9;
		}
	else if (map_info_image->localPosition.x > -70)
		{
			map_info_image->localPosition.x -= 7;
		}
	else if (map_info_image->localPosition.x > -40)
		{
			map_info_image->localPosition.x -= 6;
		}
	else if (map_info_image->localPosition.x > -20)
		{
			map_info_image->localPosition.x -= 5;
		}
	else if (map_info_image->localPosition.x > -5)
		{
			map_info_image->localPosition.x -= 2;
		}
		else
		{
			map_info_image->localPosition.x--;
		}
	}

	if (create2 == true && ok_image->localPosition.x > w / scale - 195)
	{
	 if (ok_image->localPosition.x > -200)
		{
			ok_image->localPosition.x -= 9;
		}
	else if (ok_image->localPosition.x > -70)
		{
			ok_image->localPosition.x -= 7;
		}
	else if (ok_image->localPosition.x > -40)
		{
			ok_image->localPosition.x -= 6;
		}
	else if (ok_image->localPosition.x > -20)
		{
			ok_image->localPosition.x -= 5;
		}
	else if (ok_image->localPosition.x > -5)
		{
			ok_image->localPosition.x -= 2;
		}
		else
		{
			ok_image->localPosition.x--;
		}
	}

	if (create2 == true && cancel_image->localPosition.x > w / scale - 160)
	{
		if (cancel_image->localPosition.x > -200)
		{
			cancel_image->localPosition.x -= 9;
		}
		else if (cancel_image->localPosition.x > -70)
		{
			cancel_image->localPosition.x -= 7;
		}
		else if (cancel_image->localPosition.x > -40)
		{
			cancel_image->localPosition.x -= 6;
		}
		else if (cancel_image->localPosition.x > -20)
		{
			cancel_image->localPosition.x -= 5;
		}
		else if (cancel_image->localPosition.x > -5)
		{
			cancel_image->localPosition.x -= 2;
		}
		else
		{
			cancel_image->localPosition.x--;
		}
	}

	if (create == true && load_new_game_image->localPosition.x < 0)
	{
		if (load_new_game_image->localPosition.x < -200)
		{
			load_new_game_image->localPosition.x += 9;
		}
		else if (load_new_game_image->localPosition.x < -70)
		{
			load_new_game_image->localPosition.x += 7;
		}
		else 	if (load_new_game_image->localPosition.x < -40)
		{
			load_new_game_image->localPosition.x += 6;
		}
		else if (load_new_game_image->localPosition.x < -20)
		{
			load_new_game_image->localPosition.x += 5;
		}
		else if (load_new_game_image->localPosition.x < -5)
		{
			load_new_game_image->localPosition.x += 2;
		}
		else
		{
			load_new_game_image->localPosition.x++;
		}
	}

	if (create3 == true && select_game_image->localPosition.x < 0)
	{
		if (select_game_image->localPosition.x < -200)
		{
			select_game_image->localPosition.x += 9;
		}
		else if (select_game_image->localPosition.x < -70)
		{
			select_game_image->localPosition.x += 7;
		}
		else 	if (select_game_image->localPosition.x < -40)
		{
			select_game_image->localPosition.x += 6;
		}
		else if (select_game_image->localPosition.x < -20)
		{
			select_game_image->localPosition.x += 5;
		}
		else if (select_game_image->localPosition.x < -5)
		{
			select_game_image->localPosition.x += 2;
		}
		else
		{
			select_game_image->localPosition.x++;
		}
	}

	if (create == true && back_image->localPosition.x > w / scale - 160)
	{
		if (back_image->localPosition.x > -200)
		{
			back_image->localPosition.x -= 9;
		}
		else if (back_image->localPosition.x > -70)
		{
			back_image->localPosition.x -= 7;
		}
		else if (back_image->localPosition.x > -40)
		{
			back_image->localPosition.x -= 6;
		}
		else if (back_image->localPosition.x > -20)
		{
			back_image->localPosition.x -= 5;
		}
		else if (back_image->localPosition.x > -5)
		{
			back_image->localPosition.x -= 2;
		}
		else
		{
			back_image->localPosition.x--;
		}
	}

	if (create == true && input_manager_image->localPosition.x > w / scale - 195)
	{
		if (input_manager_image->localPosition.x > -200)
		{
			input_manager_image->localPosition.x -= 9;
		}
		else if (input_manager_image->localPosition.x > -70)
		{
			input_manager_image->localPosition.x -= 7;
		}
		else if (input_manager_image->localPosition.x > -40)
		{
			input_manager_image->localPosition.x -= 6;
		}
		else if (input_manager_image->localPosition.x > -20)
		{
			input_manager_image->localPosition.x -= 5;
		}
		else if (input_manager_image->localPosition.x > -5)
		{
			input_manager_image->localPosition.x -= 2;
		}
		else
		{
			input_manager_image->localPosition.x--;
		}
	}

	if (create3 == true && back_image_4->localPosition.x > w / scale - 160)
	{
		if (back_image_4->localPosition.x > -200)
		{
			back_image_4->localPosition.x -= 9;
		}
		else if (back_image_4->localPosition.x > -70)
		{
			back_image_4->localPosition.x -= 7;
		}
		else if (back_image_4->localPosition.x > -40)
		{
			back_image_4->localPosition.x -= 6;
		}
		else if (back_image_4->localPosition.x > -20)
		{
			back_image_4->localPosition.x -= 5;
		}
		else if (back_image_4->localPosition.x > -5)
		{
			back_image_4->localPosition.x -= 2;
		}
		else
		{
			back_image_4->localPosition.x--;
		}
	}
#pragma endregion

#pragma region //Updating labels that change key controls

	if (App->events->GetEvent(E_UPDATED_KEY) == EVENT_DOWN)
	{
		open_menu_event->SetText(App->events->GetEventKeyName(E_OPEN_MENU));
		save_game_event->SetText(App->events->GetEventKeyName(E_SAVE_GAME));
		load_game_event->SetText(App->events->GetEventKeyName(E_LOAD_GAME));
		open_console_event->SetText(App->events->GetEventKeyName(E_OPEN_CONSOLE));
		activate_debug_event->SetText(App->events->GetEventKeyName(E_DEBUG_ENTITY_MANAGER));
		camera_up_event->SetText(App->events->GetEventKeyName(E_CAMERA_UP));
		camera_right_event->SetText(App->events->GetEventKeyName(E_CAMERA_RIGHT));
		camera_down_event->SetText(App->events->GetEventKeyName(E_CAMERA_DOWN));
		camera_left_event->SetText(App->events->GetEventKeyName(E_CAMERA_LEFT));
	}

#pragma endregion


	ManageInput(dt);
	return !wantToQuit;
}

bool S_SceneMenu::PostUpdate()
{
	return true;
}

bool S_SceneMenu::CleanUp()
{
	App->events->EnableCursorImage(true);
	App->audio->StopMusic();

#pragma region //Delete UI elements
	
	App->gui->DeleteUIElement(cursor);
	App->gui->DeleteUIElement(title_image);
	App->gui->DeleteUIElement(background_menu_1_image);
	App->gui->DeleteUIElement(background_menu_2_image);
	App->gui->DeleteUIElement(background_menu_3_image);
	App->gui->DeleteUIElement(background_menu_4_image);
	App->gui->DeleteUIElement(info_image);
	App->gui->DeleteUIElement(map_image);
	App->gui->DeleteUIElement(map_border);
	App->gui->DeleteUIElement(map_info_image);
	App->gui->DeleteUIElement(ok_image);
	App->gui->DeleteUIElement(cancel_image);
	App->gui->DeleteUIElement(description_panel);
	App->gui->DeleteUIElement(computer);
	App->gui->DeleteUIElement(player);
	App->gui->DeleteUIElement(protoss);
	App->gui->DeleteUIElement(zerg);
	App->gui->DeleteUIElement(vs_1);
	App->gui->DeleteUIElement(ok);
	App->gui->DeleteUIElement(cancel);
	App->gui->DeleteUIElement(map_name);
	App->gui->DeleteUIElement(enter_name_image);
	App->gui->DeleteUIElement(enter_name_text);
	App->gui->DeleteUIElement(done);
	App->gui->DeleteUIElement(new_game_image);
	App->gui->DeleteUIElement(new_game_label);
	App->gui->DeleteUIElement(load_new_game_image);
	App->gui->DeleteUIElement(load_label);
	App->gui->DeleteUIElement(load_game_image);
	App->gui->DeleteUIElement(select_game_image);
	App->gui->DeleteUIElement(name_label_1);
	App->gui->DeleteUIElement(name_label_2);
	App->gui->DeleteUIElement(name_label_3);
	App->gui->DeleteUIElement(name_label_4);
	App->gui->DeleteUIElement(controls_image);
	App->gui->DeleteUIElement(open_menu);
	App->gui->DeleteUIElement(save_game);
	App->gui->DeleteUIElement(load_game);
	App->gui->DeleteUIElement(open_console);
	App->gui->DeleteUIElement(activate_debug);
	App->gui->DeleteUIElement(camera_up);
	App->gui->DeleteUIElement(camera_down);
	App->gui->DeleteUIElement(camera_right);
	App->gui->DeleteUIElement(camera_left);
	App->gui->DeleteUIElement(ok_label);
	App->gui->DeleteUIElement(open_menu_event);
	App->gui->DeleteUIElement(save_game_event);
	App->gui->DeleteUIElement(load_game_event);
	App->gui->DeleteUIElement(open_console_event);
	App->gui->DeleteUIElement(activate_debug_event);
	App->gui->DeleteUIElement(camera_up_event);
	App->gui->DeleteUIElement(camera_down_event);
	App->gui->DeleteUIElement(camera_right_event);
	App->gui->DeleteUIElement(camera_left_event);
	App->gui->DeleteUIElement(open_menu_event_border);
	App->gui->DeleteUIElement(save_game_event_border);
	App->gui->DeleteUIElement(load_game_event_border);
	App->gui->DeleteUIElement(open_console_event_border);
	App->gui->DeleteUIElement(activate_debug_event_border);
	App->gui->DeleteUIElement(camera_up_event_border);
	App->gui->DeleteUIElement(camera_down_event_border);
	App->gui->DeleteUIElement(camera_right_event_border);
	App->gui->DeleteUIElement(camera_left_event_border);
	App->gui->DeleteUIElement(back_image_4);
	App->gui->DeleteUIElement(back_label_4);
	App->gui->DeleteUIElement(input_manager_image);
	App->gui->DeleteUIElement(input_manager_label);
	App->gui->DeleteUIElement(back_image);
	App->gui->DeleteUIElement(back_label);
	App->gui->DeleteUIElement(soon_label);
#pragma endregion

#pragma region //Unload textures
	App->tex->UnLoad(cursorTexture);
	App->tex->UnLoad(title_tex);
	App->tex->UnLoad(background_menu_tex);
	App->tex->UnLoad(info_tex);
	App->tex->UnLoad(map_tex);
	App->tex->UnLoad(map_info_tex);
	App->tex->UnLoad(ok_tex);
	App->tex->UnLoad(cancel_tex);
	App->tex->UnLoad(frame);
	App->tex->UnLoad(description);
	App->tex->UnLoad(enter_name_tex);
	App->tex->UnLoad(border_tex);
	App->tex->UnLoad(controls_tex);
	App->tex->UnLoad(border_tex2);
#pragma endregion

	//App->font->UnLoad(info_font);
	
	return true;
}

void S_SceneMenu::OnGUI(GUI_EVENTS event, UI_Element* element)
{

	if (element == ok && event == UI_MOUSE_DOWN)
	{
		background_menu_1_image->SetActive(false);
		App->changeScene(App->sceneMap, this);
	}

	if (element == title_image && event == UI_MOUSE_DOWN && startTimerDelay.ReadSec() > 0.3f)
	{
		title_image->SetActive(false);
		background_menu_3_image->SetActive(true);
		create = true;
	}

	if (element == new_game_label && event == UI_MOUSE_DOWN)
	{
		if (controls == false){
			background_menu_3_image->SetActive(false);
			background_menu_2_image->SetActive(true);
		}
	}

	if (element == load_label && event == UI_MOUSE_ENTER)
	{
		if (controls == false){
			load_label->SetText(" ");
			soon_label->SetActive(true);
		}
	}

	if (element == load_label && event == UI_MOUSE_EXIT)
	{
		if (controls == false){
			soon_label->SetActive(false);
			load_label->SetText("Load game");
		}
	}

	if (element == done && event == UI_MOUSE_DOWN)
	{
		background_menu_2_image->SetActive(false);
		player->SetText(enter_name_text->text.GetText());
		background_menu_1_image->SetActive(true);
		map_size->SetActive(false);
		create2 = true;
	}

	if (element == map_image && event == UI_MOUSE_ENTER){
		map_size->SetActive(true);
	}

	if (element == map_image && event == UI_MOUSE_EXIT){
		map_size->SetActive(false);
	}

	if (element == back_label && event == UI_MOUSE_DOWN)
	{
		if (controls == false){
			wantToQuit = true;
		}
		
	}

	if (element == input_manager_label && event == UI_MOUSE_DOWN)
	{
		controls_image->SetActive(true);
		controls = true;
	}

	if (element == back_label_4 && event == UI_MOUSE_DOWN)
	{
		background_menu_4_image->SetActive(false);
		background_menu_3_image->SetActive(true);
		create3 = false;
	}

	if (element == cancel && event == UI_MOUSE_DOWN)
	{
		wantToQuit = true;
	}

	if (element == name_label_1 && event == UI_MOUSE_DOWN)
	{
		background_menu_4_image->SetActive(false);
		App->changeScene(App->sceneMap, this);
	}

	if (element == ok_label && event == UI_MOUSE_DOWN)
	{
		controls_image->SetActive(false);
		controls = false;
	}

#pragma region //Config keys
	if (controls_image && controls_image->IsActive() && event == UI_MOUSE_DOWN)
	{
		if (element == open_menu_event)
		{
			App->events->SetEventToNextKeyPress(E_OPEN_MENU);
		}
		if (element == save_game_event)
		{
			App->events->SetEventToNextKeyPress(E_SAVE_GAME);
		}
		if (element == load_game_event)
		{
			App->events->SetEventToNextKeyPress(E_LOAD_GAME);
		}
		if (element == open_console_event)
		{
			App->events->SetEventToNextKeyPress(E_OPEN_CONSOLE);
		}
		if (element == activate_debug_event)
		{
			App->events->SetEventToNextKeyPress(E_DEBUG_ENTITY_MANAGER);
		}
		if (element == camera_up_event)
		{
			App->events->SetEventToNextKeyPress(E_CAMERA_UP);
		}
		if (element == camera_down_event)
		{
			App->events->SetEventToNextKeyPress(E_CAMERA_DOWN);
		}
		if (element == camera_right_event)
		{
			App->events->SetEventToNextKeyPress(E_CAMERA_RIGHT);
		}
		if (element == camera_left_event)
		{
			App->events->SetEventToNextKeyPress(E_CAMERA_LEFT);
		}

	}


#pragma endregion

}

