#include "S_SceneMenu.h"

#include "j1App.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_GUI.h"
#include "M_InputManager.h"
#include "S_SceneMap.h"
#include "M_IA.h"

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

	return true;
}

bool S_SceneMenu::Start()
{
	App->SetCurrentScene(this);
	cursorTimer = 0.0f;
	cursorTexture = App->tex->Load("graphics/ui/cursors/arrow.png");
	cursor = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, cursorTexture, { 63, 63, 20, 20 });
	UI_Elements.push_back(cursor);
	cursor->SetLayer(N_GUI_LAYERS - 1);
	App->IA->nBases = 4;

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
	loading_tex = App->tex->Load("graphics/map_loading_screen.png");
	//controlls texture
	controls_tex = App->tex->Load("graphics/ui/readyt/pdpopup2.png");
	border_tex2 = App->tex->Load("graphics/ui/readyt/tutbtn.png");
	selector_tex = App->tex->Load("graphics/ui/readyt/selection.png");

	LoadMenu1();

	App->audio->PlayMusic("sounds/music/menu/main-menu.ogg");

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
	UI_Elements.push_back(title_image);
	title_image->AddListener(this);
	title_image->SetActive(true);

#pragma endregion

#pragma region //Information menu

	//Background Image
	background_menu_1_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(background_menu_1_image);

	//Info Image
	info_image = App->gui->CreateUI_Image({ -400, 0, 0, 0 }, info_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(info_image);
	info_image->SetParent(background_menu_1_image);

	//Computer Label
	computer = App->gui->CreateUI_Label({ 60, 130, 0, 0 }, "Computer", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(computer);
	computer->SetParent(info_image);

	//Player Label
	player = App->gui->CreateUI_Label({ 60, 70, 0, 0 }, "Zeratul", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(player);
	player->SetParent(info_image);

	//Zerg Label
	zerg = App->gui->CreateUI_Label({ 105, 150, 0, 0 }, "Zerg", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(zerg);
	zerg->SetParent(info_image);

	//Protoss Label
	protoss = App->gui->CreateUI_Label({ 105, 90, 0, 0 }, "Protoss", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(protoss);
	protoss->SetParent(info_image); 



	//Map Info image
	map_info_image = App->gui->CreateUI_Image({w/scale, 0, 0, 0 }, map_info_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(map_info_image);
	map_info_image->SetParent(background_menu_1_image);

	//Map Image
	map_image = App->gui->CreateUI_Image({ 40, 70, 140, 140 }, map_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(map_image);
	map_image->AddListener(this);
	map_image->SetParent(map_info_image);

	//Map size
	map_size = App->gui->CreateUI_Label({ 10, 10, 0, 0 }, "192x192", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(map_size);
	map_size->SetParent(map_image);
	map_size->SetActive(false);

	//Map border
	map_border = App->gui->CreateUI_Image({ 30, 60, 165, 160 }, frame, { 0, 0, 0, 0 });
	UI_Elements.push_back(map_border);
	map_border->SetParent(map_info_image);

	//Map name Label
	map_name = App->gui->CreateUI_Label({ 5, 195, 0, 0 }, "Void's Comeback", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(map_name);
	map_name->SetParent(map_image);

	//
	dificulty_selector = App->gui->CreateUI_Image({ 130, 155, 0, 0 }, selector_tex, { 0, 0, 0, 0 });
	dificulty_selector->SetParent(map_image);
	dificulty_selector->SetActive(false);

	dif4 = App->gui->CreateUI_Rect({ 135, 160, 30, 30 }, 255, 255, 255, 0);
	dif4->SetParent(map_image);
	dif4->AddListener(this);
	dif4->SetActive(false);

	dif3 = App->gui->CreateUI_Rect({ 102, 160, 30, 30 }, 255, 255, 255, 0);
	dif3->SetParent(map_image);
	dif3->AddListener(this);
	dif3->SetActive(false);

	dif2 = App->gui->CreateUI_Rect({ 70, 160, 30, 30 }, 255, 255, 255, 0);
	dif2->SetParent(map_image);
	dif2->AddListener(this);
	dif2->SetActive(false);


	//Cancel image and button
	cancel_image = App->gui->CreateUI_Image({ w / scale, h / scale - 100, 0, 0 }, cancel_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(cancel_image);
	cancel_image->SetParent(background_menu_1_image);

	//Cancel Label
	cancel = App->gui->CreateUI_Label({ 50, 65, 50, 20 }, "Quit", info_font, { -45, -3, 140, 25 });
	UI_Elements.push_back(cancel);
	cancel->AddListener(this);
	cancel->SetParent(cancel_image);


	//OK Image and button
	ok_image = App->gui->CreateUI_Image({ w / scale, h / scale - 125, 0, 0 }, ok_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(ok_image);
	ok_image->SetParent(background_menu_1_image);

	//OK Label
	ok = App->gui->CreateUI_Label({ 20, 48, 50, 20 }, "Launch game", info_font, { -20, -5, 160, 25 });
	UI_Elements.push_back(ok);
	ok->AddListener(this);
	ok->SetParent(ok_image);

	//Description panel
	description_panel = App->gui->CreateUI_Image({ -400, h / scale - 180, 392, 189 }, description, { 0, 0, 0, 0 });
	UI_Elements.push_back(description_panel);
	description_panel->SetParent(background_menu_1_image);
#pragma endregion

#pragma region //Enter name menu

	//Background Image
	background_menu_2_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(background_menu_2_image);

	//Enter name image
	enter_name_image = App->gui->CreateUI_Image({ (w / scale*(460 / 1280.0f)), h / scale*(240 / 960.0f), 1200 / scale - 400, 900 / scale - 330 }, enter_name_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(enter_name_image);
	enter_name_image->SetParent(background_menu_2_image);

	//Enter name done label
	done = App->gui->CreateUI_Label({ 80, 92, 50, 20 }, "Done", info_font, { -23, -4, 85, 22 });
	UI_Elements.push_back(done);
	done->AddListener(this);
	done->SetParent(enter_name_image);

	//Enter name input box
	enter_name_text = App->gui->CreateUI_InputText(w / scale*(500 / 1280.0f), h / scale*(316 / 960.0f), "Enter your name here", { 0, 0, 170, 19 }, 0, 0);
	enter_name_text->maxCharacters = 15;
	UI_Elements.push_back(enter_name_text);
	enter_name_text->SetParent(background_menu_2_image);
#pragma endregion
	
#pragma region //new game & load game menu
	//Background
	background_menu_3_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(background_menu_3_image);

	//Image
	load_new_game_image = App->gui->CreateUI_Image({ -400, 0, 0, 0 }, info_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(load_new_game_image);
	load_new_game_image->SetParent(background_menu_3_image);

	//New game label
	new_game_label = App->gui->CreateUI_Label({ 70, 130, 50, 20 }, "New game", info_font, { -8, -8, 110, 28 });
	UI_Elements.push_back(new_game_label);
	new_game_label->AddListener(this);
	new_game_label->SetParent(load_new_game_image);

	//New game border
	new_game_image = App->gui->CreateUI_Image({ -8, -8, 110, 28 }, border_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(new_game_image);
	new_game_image->SetParent(new_game_label);

	//Load game label
	load_label = App->gui->CreateUI_Label({ 200, 130, 50, 20 }, "Load game", info_font, { -8, -8, 120, 28 });
	UI_Elements.push_back(load_label);
	load_label->AddListener(this);
	load_label->SetParent(load_new_game_image);

	//Load game border
	load_game_image = App->gui->CreateUI_Image({ -8, -8, 120, 28 }, border_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(load_game_image);
	load_game_image->SetParent(load_label);

	//Open input manager image
	input_manager_image = App->gui->CreateUI_Image({ w / scale, h / scale - 125, 0, 0 }, ok_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(input_manager_image);
	input_manager_image->SetParent(background_menu_3_image);

	//Open input manager label
	input_manager_label = App->gui->CreateUI_Label({ 4, 50, 50, 20 }, "Change main keys", info_font, { -2, -5, 160, 25 });
	UI_Elements.push_back(input_manager_label);
	input_manager_label->AddListener(this);
	input_manager_label->SetParent(input_manager_image);

	//Cancel image and button
	back_image = App->gui->CreateUI_Image({ w / scale, h / scale - 100, 0, 0 }, cancel_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(back_image);
	back_image->SetParent(background_menu_3_image);

	//Cancel Label
	back_label = App->gui->CreateUI_Label({ 50, 65, 50, 20 }, "Quit", info_font, { -45, -3, 140, 25 });
	UI_Elements.push_back(back_label);
	back_label->AddListener(this);
	back_label->SetParent(back_image);

#pragma endregion

#pragma region //Menu load games
	//Background
	background_menu_4_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(background_menu_4_image);

	//Image
	select_game_image = App->gui->CreateUI_Image({ -400, -20, 350, 400 }, info_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(select_game_image);
	select_game_image->SetParent(background_menu_4_image);

	//Labels
	std::vector<std::string> saveGames;
	App->GetSaveGames(saveGames);
	nOfSaveGames = saveGames.size();

	save_games = new UI_Label*[nOfSaveGames];

	for (int n = 0; n < nOfSaveGames; n++)
	{
		save_games[n] = App->gui->CreateUI_Label({ 50, 110 + 20*n, 50, 20 }, "Slot 1", info_font, { 0, 0, 250, 15 });
		UI_Elements.push_back(save_games[n]);
		save_games[n]->AddListener(this);
		save_games[n]->SetParent(select_game_image);
	}

	//Cancel image and button
	back_image_4 = App->gui->CreateUI_Image({ w / scale, h / scale - 100, 0, 0 }, cancel_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(back_image_4);
	back_image_4->SetParent(background_menu_4_image);

	//Cancel Label
	back_label_4 = App->gui->CreateUI_Label({ 50, 65, 50, 20 }, "Back", info_font, { -45, -3, 140, 25 });
	UI_Elements.push_back(back_label_4);
	back_label_4->AddListener(this);
	back_label_4->SetParent(back_image_4);
#pragma endregion

#pragma region //Change controls menu
	//Create change controls menu
	controls_image = App->gui->CreateUI_Image({ (w / 2 - 300) / scale, (h / 2 - 250) / scale, 700 / scale, 400 / scale }, controls_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(controls_image);

	open_menu = App->gui->CreateUI_Label({ 35 / scale, 30 / scale, 0, 0 }, "Open Menu", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(open_menu);
	open_menu->SetParent(controls_image);

	open_console = App->gui->CreateUI_Label({ 35 / scale, 70 / scale, 0, 0 }, "Open Console", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(open_console);
	open_console->SetParent(controls_image);

	activate_debug = App->gui->CreateUI_Label({ 35 / scale, 110 / scale, 0, 0 }, "Activate Debug Mode", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(activate_debug);
	activate_debug->SetParent(controls_image);

	camera_up = App->gui->CreateUI_Label({ 35 / scale, 150 / scale, 0, 0 }, "Move camera up", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(camera_up);
	camera_up->SetParent(controls_image);

	camera_down = App->gui->CreateUI_Label({ 35 / scale, 190 / scale, 0, 0 }, "Move camera down", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(camera_down);
	camera_down->SetParent(controls_image);

	camera_right = App->gui->CreateUI_Label({ 35 / scale, 230 / scale, 0, 0 }, "Move camera right", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(camera_right);
	camera_right->SetParent(controls_image);

	camera_left = App->gui->CreateUI_Label({ 35 / scale, 270 / scale, 0, 0 }, "Move camera left", info_font, { 0, 0, 0, 0 });
	UI_Elements.push_back(camera_left);
	camera_left->SetParent(controls_image);

	ok_label = App->gui->CreateUI_Label({ 310 / scale, 335 / scale, 0, 0 }, "OK", info_font, { -60, -9, 150, 29 });
	UI_Elements.push_back(ok_label);
	ok_label->AddListener(this);
	ok_label->SetParent(controls_image);
#pragma endregion

#pragma region //event labels

	//Event labels
	open_menu_event = App->gui->CreateUI_Label({ 420 / scale, 30 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_OPEN_MENU), info_font, { -5, 0, 110, 13 });
	UI_Elements.push_back(open_menu_event);
	open_menu_event->AddListener(this);
	open_menu_event->SetParent(controls_image);
	open_menu_event_border = App->gui->CreateUI_Image({ -5, -2, 110, 18 }, border_tex2, { 0, 0, 0, 0 });
	UI_Elements.push_back(open_menu_event_border);
	open_menu_event_border->SetParent(open_menu_event);

	open_console_event = App->gui->CreateUI_Label({ 420 / scale, 70 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_OPEN_CONSOLE), info_font, { -5, 0, 110, 12 });
	UI_Elements.push_back(open_console_event);
	open_console_event->AddListener(this);
	open_console_event->SetParent(controls_image);
	open_console_event_border = App->gui->CreateUI_Image({ -5, -2, 110, 18 }, border_tex2, { 0, 0, 0, 0 });
	UI_Elements.push_back(open_console_event_border);
	open_console_event_border->SetParent(open_console_event);

	activate_debug_event = App->gui->CreateUI_Label({ 420 / scale, 110 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_DEBUG_ENTITY_MANAGER), info_font, { -5, 0, 110, 12 });
	UI_Elements.push_back(activate_debug_event);
	activate_debug_event->AddListener(this);
	activate_debug_event->SetParent(controls_image);
	activate_debug_event_border = App->gui->CreateUI_Image({ -5, -2, 110, 18 }, border_tex2, { 0, 0, 0, 0 });
	UI_Elements.push_back(activate_debug_event_border);
	activate_debug_event_border->SetParent(activate_debug_event);

	camera_up_event = App->gui->CreateUI_Label({ 420 / scale, 150 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_CAMERA_UP), info_font, { -5, 0, 110, 12 });
	UI_Elements.push_back(camera_up_event);
	camera_up_event->AddListener(this);
	camera_up_event->SetParent(controls_image);
	camera_up_event_border = App->gui->CreateUI_Image({ -5, -2, 110, 18 }, border_tex2, { 0, 0, 0, 0 });
	UI_Elements.push_back(camera_up_event_border);
	camera_up_event_border->SetParent(camera_up_event);

	camera_down_event = App->gui->CreateUI_Label({ 420 / scale, 190 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_CAMERA_DOWN), info_font, { -5, 0, 110, 12 });
	UI_Elements.push_back(camera_down_event);
	camera_down_event->AddListener(this);
	camera_down_event->SetParent(controls_image);
	camera_down_event_border = App->gui->CreateUI_Image({ -5, -2, 110, 18 }, border_tex2, { 0, 0, 0, 0 });
	UI_Elements.push_back(camera_down_event_border);
	camera_down_event_border->SetParent(camera_down_event);

	camera_right_event = App->gui->CreateUI_Label({ 420 / scale, 230 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_CAMERA_RIGHT), info_font, { -5, 0, 110, 12 });
	UI_Elements.push_back(camera_right_event);
	camera_right_event->AddListener(this);
	camera_right_event->SetParent(controls_image);
	camera_right_event_border = App->gui->CreateUI_Image({ -5, -2, 110, 18 }, border_tex2, { 0, 0, 0, 0 });
	UI_Elements.push_back(camera_right_event_border);
	camera_right_event_border->SetParent(camera_right_event);

	camera_left_event = App->gui->CreateUI_Label({ 420 / scale, 270 / scale, 0, 0 }, (char*)App->events->GetEventKeyName(E_CAMERA_LEFT), info_font, { -5, 0, 110, 12 });
	UI_Elements.push_back(camera_left_event);
	camera_left_event->AddListener(this);
	camera_left_event->SetParent(controls_image);
	camera_left_event_border = App->gui->CreateUI_Image({ -5, -2, 110, 18 }, border_tex2, { 0, 0, 0, 0 });
	UI_Elements.push_back(camera_left_event_border);
	camera_left_event_border->SetParent(camera_left_event);

	//---------------------------------------------------
#pragma endregion

#pragma region //Loading image
	loading_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, loading_tex, { 0, 0, 0, 0 });
	UI_Elements.push_back(loading_image);

#pragma endregion

	App->GetSaveGames(vector);

	controls_image->SetActive(false);
	background_menu_1_image->SetActive(false);
	background_menu_2_image->SetActive(false);
	background_menu_3_image->SetActive(false);
	background_menu_4_image->SetActive(false);
	loading_image->SetActive(false);
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
	App->audio->StopMusic();
	RELEASE_ARRAY(save_games);

#pragma region //Delete UI elements
	for (std::vector<UI_Element*>::iterator it = UI_Elements.begin(); it != UI_Elements.end(); it++)
	{
		App->gui->DeleteUIElement((*it));
	}
	UI_Elements.clear();

#pragma endregion

	App->font->Unload(info_font);

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
	App->tex->UnLoad(loading_tex);
	App->tex->UnLoad(selector_tex);
#pragma endregion

	vector.clear();
	//App->font->UnLoad(info_font);
	
	return true;
}

void S_SceneMenu::OnGUI(GUI_EVENTS event, UI_Element* element)
{
	if (element == dif4 && event == UI_MOUSE_DOWN)
	{
		App->IA->nBases = 4;
		dificulty_selector->localPosition = { dif4->localPosition.x - 5, dif4->localPosition.y - 5, 0, 0 };
	}
	if (element == dif3 && event == UI_MOUSE_DOWN)
	{
		App->IA->nBases = 3;
		dificulty_selector->localPosition = { dif3->localPosition.x - 5, dif3->localPosition.y - 5, 0, 0 };
	}
	if (element == dif2 && event == UI_MOUSE_DOWN)
	{
		App->IA->nBases = 2;
		dificulty_selector->localPosition = { dif2->localPosition.x - 5, dif2->localPosition.y - 5, 0, 0 };
	}

	if (element == ok && event == UI_MOUSE_DOWN)
	{
		background_menu_1_image->SetActive(false);
		cursor->SetActive(false);
		loading_image->SetActive(true);
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

	if (element == load_label && event == UI_MOUSE_DOWN)
	{
		if (controls == false)
		{
			for (int i = 0; i < nOfSaveGames && i < vector.size(); i++)
			{
				save_games[i]->SetText(vector[i].c_str());
			}
			background_menu_3_image->SetActive(false);
			background_menu_4_image->SetActive(true);
			create3 = true;
		}
	}

	if (element == done && event == UI_MOUSE_DOWN)
	{
		background_menu_2_image->SetActive(false);

		if (enter_name_text->text.GetText() == "Enter your name here"){
			enter_name_text->text.SetText("Player");
		}
			player->SetText(enter_name_text->text.GetText().c_str());
			App->player_name = enter_name_text->text.GetText();
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

	for (int i = 0; i < nOfSaveGames && i < vector.size(); i++){
		if (element == save_games[i] && event == UI_MOUSE_DOWN)
		{
			App->LoadGame(vector[i].c_str());
			background_menu_4_image->SetActive(false);
			loading_image->SetActive(true);
			cursor->SetActive(false);
		}
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

