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
	single_player_tex = App->tex->Load("graphics/ui/readyt/p2terr.png");
	single_player_font = App->font->Load("fonts/mirage_gothic/mirage_gothic.ttf");
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
	title_image->SetActive(true);


	//Menu 1
	//Background Image
	background_menu_1_image = App->gui->CreateUI_Image({ 0, 0, 640, 480 }, background_menu_tex, { 0, 0, 640, 480 });

	//Single Player Label
	single_player = App->gui->CreateUI_Label({ 400, 400, 256, 144 }, "SINGLE PLAYER", single_player_font, { 0, 0, 256, 144 });
	single_player->SetParent(background_menu_1_image);

	//Single Player Image
	single_player_image = App->gui->CreateUI_Image({ -256, 0, 256, 144 }, single_player_tex, { 0, 0, 256, 144 });
	single_player_image->SetParent(background_menu_1_image);
	single_player_image->AddListener(this);
	background_menu_1_image->SetActive(false);

	//Single Player Button
	//single_player_button = App->gui->CreateUI_Button({ 100, 100, 256, 144 }, "graphics/ui/readyt/p2terr.png", { 0, 0, 256, 144 }, { 50, 50, 206, 94 }, { 0, 0, 256, 144 }, { 0, 0, 256, 144 });
	//single_player_button->AddListener(this);
	//single_player_button->SetParent(background_menu_1_image);


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
	if (create == false && App->GetTimeSinceStart() >= 1)
	{
		title_image->SetActive(false);
		background_menu_1_image->SetActive(true);
		create = true;
	}
	//Esto lo ha hecho Moya just sayin
	if (create == true && single_player_image->localPosition.x < 0)
	{
		if (single_player_image->localPosition.x < -100)
		{
			single_player_image->localPosition.x++;
			single_player_image->localPosition.x++;
			single_player_image->localPosition.x++;
			single_player_image->localPosition.x++;
			single_player_image->localPosition.x++;
		}
		if (single_player_image->localPosition.x < -50)
		{
			single_player_image->localPosition.x++;
			single_player_image->localPosition.x++;
			single_player_image->localPosition.x++;
			single_player_image->localPosition.x++;
		}
		if (single_player_image->localPosition.x < -25)
		{
			single_player_image->localPosition.x++;
			single_player_image->localPosition.x++;
			single_player_image->localPosition.x++;
		}
		if (single_player_image->localPosition.x < -15)
		{
			single_player_image->localPosition.x++;
			single_player_image->localPosition.x++;
		}
		if (single_player_image->localPosition.x < -15)
		{
			single_player_image->localPosition.x++;
		}
		single_player_image->localPosition.x++;
		
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
	if (element == single_player_button && event == UI_MOUSE_DOWN)
	{
		background_menu_1_image->SetActive(false);
		background_menu_2_image->SetActive(true);
	}
	if (element == single_player_image && event == UI_MOUSE_DOWN)
	{
		background_menu_1_image->SetActive(false);
		//background_menu_2_image->SetActive(true);
		App->sceneMap->Enable();
		App->sceneMenu->Disable();
	}
}

