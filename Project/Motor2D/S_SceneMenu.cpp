#include "S_SceneMenu.h"

#include "j1App.h"
#include "M_Textures.h"
#include "M_GUI.h"
#include "M_EntityManager.h"
#include "M_Input.h"

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
	//We load all the textures on memory once, then we'll delete them at the end of the application
	LoadMenu1();

	return true;
}

void S_SceneMenu::ManageInput(float dt)
{

	
}
void S_SceneMenu::LoadMenu1()
{
	//Background Image
	title_image = App->gui->CreateUI_Image({ 0, 0, 640, 480 }, title_tex, { 0, 0, 640, 480 });


}

bool S_SceneMenu::Update(float dt)
{
	if (App->GetTimeSinceStart() >= 8)
	{
		background_image = App->gui->CreateUI_Image({ 0, 0, 640, 480 }, background_menu_tex, { 0, 0, 640, 480 });
		//single_player = App->gui->CreateUI_Label({ 250, 250, 256, 144 }, "SINGLE PLAYER", single_player_font, { 0, 0, 256, 144 });
		single_player_image = App->gui->CreateUI_Image({ 0, 0, 256, 144 }, single_player_tex, { 0, 0, 256, 144 });
		//single_player_button = App->gui->CreateUI_Button({ 0, 0, 256, 144 }, { 0, 0, 256, 144 }, { 50, 50, 206, 94 }, { 0, 0, 256, 144 }, { 0, 0, 0, 0 });
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
	if (title_image != NULL)
	{
		App->gui->DeleteUIElement(title_image);
	}

	App->gui->DeleteUIElement(background_image);


	App->tex->UnLoad(title_tex);
	App->tex->UnLoad(background_menu_tex);
	return true;
}

