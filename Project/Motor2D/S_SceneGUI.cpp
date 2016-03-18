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
	consoleT = App->tex->Load("gui/pconsole.png");
	iconsT = App->tex->Load("gui/cmdicons.png");
	atlasT = App->tex->Load("gui/pcmdbtns.png");
	if (iconsT == NULL)
	{
		LOG("Error at loading ICONS texture");
	}
	if (atlasT == NULL)
	{
		LOG("Error at loading the ATLAS texture");
	}
	if (consoleT == NULL)
	{
		LOG("Error at loading TOSS-CONSOLE texture");
	}
	LoadGUI();
	return true;
}

void S_SceneGUI::LoadGUI()
{
	//Button Rect Measueres
	SDL_Rect idle { 1, 0, 33, 34 };
	SDL_Rect clicked{ 74, 1, 34, 34 };

	// Inserting the console Image
	console = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, consoleT, { 0, 0, 640, 480 });

	//Struct that all grids will use
	Grid_Coords coords;
	//Image iterator
	UI_Image* image_it = NULL;
	M_Orders* ptr = App->orders;
	M_GUI* gui = App->gui;

	//-----------------
	Grid3x3 nexus(coords);

	nexus.setOrder(ptr->o_GenProbe_toss, idle, clicked, 0, 0, *atlasT);

	image_it = gui->CreateUI_Image({ 0, 0, 0, 0 }, iconsT, SDL_Rect{ 468, 102, 32, 32 });
	image_it->SetParent(nexus.buttons[0]);

	nexus.buttons[0]->AddListener((j1Module*)App->orders);


	nexus.setOrder(ptr->o_Set_rallyPoint, idle, clicked, 1, 2, *atlasT);

	image_it = gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, iconsT, { 504, 544, 32, 32 });
	image_it->SetParent(nexus.buttons[1]);

	nexus.buttons[1]->AddListener((j1Module*)App->orders);

	nexus.buttons[0]->SetActive(false);
	nexus.buttons[1]->SetActive(false);
	//-----------------
	Grid3x3 basic_u(coords);

	basic_u.setOrder(ptr->o_Move,idle,clicked, 0, 0, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, icons, { 252, 442, 32, 32 });
	image_it->SetParent(basic_u.buttons[0]);

	basic_u.buttons[0]->AddListener((j1Module*)App->orders);

	basic_u.setOrder(ptr->o_Stop, idle, clicked, 0, 1, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 288, 442, 32, 32 });
	image_it->SetParent(basic_u.buttons[1]);

	basic_u.buttons[1]->AddListener((j1Module*)App->orders);

	basic_u.setOrder(ptr->o_Attack, idle, clicked, 0, 2, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 324, 442, 32, 32 });
	image_it->SetParent(basic_u.buttons[2]);

	basic_u.buttons[2]->AddListener((j1Module*)App->orders);

	basic_u.setOrder(ptr->o_Patrol, idle, clicked, 1, 0, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 576, 475, 32, 32 });
	image_it->SetParent(basic_u.buttons[3]);

	basic_u.buttons[3]->AddListener((j1Module*)App->orders);

	basic_u.setOrder(ptr->o_Hold_pos, idle, clicked, 1, 1, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 0, 510, 32, 32 });
	image_it->SetParent(basic_u.buttons[4]);

	basic_u.buttons[4]->AddListener((j1Module*)App->orders);
}

bool S_SceneGUI::Update(float dt)
{
	console->SetActive(false);
	return true;
}

bool S_SceneGUI::CleanUp()
{

	return true;
}