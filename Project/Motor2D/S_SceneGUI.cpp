#include "S_SceneGUI.h"

#include "j1App.h"
#include "M_Textures.h"
#include "M_GUI.h"
#include "M_Orders.h"

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
	controlPT = App->tex->Load("gui/pconsole.png");
	orderIconsT = App->tex->Load("gui/cmdicons.png");
	atlasT = App->tex->Load("gui/pcmdbtns.png");
	uiIconsT = App->tex->Load("gui/icons.png");
	//We load all the textures on memory once, then we'll delete them at the end of the application
	if (!loaded)
	{
		LoadGUI();
	}
	else
	{
		controlPanel->SetActive(true);
		coords->frame->SetActive(true);
		std::list<Grid3x3*>::iterator it = grids.begin();
		std::list<Grid3x3*>::iterator it2 = grids.begin();

		while (it != grids.end())
		{
			it._Ptr->_Myval->changeState(true);
		}

	}
		
	return true;
}

void S_SceneGUI::LoadGUI()
{
	//Minerals Image
	res_img[0] = App->gui->CreateUI_Image({ 436, 3, 0, 0 }, uiIconsT, { 0, 0, 14, 14 });

	res_img[1] = App->gui->CreateUI_Image({ 504, 3, 0, 0 }, uiIconsT, { 0, 42, 14, 14 });

	res_img[2] = App->gui->CreateUI_Image({ 572, 3, 0, 0 }, uiIconsT, { 0, 84, 14, 14 });

	res_lab[0] = App->gui->CreateUI_Label({ 452, 4, 0, 0 }, "0");

	res_lab[1] = App->gui->CreateUI_Label({ 520, 4, 0, 0 }, "0");

	res_lab[2] = App->gui->CreateUI_Label({ 588, 4, 0, 0 }, "0");
#pragma region Grids
	loaded = true;
	coords = new Grid_Coords;

	// Inserting the control Panel Image
	controlPanel = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, controlPT, { 0, 0, 640, 480 });

	//Image iterator
	UI_Image* image_it = NULL;
	UI_Button2* butt_it = NULL;

	//Makes the code cleaner
	M_Orders* ptr = App->orders;
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
	butt_it = nexus->setOrder(ptr->o_GenProbe_toss, idle, clicked, 0, 0, *atlasT);

	image_it = gui->CreateUI_Image({ 0, 0, 0, 0 }, orderIconsT, SDL_Rect{ 468, 102, 32, 32 });
	image_it->SetParent(nexus->buttons[0]);

	butt_it->son = image_it;

	nexus->buttons[0]->AddListener((j1Module*)App->orders);

	//------------
	butt_it = nexus->setOrder(ptr->o_Set_rallyPoint, idle, clicked, 1, 2, *atlasT);

	image_it = gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, orderIconsT, { 504, 544, 32, 32 });
	image_it->SetParent(nexus->buttons[1]);

	butt_it->son = image_it;

	nexus->buttons[1]->AddListener((j1Module*)App->orders);

	nexus->changeState(false);

	//Basic Unit
	Grid3x3* basic_u = new Grid3x3(*coords);

	grids.push_back(basic_u);
	currentGrid = basic_u;

	butt_it = basic_u->setOrder(ptr->o_Move, idle, clicked, 0, 0, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 252, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[0]);

	butt_it->son = image_it;

	basic_u->buttons[0]->AddListener((j1Module*)App->orders);

	//------------
	butt_it = basic_u->setOrder(ptr->o_Stop, idle, clicked, 0, 1, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 288, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[1]);

	butt_it->son = image_it;

	basic_u->buttons[1]->AddListener((j1Module*)App->orders);

	//------------
	butt_it = basic_u->setOrder(ptr->o_Attack, idle, clicked, 0, 2, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 324, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[2]);

	butt_it->son = image_it;

	basic_u->buttons[2]->AddListener((j1Module*)App->orders);

	//------------
	basic_u->setOrder(ptr->o_Patrol, idle, clicked, 1, 0, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 576, 475, 32, 32 });
	image_it->SetParent(basic_u->buttons[3]);

	butt_it->son = image_it;

	basic_u->buttons[3]->AddListener((j1Module*)App->orders);

	//------------
	basic_u->setOrder(ptr->o_Hold_pos, idle, clicked, 1, 1, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, orderIconsT, { 0, 510, 32, 32 });
	image_it->SetParent(basic_u->buttons[4]);

	butt_it->son = image_it;

	basic_u->buttons[4]->AddListener((j1Module*)App->orders);
#pragma endregion
}

bool S_SceneGUI::Update(float dt)
{
	//Update resource display
	sprintf_s(it_res_c, 7, "%d", min);
	res_lab[0]->SetText(it_res_c);

	sprintf_s(it_res_c, 7, "%d", gas);
	res_lab[1]->SetText(it_res_c);

	sprintf_s(it_res_c, 9, "%d/%d", pep, max_pep);
	res_lab[2]->SetText(it_res_c);
	

	//Change Grids
	std::list<Grid3x3*>::iterator it = grids.begin();
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		changeCurrentGrid(it._Ptr->_Myval);
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		it++;
		changeCurrentGrid(it._Ptr->_Myval);
	}

	//Change resources
	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_REPEAT)
	{
		min-=5;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_REPEAT)
	{
		min += 7;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_REPEAT)
	{
		gas -= 5;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_REPEAT)
	{
		gas += 7;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_3) == KEY_REPEAT)
	{
		pep -= 5;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_REPEAT)
	{
		pep += 7;
	}
	return true;
}

bool S_SceneGUI::CleanUp()
{
	//App->gui->UI_Elements.remove(controlPanel);
	//RELEASE(controlPanel);
	controlPanel->SetActive(false);

	std::list<Grid3x3*>::iterator it = grids.begin();
	std::list<Grid3x3*>::iterator it2 = grids.begin();
	
	while (it != grids.end())
	{
		it._Ptr->_Myval->changeState(false);
			//RELEASE(it._Ptr->_Myval);
		++it;
	}
	coords->frame->SetActive(false);
		//RELEASE(coords);

	App->tex->UnLoad(controlPT);
	App->tex->UnLoad(orderIconsT);
	App->tex->UnLoad(atlasT);
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