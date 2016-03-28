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
	if (controlPT == NULL)
	{
		LOG("Error at loading TOSS-CONTROL-PANEL texture");
	}
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
	SDL_Rect clicked{ 74, 1, 34, 34 };

	//Initialize Grid 3x3 frame
	coords->frame->SetActive(true);

	//Nexus
	Grid3x3* nexus = new Grid3x3(*coords);
	grids.push_back(nexus);
	//------------
	butt_it = nexus->setOrder(ptr->o_GenProbe_toss, idle, clicked, 0, 0, *atlasT);

	image_it = gui->CreateUI_Image({ 0, 0, 0, 0 }, iconsT, SDL_Rect{ 468, 102, 32, 32 });
	image_it->SetParent(nexus->buttons[0]);

	butt_it->son = image_it;

	nexus->buttons[0]->AddListener((j1Module*)App->orders);

	//------------
	butt_it = nexus->setOrder(ptr->o_Set_rallyPoint, idle, clicked, 1, 2, *atlasT);

	image_it = gui->CreateUI_Image(SDL_Rect{ 3, 3, 0, 0 }, iconsT, { 504, 544, 32, 32 });
	image_it->SetParent(nexus->buttons[1]);

	butt_it->son = image_it;

	nexus->buttons[1]->AddListener((j1Module*)App->orders);

	nexus->changeState(false);

	//Basic Unit
	Grid3x3* basic_u = new Grid3x3(*coords);

	grids.push_back(basic_u);
	currentGrid = basic_u;

	butt_it = basic_u->setOrder(ptr->o_Move, idle, clicked, 0, 0, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 252, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[0]);

	butt_it->son = image_it;

	basic_u->buttons[0]->AddListener((j1Module*)App->orders);

	//------------
	butt_it = basic_u->setOrder(ptr->o_Stop, idle, clicked, 0, 1, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 288, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[1]);

	butt_it->son = image_it;

	basic_u->buttons[1]->AddListener((j1Module*)App->orders);

	//------------
	butt_it = basic_u->setOrder(ptr->o_Attack, idle, clicked, 0, 2, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 324, 442, 32, 32 });
	image_it->SetParent(basic_u->buttons[2]);

	butt_it->son = image_it;

	basic_u->buttons[2]->AddListener((j1Module*)App->orders);

	//------------
	basic_u->setOrder(ptr->o_Patrol, idle, clicked, 1, 0, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 576, 475, 32, 32 });
	image_it->SetParent(basic_u->buttons[3]);

	butt_it->son = image_it;

	basic_u->buttons[3]->AddListener((j1Module*)App->orders);

	//------------
	basic_u->setOrder(ptr->o_Hold_pos, idle, clicked, 1, 1, *atlasT, true);

	image_it = gui->CreateUI_Image({ 3, 3, 0, 0 }, iconsT, { 0, 510, 32, 32 });
	image_it->SetParent(basic_u->buttons[4]);

	butt_it->son = image_it;

	basic_u->buttons[4]->AddListener((j1Module*)App->orders);
}

bool S_SceneGUI::Update(float dt)
{
	std::list<Grid3x3*>::iterator it = grids.begin();
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		changeCurrentGrid(it._Ptr->_Myval);
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		it++;
		changeCurrentGrid(it._Ptr->_Myval);
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
	App->tex->UnLoad(iconsT);
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