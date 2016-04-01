#include "S_SceneGUI.h"

#include "j1App.h"
#include "M_Textures.h"
#include "M_GUI.h"
#include "M_EntityManager.h"
#include "M_Input.h"

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
	contorlPanel_tex = App->tex->Load("gui/pconsole.png");
	order_icons_tex = App->tex->Load("gui/cmdicons.png");
	buttons_back_tex = App->tex->Load("gui/pcmdbtns.png");
	resource_icons_tex = App->tex->Load("gui/icons.png");
	//We load all the textures on memory once, then we'll delete them at the end of the application

	LoadGUI();
		
	return true;
}

void S_SceneGUI::ManageInput(float dt)
{
	//Change Grids
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		changeCurrentGrid(grids[0]);
	}
	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		changeCurrentGrid(grids[1]);
	}

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
void S_SceneGUI::LoadGUI()
{
	//Minerals Image
	mineral_image = App->gui->CreateUI_Image({ 436, 3, 0, 0 }, resource_icons_tex, { 0, 0, 14, 14 });

	gas_image = App->gui->CreateUI_Image({ 504, 3, 0, 0 }, resource_icons_tex, { 0, 42, 14, 14 });

	psi_image = App->gui->CreateUI_Image({ 572, 3, 0, 0 }, resource_icons_tex, { 0, 84, 14, 14 });

	mineral_label = App->gui->CreateUI_Label({ 452, 4, 0, 0 }, "0");

	gas_label = App->gui->CreateUI_Label({ 520, 4, 0, 0 }, "0");

	psi_label = App->gui->CreateUI_Label({ 588, 4, 0, 0 }, "0");

#pragma region Grids
	coords = new Grid_Coords;

	// Inserting the control Panel Image
	controlPanel = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, contorlPanel_tex, { 0, 0, 640, 480 });

	//Image iterator
	UI_Image* image_it = NULL;
	UI_Button2* button_it = NULL;

	//Makes the code cleaner
	M_EntityManager* ptr = App->entityManager;
	M_GUI* gui = App->gui;
	//Grid 3x3 definition

	//Button Rect Measueres
	SDL_Rect idle{ 1, 0, 33, 34 };
	SDL_Rect clicked{ 74, 1, 33, 34 };

	//Initialize Grid 3x3 frame
	coords->frame->SetActive(true);

	//Nexus
	Grid3x3* nexus = new Grid3x3(*coords);
	grids.push_back(nexus);
	//------------
	button_it = nexus->setOrder(ptr->o_GenProbe_toss, idle, clicked, 0, 0, *buttons_back_tex);

	image_it = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, order_icons_tex, SDL_Rect{ 468, 102, 32, 32 });
	image_it->SetParent(nexus->buttons[0]);

	button_it->son = image_it;


	//------------
	button_it = nexus->setOrder(ptr->o_Set_rallyPoint, idle, clicked, 1, 2, *buttons_back_tex);

	image_it = App->gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, order_icons_tex, { 504, 544, 32, 32 });
	image_it->SetParent(nexus->buttons[1]);

	button_it->son = image_it;

	nexus->buttons[1]->AddListener((j1Module*)ptr);

	nexus->changeState(false);

	//Basic Unit
	Grid3x3* basic_unit = new Grid3x3(*coords);

	grids.push_back(basic_unit);
	currentGrid = basic_unit;

	button_it = basic_unit->setOrder(ptr->o_Move, idle, clicked, 0, 0, *buttons_back_tex, true);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, order_icons_tex, { 252, 442, 32, 32 });
	image_it->SetParent(basic_unit->buttons[0]);

	button_it->son = image_it;

	basic_unit->buttons[0]->AddListener((j1Module*)ptr);

	//------------
	button_it = basic_unit->setOrder(ptr->o_Stop, idle, clicked, 0, 1, *buttons_back_tex, true);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, order_icons_tex, { 288, 442, 32, 32 });
	image_it->SetParent(basic_unit->buttons[1]);

	button_it->son = image_it;

	basic_unit->buttons[1]->AddListener((j1Module*)ptr);

	//------------
	button_it = basic_unit->setOrder(ptr->o_Attack, idle, clicked, 0, 2, *buttons_back_tex, true);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, order_icons_tex, { 324, 442, 32, 32 });
	image_it->SetParent(basic_unit->buttons[2]);

	button_it->son = image_it;

	basic_unit->buttons[2]->AddListener((j1Module*)ptr);

	//------------
	basic_unit->setOrder(ptr->o_Patrol, idle, clicked, 1, 0, *buttons_back_tex, true);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, order_icons_tex, { 576, 475, 32, 32 });
	image_it->SetParent(basic_unit->buttons[3]);

	button_it->son = image_it;

	basic_unit->buttons[3]->AddListener((j1Module*)ptr);

	//------------
	basic_unit->setOrder(ptr->o_Hold_pos, idle, clicked, 1, 1, *buttons_back_tex, true);

	image_it = App->gui->CreateUI_Image({ 3, 3, 0, 0 }, order_icons_tex, { 0, 510, 32, 32 });
	image_it->SetParent(basic_unit->buttons[4]);

	button_it->son = image_it;

#pragma endregion
}

bool S_SceneGUI::Update(float dt)
{
	char* text = new char[9];
	//Update resource display
	sprintf_s(text, 7, "%d", player.mineral);
	mineral_label->SetText(text);

	sprintf_s(text, 7, "%d", player.gas);
	gas_label->SetText(text);

	sprintf_s(text, 9, "%d/%d", player.psi, player.maxPsi);
	psi_label->SetText(text);
	
	ManageInput(dt);
	return true;
}

bool S_SceneGUI::PostUpdate()
{
	return true;
}
bool S_SceneGUI::CleanUp()
{
	App->gui->DeleteUIElement(controlPanel);

	std::vector<Grid3x3*>::iterator it = grids.begin();
	while (it != grids.end())
	{
		RELEASE((*it));
		++it;
	}

	RELEASE(coords);

	App->tex->UnLoad(contorlPanel_tex);
	App->tex->UnLoad(order_icons_tex);
	App->tex->UnLoad(buttons_back_tex);
	return true;
}

bool S_SceneGUI::changeCurrentGrid(Grid3x3 * newCurrent)
{
	if (currentGrid == newCurrent)
		return false;

	if (currentGrid != NULL)
	{
		currentGrid->changeState(false);
	}
	if (newCurrent != NULL)
	{
		newCurrent->changeState(true);
	}
	currentGrid = newCurrent;

	return true;
}