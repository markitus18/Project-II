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
	//We load all the textures on memory once, then we'll delete them at the end of the application
	LoadMenu1();

	App->audio->PlayMusic("sounds/sounds/menu/main-menu.wav");
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
	//Title
	//Background Image
	int w, h, scale;
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;
	scale = App->events->GetScale();

	title_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, title_tex, { 0, 0, 0, 0 });
	title_image->AddListener(this);
	title_image->SetActive(true);

	//Menu 1 Information
	//Background Image
	background_menu_1_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });

	//Info Image
	info_image = App->gui->CreateUI_Image({ -400, 0, 0, 0 }, info_tex, { 0, 0, 0, 0 });
	info_image->SetParent(background_menu_1_image);

	//Computer Label
	computer = App->gui->CreateUI_Label({ 60, 130, 0, 0 }, "computer", info_font, { 0, 0, 0, 0 });
	computer->SetParent(info_image);

	//Player Label
	player = App->gui->CreateUI_Label({ 60, 70, 0, 0 }, "Player", info_font, { 0, 0, 0, 0 });
	player->SetParent(info_image);

	//Zerg Label
	zerg = App->gui->CreateUI_Label({ 105, 150, 0, 0 }, "zerg", info_font, { 0, 0, 0, 0 });
	zerg->SetParent(info_image);

	//Protoss Label
	protoss = App->gui->CreateUI_Label({ 105, 90, 0, 0 }, "protoss", info_font, { 0, 0, 0, 0 });
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

	//Menu 2 Enter your name
	//Background Image
	background_menu_2_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });

	//Enter name image
	enter_name_image = App->gui->CreateUI_Image({ 200, 120, w / scale - 400, h / scale - 330 }, enter_name_tex, { 0, 0, 0, 0 });
	enter_name_image->SetParent(background_menu_2_image);

	//Enter name done label
	done = App->gui->CreateUI_Label({ 80, 92, 50, 20 }, "Done", info_font, { -23, -4, 85, 22 });
	done->AddListener(this);
	done->SetParent(enter_name_image);

	//Enter name input box
	enter_name_text = App->gui->CreateUI_InputText(230, 156, "Enter your name here", { -15, -2, 170, 19 }, 0, 0);
	enter_name_text->SetParent(background_menu_2_image);

	//Single Player Image Animated
	//single_player_image_animation = App->gui->CreateUI_AnimatedImage({ 0, 0, 256, 144 }, array, 5, 4.f, { 0, 0, 0, 0 });
	

	//Menu 3 Select load or new game
	//Background
	background_menu_3_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });

	//Image
	load_new_game_image = App->gui->CreateUI_Image({ -400, 0, 0, 0 }, info_tex, { 0, 0, 0, 0 });
	load_new_game_image->SetParent(background_menu_3_image);

	//New game label
	new_game_label = App->gui->CreateUI_Label({ 50, 80, 50, 20 }, "New game", info_font, { -1, -1, 90, 16 });
	new_game_label->AddListener(this);
	new_game_label->SetParent(load_new_game_image);

	//Load game label
	load_label = App->gui->CreateUI_Label({ 50, 110, 50, 20 }, "Load game", info_font, { -1, -1, 100, 16 });
	load_label->AddListener(this);
	load_label->SetParent(load_new_game_image);


	background_menu_1_image->SetActive(false);
	background_menu_2_image->SetActive(false);
	background_menu_3_image->SetActive(false);
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
	
	//Delete UI Elements
	App->gui->DeleteUIElement(cursor);
	App->gui->DeleteUIElement(title_image);
	App->gui->DeleteUIElement(background_menu_1_image);
	App->gui->DeleteUIElement(background_menu_2_image);
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


	//Unload textures
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
	//App->font->UnLoad(info_font);
	
	return true;
}

void S_SceneMenu::OnGUI(GUI_EVENTS event, UI_Element* element)
{
	/*if (element == single_player_button && event == UI_MOUSE_DOWN)
	{
		background_menu_1_image->SetActive(false);
		background_menu_2_image->SetActive(true);
	}*/

	if (element == ok && event == UI_MOUSE_DOWN)
	{
		background_menu_1_image->SetActive(false);
		//background_menu_2_image->SetActive(true);
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
		background_menu_3_image->SetActive(false);
		background_menu_2_image->SetActive(true);
	}

	if (element == load_label && event == UI_MOUSE_DOWN)
	{
		background_menu_3_image->SetActive(false);
		background_menu_1_image->SetActive(true);
		create2 = true;
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


	if (element == cancel && event == UI_MOUSE_DOWN)
	{
		wantToQuit = true;
	}
}

