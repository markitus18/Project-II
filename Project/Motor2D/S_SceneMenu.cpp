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
	//We load all the textures on memory once, then we'll delete them at the end of the application
	LoadMenu1();

	return true;
}

void S_SceneMenu::ManageInput(float dt)
{

	//Change resources
	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_REPEAT)
	{
		player.mineral -= 5;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_REPEAT)
	{
		player.mineral += 7;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_REPEAT)
	{
		player.gas -= 5;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_REPEAT)
	{
		player.gas += 7;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_3) == KEY_REPEAT)
	{
		player.psi -= 5;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_REPEAT)
	{
		player.psi += 7;
	}
}
void S_SceneMenu::LoadMenu1()
{
	//Background Image
	title_image = App->gui->CreateUI_Image({ 0, 0, 640, 480 }, title_tex, { 0, 0, 640, 480 });

}

bool S_SceneMenu::Update(float dt)
{
	/*char* text = new char[9];
	//Update resource display
	sprintf_s(text, 7, "%d", player.mineral);
	mineral_label->SetText(text);

	sprintf_s(text, 7, "%d", player.gas);
	gas_label->SetText(text);

	sprintf_s(text, 9, "%d/%d", player.psi, player.maxPsi);
	psi_label->SetText(text);
	*/
	ManageInput(dt);
	return true;
}

bool S_SceneMenu::PostUpdate()
{
	return true;
}
bool S_SceneMenu::CleanUp()
{
	App->gui->DeleteUIElement(title_image);

	App->tex->UnLoad(title_tex);
	return true;
}

