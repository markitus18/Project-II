#include "S_SceneMenu.h"

#include "j1App.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_GUI.h"
#include "M_EntityManager.h"
#include "M_Input.h"
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
	
	title_tex = App->tex->Load("graphics/ui/title.png");
	background_menu_tex = App->tex->Load("graphics/ui/Menu background without title.png");
	info_tex = App->tex->Load("graphics/ui/readyt/plistsml.png");
	map_tex = App->tex->Load("maps/graphic.png");
	map_info_tex = App->tex->Load("graphics/ui/readyt/pinfo2.png");
	ok_tex = App->tex->Load("graphics/ui/readyt/pok.png");
	cancel_tex = App->tex->Load("graphics/ui/readyt/pcancel.png");
	info_font = App->font->Load("fonts/open_sans/OpenSans-Bold.ttf");
	frame = App->tex->Load("graphics/ui/readyt/terrframe.png");
	description = App->tex->Load("graphics/ui/readyt/pchat.png");
	//We load all the textures on memory once, then we'll delete them at the end of the application
	LoadMenu1();

	//Music Load
	App->audio->PlayMusic("sounds/sounds/menu/main-menu.wav");

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
	title_image = App->gui->CreateUI_Image({ 0, 0, 640, 480 }, title_tex, { 0, 0, 640, 480 });
	title_image->AddListener(this);
	title_image->SetActive(true);

	//Menu 1
	//Background Image
	background_menu_1_image = App->gui->CreateUI_Image({ 0, 0, 640, 480 }, background_menu_tex, { 0, 0, 0, 0 });

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
	map_info_image = App->gui->CreateUI_Image({ 640, 0, 0, 0 }, map_info_tex, { 0, 0, 0, 0 });
	map_info_image->SetParent(background_menu_1_image);

	//Map Image
	map_image = App->gui->CreateUI_Image({ 40, 70, 140, 140 }, map_tex, { 0, 0, 0, 0 });
	map_image->SetParent(map_info_image);

	//Map border
	map_border = App->gui->CreateUI_Image({ 30, 60, 165, 160 }, frame, { 0, 0, 0, 0 });
	map_border->SetParent(map_info_image);

	// Info
	UI_Label* versus = App->gui->CreateUI_Label({ 50, 165, 0, 0 }, "1 vs 1", info_font, { 0, 0, 0, 0 });
	versus->SetParent(map_image);

	//Map name Label
	map_name = App->gui->CreateUI_Label({ 20, 195, 0, 0 }, "Void's Comeback", info_font, { 0, 0, 0, 0 });
	map_name->SetParent(map_image);

	//Cancel image and button
	cancel_image = App->gui->CreateUI_Image({ 475, 500, 0, 0 }, cancel_tex, { 0, 0, 0, 0 });
	cancel_image->SetParent(background_menu_1_image);

	//Cancel Label
	cancel = App->gui->CreateUI_Label({ 50, 65, 50, 20 }, "Back", info_font, { -45, -3, 140, 25 });
	cancel->AddListener(this);
	cancel->SetParent(cancel_image);

	//OK Image and button
	ok_image = App->gui->CreateUI_Image({ 640, 350, 0, 0 }, ok_tex, { 0, 0, 0, 0});
	ok_image->SetParent(background_menu_1_image);

	//OK Label
	ok = App->gui->CreateUI_Label({ 40, 48, 50, 20 }, "Launch game", info_font, { -65, -5, 155, 22 });
	ok->AddListener(this);
	ok->SetParent(ok_image);

	descriptionPanel = App->gui->CreateUI_Image({ 0, 275, 0, 0 }, description, { 0, 0, 0, 0 });
	descriptionPanel->SetParent(background_menu_1_image);


	background_menu_1_image->SetActive(false);


	//Menu 2
	//Background Image
	background_menu_2_image = App->gui->CreateUI_Image({ 0, 0, 640, 480 }, background_menu_tex, { 0, 0, 640, 480 });

	//Single Player Image Animated
	//single_player_image_animation = App->gui->CreateUI_AnimatedImage({ 0, 0, 256, 144 }, array, 5, 4.f, { 0, 0, 0, 0 });
	background_menu_2_image->SetActive(false);

}

bool S_SceneMenu::Update(float dt)
{

	//Active the Menu 1 after 6 seconds from the start
	if (create == false && App->GetTimeSinceStart() >= seconds)
	{
		title_image->SetActive(false);
		background_menu_1_image->SetActive(true);
		create = true;
	}

	if (m_play == false && create == true)
	{
		//App->audio->PlayFx(background_music, -1);
		m_play = true;
	}

	//The way to move the player info to the right at the menu 1
	if (create == true && info_image->localPosition.x < 0)
	{
		if (info_image->localPosition.x < -200)
		{
			info_image->localPosition.x += 9;
		}
		if (info_image->localPosition.x < -40)
		{
			info_image->localPosition.x += 7;
		}
		if (info_image->localPosition.x < -25)
		{
			info_image->localPosition.x += 6;
		}
		if (info_image->localPosition.x < -15)
		{
			info_image->localPosition.x += 5;
		}
		if (info_image->localPosition.x < -2)
		{
			info_image->localPosition.x++;
		}
		info_image->localPosition.x++;		
	}

	//The way to move the map to the left at the menu 1
	if (create == true && map_info_image->localPosition.x > 400)
	{
		if (map_info_image->localPosition.x > 600)
		{
			map_info_image->localPosition.x-=10;
		}
		if (map_info_image->localPosition.x > 440)
		{
			map_info_image->localPosition.x -= 8;
		}
		if (map_info_image->localPosition.x > 420)
		{
			map_info_image->localPosition.x -=7;
		}
		if (map_info_image->localPosition.x > 410)
		{
			map_info_image->localPosition.x -= 5;
		}
		if (map_info_image->localPosition.x > 405)
		{
			map_info_image->localPosition.x -= 2;
		}
		map_info_image->localPosition.x--;
	}

	//The way to move OK button to the left at the Menu 1
	if (create == true && ok_image->localPosition.x > 438)
	{
		if (ok_image->localPosition.x > 638)
		{
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
		}
		if (ok_image->localPosition.x > 478)
		{
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
		}
		if (ok_image->localPosition.x > 458)
		{
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
		}
		if (ok_image->localPosition.x > 448)
		{
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
		}
		if (ok_image->localPosition.x > 443)
		{
			ok_image->localPosition.x--;
			ok_image->localPosition.x--;
		}
		ok_image->localPosition.x--;
	}

	//The way to move the Cancel button up at the menu 1
	if (create == true && cancel_image->localPosition.y > 365)
	{
		if (cancel_image->localPosition.x > 535)
		{
			cancel_image->localPosition.y--;
			cancel_image->localPosition.y--;
			cancel_image->localPosition.y--;
			cancel_image->localPosition.y--;
			cancel_image->localPosition.y--;
		}
		if (cancel_image->localPosition.x > 435)
		{
			cancel_image->localPosition.y--;
			cancel_image->localPosition.y--;
			cancel_image->localPosition.y--;
			cancel_image->localPosition.y--;
		}
		if (cancel_image->localPosition.x > 385)
		{
			cancel_image->localPosition.y--;
			cancel_image->localPosition.y--;
			cancel_image->localPosition.y--;
		}
		if (cancel_image->localPosition.x > 375)
		{
			cancel_image->localPosition.y--;
			cancel_image->localPosition.y--;
		}
		if (cancel_image->localPosition.x > 370)
		{
			cancel_image->localPosition.y--;
		}
		ok_image->localPosition.y--;
	}



	ManageInput(dt);
	return true;
}

bool S_SceneMenu::PostUpdate()
{
	return true;
}

bool S_SceneMenu::CleanUp()
{

	App->audio->StopMusic();
	
	//Delete UI Elements
	/*
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
	*/

	//Unload textures
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
		App->sceneMap->Enable();
		App->sceneMenu->Disable();
	}

	if (element == title_image && event == UI_MOUSE_DOWN)
	{
		title_image->SetActive(false);
		background_menu_1_image->SetActive(true);
		create = true;
	}

	if (element == cancel && event == UI_MOUSE_DOWN)
	{
		background_menu_1_image->SetActive(false);
		title_image->SetActive(true);
	}
}

