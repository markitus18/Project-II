#include "S_SceneGUI.h"

#include "j1App.h"
#include "M_Textures.h"
#include "M_GUI.h"
#include "M_Orders.h"

S_SceneGUI::S_SceneGUI(bool at_start) : j1Module(at_start)
{
	name.create("scene_gui");
}

// Called before render is available
bool S_SceneGUI::Awake(pugi::xml_node& node)
{
	LOG("Loading Scene");
	App->SetCurrentScene(this);

	return true;
}

bool S_SceneGUI::Start()
{
	console = App->tex->Load("gui/pconsole.png");
	icons = App->tex->Load("gui/cmdicons.png");
	atlas = App->tex->Load("gui/pcmdbtns.png");
	if (icons == NULL)
	{
		LOG("Error at loading ICONS texture");
	}
	if (atlas == NULL)
	{
		LOG("Error at loading the ATLAS texture");
	}
	if (console == NULL)
	{
		LOG("Error at loading TOSS-CONSOLE texture");
	}
		//LoadGUI();
	return true;
}

void S_SceneGUI::LoadGUI()
{
	// Inserting the console Image

	App->gui->CreateUI_Image({ 0, 0, 0, 0 }, console, { 0, 0, 640, 480 });

	//Struct that all grids will use
	Grid_Coords coords;

	//Testing buttons
	Grid3x3 nexus(coords);

	UI_Button2* buttonIT = nexus.setOrder(App->orders->o_genProbe_toss, { 0, 0, 33, 34 }, { 74, 0, 34, 34 }, 0, 0, *atlas, true);

	UI_Image* image = App->gui->CreateUI_Image(SDL_Rect{ 0, 0, 0, 0 }, icons, SDL_Rect{ 468, 102, 32, 32 }, SDL_Rect{ 0, 0, 0, 0 });
	
		//image->SetParent(buttonIT);
		//nexus.buttons[0]->AddListener((j1Module*)App->orders);


	buttonIT = nexus.setOrder(App->orders->o_genProbe_toss, { 1, 0, 33, 34 }, { 74, 1, 34, 34 }, 1, 2, *atlas, true);
	
	UI_Image* attack = App->gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, icons, { 504, 544, 32, 32 }, SDL_Rect{ 0, 0, 0, 0 });
	attack->SetParent(buttonIT);
	buttonIT->AddListener((j1Module*)App->orders);

	//504 544 32,32
	/*UI_Button2* button2 = App->gui->CreateUI_Button2({ 0, 0, 0, 0 }, atlas, { 1, 0, 33, 34 }, { 74, 1, 33, 34 }, true);
	nexus.setOrder(App->orders->o_genProbe_toss, 0, 0, *button2);
	UI_Image* image = App->gui->CreateUI_Image(SDL_Rect{ 0, 0, 31, 31 }, icons, SDL_Rect{ 468, 102, 32, 32 }, SDL_Rect{ 0, 0, 0, 0 });
	image->SetParent(button2);
	button2->AddListener((j1Module*)App->orders);*/
}

bool S_SceneGUI::Update(float dt)
{
	return true;
}



bool S_SceneGUI::CleanUp()
{
	return true;
}