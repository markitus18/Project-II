#include "Orders Factory.h"

#include "Log.h"
#include "j1App.h"
#include "M_EntityManager.h"
#include "Building.h"
#include "M_GUI.h"
#include "Unit.h"
#include "M_InputManager.h"

#pragma region Orders

bool Order::CreateUnit(Unit_Type type)
{
	bool ret = button->enabled;
	if (ret)
		App->entityManager->StartUnitCreation(type);
	return ret;
}

bool Order::CreateBuilding(Building_Type type)
{
	bool ret = button->enabled;
	if (ret)
		App->entityManager->StartBuildingCreation(type);
	return ret;
}

void Set_RallyPoint::Function()
{
	App->entityManager->setWaypoint = true;
	if (App->entityManager->selectedBuilding)
		App->entityManager->selectedBuilding->hasWaypoint = false;
}

void Move::Function()
{
	App->entityManager->moveUnits = true;
}

void Stop::Function()
{
	App->entityManager->StopSelectedUnits();
}

void Attack::Function()
{
	App->entityManager->attackUnits = true;
}

void Hold_Pos::Function()
{
	//Delete & reasign
	//App->entityManager->selectedUnits
	//App->entityManager->StartBuildingCreation(PYLON);
}

void Gather::Function()
{
	//App->entityManager->SendToGather
}

void Patrol::Function()
{
}

void Build_Nexus::Function()
{
	CreateBuilding(NEXUS);
}

void Build_Gateaway::Function()
{
	CreateBuilding(GATEWAY);
}

void Build_Pylon::Function()
{
	CreateBuilding(PYLON);
}

void Build_Assimilator::Function()
{
	CreateBuilding(ASSIMILATOR);
}

void Ret_Cargo::Function()
{
}

void Basic_Builds::Function()
{
	App->gui->SetCurrentGrid(G_BASIC_BUILDINGS);
}

void Return_Builds_Menu::Function()
{
	App->gui->SetCurrentGrid(G_PROBE);
}

void Cancel_Current_Action::Function()
{
}


void Build_Templar_Archives::Function()
{
	CreateBuilding(TEMPLAR_ARCHIVES);
}


void Build_Citadel_Adun::Function()
{
	CreateBuilding(CITADEL);
}

void Build_Robotics_Facility::Function()
{
	CreateBuilding(ROBOTICS_FACILITY);
}

void Build_Robotics_Support_Bay::Function()
{
	CreateBuilding(ROBOTICS_BAY);
}
void Build_Stargate::Function()
{
	CreateBuilding(STARGATE);
}

void Build_Photon_Cannon::Function()
{
	CreateBuilding(PHOTON_CANNON);
}


void Build_Cybernetics::Function()
{
	CreateBuilding(CYBERNETICS_CORE);
}


void Advanced_Builds::Function()
{
	App->gui->SetCurrentGrid(G_ADVANCED_BUILDINGS);
}

void Gen_Zealot::Function()
{
	CreateUnit(ZEALOT);
}

void Gen_Dragoon::Function()
{
	CreateUnit(DRAGOON);
}

void Gen_Probe::Function()
{
	CreateUnit(PROBE);
}

void Gen_Scout::Function()
{
	CreateUnit(SCOUT);
}

void Gen_Reaver::Function()
{
	CreateUnit(REAVER);
}

void Gen_Observer::Function()
{
	CreateUnit(OBSERVER);
}

void Gen_High_Templar::Function()
{
	CreateUnit(HIGH_TEMPLAR);
}

void Gen_Dark_Templar::Function()
{
	CreateUnit(DARK_TEMPLAR);
}

void Gen_Shuttle::Function()
{
	CreateUnit(SHUTTLE);
}

void Gen_Carrier::Function()
{
	CreateUnit(CARRIER);
}
#pragma endregion

#pragma region Grid3x3

Grid_Coords::Grid_Coords()
{
	//Frame definition!
	//Frame: 498,356
	frame = App->gui->CreateUI_Rect({ 498, 356, 137, 121 }, 255, 240, 240, 254);
	//frame->localPosition.x += (frame->localPosition.w - 135) / 6;

	float posX = 8;
	float posY = 1;
	pos1 = { posX, posY };
	button_distance.x = 46;
	button_distance.y = 40;
}

Grid_Coords::~Grid_Coords()
{
	App->gui->DeleteUIElement(frame);
}


Grid3x3::Grid3x3(Grid_Coords& _origin, Grid_Type _type)
{
	coords = &_origin;
	type = _type;
}


/*
UI_Button2* Grid3x3::setOrder(Order& toAssign, const SDL_Rect & idle, const SDL_Rect & clicked, unsigned int row_index, unsigned int col_index, char* path, bool _toRender, UI_Image* img, unsigned int width, unsigned int height, SDL_Rect collider)
{
	UI_Button2* newButton = NULL;
	// Do not change these indexs!
	if (row_index > 2 || col_index > 2 || i_total >= GRID_TOTAL - 1)
	{
		LOG("Error at selecting the indexs");
	}
	else
	{
		++i_total;
		unsigned int result = col_index + row_index;

		newButton->son = img;
		unsigned int pX = coords->pos1.x + (coords->button_distance.x *col_index);
		unsigned int pY = coords->pos1.y + (coords->button_distance.y *row_index);
		newButton = App->gui->CreateUI_Button2({ pX, pY, width, height }, path, idle, clicked, _toRender, collider);
		newButton->order = &toAssign;
		buttons[i_total] = newButton;
		toAssign.SetButton(*newButton);
	}
	return newButton;

	/*
	int x = pos1.x;
	int y = pos1.y;


	for (unsigned int i2 = 0, y = pos1.y; i2 < 3; i2++) // Y
	{
	for (unsigned int i3 = 0, x = pos1.x; i3 < 3; i3++) // X
	{
	unsigned int i = (i2 + i3);
	/*buttons[i] = App->gui->CreateUIButton({ x, y, 0, 0 }, "graphics/cmdicons.png", { 468, 102, 32, 32 }, { 468, 102, 32, 32 }, { 468, 102, 32, 32 });
	buttons[i]->SetParent(frame);
	buttons[i]->AddListener((j1Module*)App->orders);
	buttons[i]->order = &App->orders->o_genZergling; *
	x += (measures.x + margin.x);
	}
	y += (measures.y + margin.y);
	}
	
}
*/
UI_Button2* Grid3x3::setOrder(Order& toAssign, const SDL_Rect & idle, const SDL_Rect & clicked, uint row_index, uint col_index, SDL_Texture& tex, float _width, float _height, SDL_Rect collider)
{
	UI_Button2* newButton = NULL;
	if (row_index > 2 || col_index > 2 || i_total >7)
	{
		LOG("Error at selecting the indexs");
	}
	else
	{
		++i_total;
		unsigned int result = col_index + row_index;

		unsigned int pX = coords->pos1.x + (coords->button_distance.x *col_index);
		unsigned int pY = coords->pos1.y + (coords->button_distance.y *row_index);

		newButton = App->gui->CreateUI_Button2({ pX, pY, 0, 0 }, &tex, idle, clicked, true, collider);
//		newButton->son = img;
		//newButton->localPosition = { pX, pY, _width, _height };
		buttons[i_total] = newButton;
		newButton->order = &toAssign;
		toAssign.SetButton(*newButton);
		
		//We need to add a listener so it executes the onEvent function
		//So we add the GUI listener that does nothing

		newButton->AddListener(App->gui);
		newButton->SetParent(coords->frame);
	}
	return newButton;
}

void Grid3x3::setOrder(Order& toAssign, unsigned int row_index, unsigned int col_index, UI_Button2 & button)
{
	if (row_index > 2 || col_index > 2 || i_total > 7)
	{
		LOG("Error at selecting the indexs");
	}
	else
	{
		++i_total;
		button.SetParent(coords->frame);

		unsigned int result = col_index + row_index;

		unsigned int pX = coords->pos1.x + (coords->button_distance.x *col_index);
		unsigned int pY = coords->pos1.y + (coords->button_distance.y *row_index);

		button.localPosition.x = pX;
		button.localPosition.y = pY;

		toAssign.SetButton(button);
		buttons[i_total] = &button;
	}
}

void Grid3x3::changeState(bool change)
{
	for (uint i = 0; i < GRID_TOTAL; i++)
	{
		if (buttons[i] != NULL)
			buttons[i]->SetActive(change);
	}
	return;
}

void Grid3x3::copyButtons(const Grid3x3 & toCopy)
{
	i_total = toCopy.i_total;
	for (int i = 0; i < 9; i++)
	{
		buttons[i] = toCopy.buttons[i];
	}
	
}

void Grid3x3::setButtonOnDepencencies(Building_Type _type, bool state)
{
	for (int i = 0; i < GRID_TOTAL; i++)
	{
		if (buttons[i])
		{
			buttons[i]->changeStateOnBuilding(_type,state);
		}
	}
	
}
void Grid3x3::cleanUp()
{
	for (uint i = 0; i < GRID_TOTAL; i++)
	{
		//The loop will jump if buttons[i] is null
		if (buttons[i] != NULL)
		{
			//Remove the button texture from UI_Elements list
			App->gui->DeleteUIElement(buttons[i]);
		}
	}
}
Grid3x3::~Grid3x3()
{
	cleanUp();
}
#pragma endregion