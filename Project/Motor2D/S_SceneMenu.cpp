#include "S_SceneMenu.h"

#include "j1App.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_GUI.h"
//#include "M_EntityManager.h"
#include "M_Input.h"
#include "S_SceneMap.h"
#include "M_Window.h"

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
	//We load all the textures on memory once, then we'll delete them at the end of the application
	LoadMenu1();

	App->audio->PlayMusic("sounds/sounds/menu/main-menu.wav");
	App->input->DisableCursorImage();

	startTimerDelay.Start();
	create = false;
	wantToQuit = false;

	return true;
}

void S_SceneMenu::ManageInput(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
	{
		App->gui->debug = !App->gui->debug;
	}

	
}
void S_SceneMenu::LoadMenu1()
{
	//Title
	//Background Image
	int w, h, scale;
	App->win->GetWindowSize(&w, &h);
	scale = App->win->GetScale();

	title_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, title_tex, { 0, 0, 0, 0 });
	title_image->AddListener(this);
	title_image->SetActive(true);

	//Menu 1
	//Background Image
	background_menu_1_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });

	//Info Image
	info_image = App->gui->CreateUI_Image({ -400, 0, 0, 0 }, info_tex, { 0, 0, 0, 0 });
	info_image->SetParent(background_menu_1_image);

	//Computer Label
	computer = App->gui->CreateUI_Label({ 60, 130, 0, 0 }, "computer", info_font, { 0, 0, 0, 0 });
	computer->SetParent(info_image);

	//Player Label
	player = App->gui->CreateUI_Label({ 60, 70, 0, 0 }, "player", info_font, { 0, 0, 0, 0 });
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
	map_image->SetParent(map_info_image);

	//Map border
	map_border = App->gui->CreateUI_Image({ 30, 60, 165, 160 }, frame, { 0, 0, 0, 0 });
	map_border->SetParent(map_info_image);

	// Info
	UI_Label* versus = App->gui->CreateUI_Label({ 40, 165, 0, 0 }, "1 vs 1", info_font, { 0, 0, 0, 0 });
	versus->SetParent(map_image);

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

	//
	descriptionPanel = App->gui->CreateUI_Image({ -400, h / scale - 180, 392, 189 }, description, { 0, 0, 0, 0 });
	descriptionPanel->SetParent(background_menu_1_image);


	background_menu_1_image->SetActive(false);


	//Menu 2
	//Background Image
	background_menu_2_image = App->gui->CreateUI_Image({ 0, 0, w / scale, h / scale }, background_menu_tex, { 0, 0, 0, 0 });

	//Single Player Image Animated
	//single_player_image_animation = App->gui->CreateUI_AnimatedImage({ 0, 0, 256, 144 }, array, 5, 4.f, { 0, 0, 0, 0 });
	background_menu_2_image->SetActive(false);

}

bool S_SceneMenu::Update(float dt)
{
	int w, h, scale;
	App->win->GetWindowSize(&w, &h);
	scale = App->win->GetScale();

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
	int mouseX, mouseY;
	App->input->GetMousePosition(mouseX, mouseY);
	cursor->localPosition.x = mouseX;
	cursor->localPosition.y = mouseY;

	//Active the Menu 1 after 6 seconds from the start
	if (create == false && startTimerDelay.ReadSec() >= seconds)
	{
		title_image->SetActive(false);
		background_menu_1_image->SetActive(true);
		create = true;
	}

	if (!m_play && create)
	{
		// No suena musica en la pantalla de carga. Almenos no esta

		m_play = true;
	}		

	//The way to move the player info to the right at the menu 1
	if (create == true && info_image->localPosition.x < 0)
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

	if (create == true && descriptionPanel->localPosition.x < 0)
	{
	if (descriptionPanel->localPosition.x < -200)
		{
			descriptionPanel->localPosition.x += 9;
		}
	else if (descriptionPanel->localPosition.x < -70)
		{
			descriptionPanel->localPosition.x += 7;
		}
	else if (descriptionPanel->localPosition.x < -40)
		{
			descriptionPanel->localPosition.x += 6;
		}
	else if (descriptionPanel->localPosition.x < -20)
		{
			descriptionPanel->localPosition.x += 5;
		}
	else if (descriptionPanel->localPosition.x < -5)
		{
			descriptionPanel->localPosition.x += 2;
		}
		else
		{
			descriptionPanel->localPosition.x++;
		}
	}
	
	if (create == true && map_info_image->localPosition.x > w / scale - 230)
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

	if (create == true && ok_image->localPosition.x > w / scale - 195)
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

	if (create == true && cancel_image->localPosition.x > w / scale - 160)
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



	ManageInput(dt);
	return !wantToQuit;
}

bool S_SceneMenu::PostUpdate()
{
	return true;
}

bool S_SceneMenu::CleanUp()
{
	App->input->EnableCursorImage();
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
	App->gui->DeleteUIElement(descriptionPanel);
	App->gui->DeleteUIElement(start_button);
	App->gui->DeleteUIElement(computer);
	App->gui->DeleteUIElement(player);
	App->gui->DeleteUIElement(protoss);
	App->gui->DeleteUIElement(zerg);
	App->gui->DeleteUIElement(vs_1);
	App->gui->DeleteUIElement(ok);
	App->gui->DeleteUIElement(cancel);
	App->gui->DeleteUIElement(map_name);


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
		background_menu_1_image->SetActive(true);
		create = true;
	}

	if (element == cancel && event == UI_MOUSE_DOWN)
	{
		wantToQuit = true;
	}
}

