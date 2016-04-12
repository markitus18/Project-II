#include "S_SceneMenu.h"

#include "j1App.h"
#include "M_Textures.h"
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
	info_tex = App->tex->Load("graphics/ui/readyt/p2terr.png");
	map_tex = App->tex->Load("maps/graphic.png");
	map_info_tex = App->tex->Load("graphics/ui/readyt/p3terr.png");
	info_font = App->font->Load("fonts/open_sans/OpenSans-Bold.ttf");
	//We load all the textures on memory once, then we'll delete them at the end of the application
	LoadMenu1();

	return true;
}

void S_SceneMenu::ManageInput(float dt)
{

	
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
	info_image = App->gui->CreateUI_Image({ -500, -30, 700, 400 }, info_tex, { 0, 0, 0, 0 });
	info_image->SetParent(background_menu_1_image);

	//Computer Label
	computer = App->gui->CreateUI_Label({ 300, 110, 0, 0 }, "computer", info_font, { 0, 0, 0, 0 });
	computer->SetParent(info_image);

	//Player Label
	player = App->gui->CreateUI_Label({ 300, 160, 0, 0 }, "player", info_font, { 0, 0, 0, 0 });
	player->SetParent(info_image);

	//Zerg Label
	zerg = App->gui->CreateUI_Label({ 330, 130, 0, 0 }, "zerg", info_font, { 0, 0, 0, 0 });
	zerg->SetParent(info_image);

	//Protoss Label
	protoss = App->gui->CreateUI_Label({ 330, 180, 0, 0 }, "protoss", info_font, { 0, 0, 0, 0 });
	protoss->SetParent(info_image); 

	//Start Label
	start = App->gui->CreateUI_Label({ 380, 290, 50, 20 }, "Start", info_font, { 0, 0, 60, 20 });
	start->AddListener(this);
	start->SetParent(info_image);

	//Back Label
	back = App->gui->CreateUI_Label({ 300, 290, 50, 20 }, "Back", info_font, { 0, 0, 60, 20 });
	back->AddListener(this);
	back->SetParent(info_image);

	//Map Info image
	map_info_image = App->gui->CreateUI_Image({ 640, -30, 400, 400 }, map_info_tex, { 0, 0, 0, 0 });
	map_info_image->SetParent(background_menu_1_image);

	//Map Image
	map_image = App->gui->CreateUI_Image({ 100, 100, 140, 140 }, map_tex, { 0, 0, 0, 0 });
	map_image->SetParent(map_info_image);

	//Map name Label
	map_name = App->gui->CreateUI_Label({ 20, 150, 50, 20 }, "Void's Comeback", info_font, { 0, 0, 0, 0 });
	map_name->SetParent(map_image);

	background_menu_1_image->SetActive(false);

	//Start Button
	start_button = App->gui->CreateUI_Button({ 400, 400, 256, 144 }, { 0, 0, 0, 0 }, { 50, 50, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 });
	//start_button->AddListener(this);
	start_button->SetParent(background_menu_1_image);


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
	
	//The way to move the player info to the right at the menu 1
	if (create == true && info_image->localPosition.x < -260)
	{
		if (info_image->localPosition.x < -320)
		{
			info_image->localPosition.x++;
			info_image->localPosition.x++;
			info_image->localPosition.x++;
			info_image->localPosition.x++;
			info_image->localPosition.x++;
		}
		if (info_image->localPosition.x < -270)
		{
			info_image->localPosition.x++;
			info_image->localPosition.x++;
			info_image->localPosition.x++;
			info_image->localPosition.x++;
		}
		if (info_image->localPosition.x < -245)
		{
			info_image->localPosition.x++;
			info_image->localPosition.x++;
			info_image->localPosition.x++;
		}
		if (info_image->localPosition.x < -235)
		{
			info_image->localPosition.x++;
			info_image->localPosition.x++;
		}
		if (info_image->localPosition.x < -225)
		{
			info_image->localPosition.x++;
		}
		info_image->localPosition.x++;		
	}

	//The way to move the map to the left at the menu 1
	if (create == true && map_info_image->localPosition.x > 340)
	{
		if (map_info_image->localPosition.x > 540)
		{
			map_info_image->localPosition.x--;
			map_info_image->localPosition.x--;
			map_info_image->localPosition.x--;
			map_info_image->localPosition.x--;
			map_info_image->localPosition.x--;
		}
		if (map_info_image->localPosition.x > 400)
		{
			map_info_image->localPosition.x--;
			map_info_image->localPosition.x--;
			map_info_image->localPosition.x--;
			map_info_image->localPosition.x--;
		}
		if (map_info_image->localPosition.x > 360)
		{
			map_info_image->localPosition.x--;
			map_info_image->localPosition.x--;
			map_info_image->localPosition.x--;
		}
		if (map_info_image->localPosition.x > 350)
		{
			map_info_image->localPosition.x--;
			map_info_image->localPosition.x--;
		}
		if (map_info_image->localPosition.x > 345)
		{
			map_info_image->localPosition.x--;
		}
		map_info_image->localPosition.x--;
	}


	ManageInput(dt);
	return true;
}

bool S_SceneMenu::PostUpdate()
{
	return true;
}
/*bool S_SceneMenu::CleanUp()
{
	//Delete UI Elements
	App->gui->DeleteUIElement(title_image);
	App->gui->DeleteUIElement(background_menu_1_image);
	App->gui->DeleteUIElement(single_player);
	App->gui->DeleteUIElement(single_player_image);
	//App->gui->DeleteUIElement(single_player_button);

	//Unload textures
	App->tex->UnLoad(title_tex);
	App->tex->UnLoad(background_menu_tex);
	App->tex->UnLoad(single_player_tex);

	//App->font->UnLoad(single_player_font);
	return true;
}*/

void S_SceneMenu::OnGUI(GUI_EVENTS event, UI_Element* element)
{
	/*if (element == single_player_button && event == UI_MOUSE_DOWN)
	{
		background_menu_1_image->SetActive(false);
		background_menu_2_image->SetActive(true);
	}*/

	if (element == start && event == UI_MOUSE_DOWN)
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

	if (element == back && event == UI_MOUSE_DOWN)
	{
		background_menu_1_image->SetActive(false);
		title_image->SetActive(true);
	}
}

