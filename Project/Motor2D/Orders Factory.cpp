#include "Orders Factory.h"

#include "Log.h"
#include "j1App.h"
#include "M_EntityManager.h"
#include "Building.h"
#include "M_GUI.h"
#include "Unit.h"
#include "M_InputManager.h"

#pragma region Orders
void Set_RallyPoint::Function()
{
	App->entityManager->setWaypoint = true;
	if (App->entityManager->selectedBuilding)
		App->entityManager->selectedBuilding->hasWaypoint = false;
	LOG("Setting Rally Point!");
}

void Move::Function()
{
	App->entityManager->moveUnits = true;
	LOG("Executing Move");
}

void Stop::Function()
{
	App->entityManager->StopSelectedUnits();
	LOG("Executing Stop");
}

void Attack::Function()
{
	App->entityManager->attackUnits = true;
	LOG("Executing Attack");
}

void Hold_Pos::Function()
{
	//Delete & reasign
	//App->entityManager->selectedUnits
	//App->entityManager->StartBuildingCreation(PYLON);
	LOG("Executing Hold_pos");
}

void Gather::Function()
{
	//App->entityManager->SendToGather
	LOG("Executing Gather");
}

void Patrol::Function()
{
	LOG("Executing Patrol");
}

void Build_Nexus::Function()
{
	App->entityManager->StartBuildingCreation(NEXUS);
	LOG("Executing Build_Nexus");
}

void Build_Gateaway::Function()
{
	App->entityManager->StartBuildingCreation(GATEWAY);
	LOG("Executing Build_Gateaway");
}

void Build_Pylon::Function()
{
	App->entityManager->StartBuildingCreation(PYLON);
	LOG("Executing Build_Pylon");
}

void Build_Assimilator::Function()
{
	App->entityManager->StartBuildingCreation(ASSIMILATOR);
	LOG("Executing Build_Assimilator");
}
void Gen_Zealot::Function()
{
	App->entityManager->StartUnitCreation(ZEALOT);
	LOG("Executing Gen_Zealot");
}

void Gen_Dragoon::Function()
{
	App->entityManager->StartUnitCreation(DRAGOON);
	LOG("Executing Gen_Dragoon");
}

void Gen_Probe::Function()
{
	App->entityManager->StartUnitCreation(PROBE);
	LOG("Generate Probe");
}

void Ret_Cargo::Function()
{
	LOG("Executing Ret_Cargo");
}

void Basic_Builds::Function()
{
	App->gui->SetCurrentGrid(G_BASIC_BUILDINGS);
	LOG("Executing Basic_Builds");
}

void Return_Builds_Menu::Function()
{
	App->gui->SetCurrentGrid(G_PROBE);
	LOG("Executing Return_Builds_Menu");
}

void Cancel_Current_Action::Function()
{
	LOG("Executing Cancel_Current_Action");
}


void Build_Templar_Archives::Function()
{
	App->entityManager->StartBuildingCreation(TEMPLAR_ARCHIVES);
	LOG("Executing Build_Templar_Archives");
}


void Build_Citadel_Adun::Function()
{
	App->entityManager->StartBuildingCreation(CITADEL);
	LOG("Executing Build_Citadel_Adun");
}

void Build_Robotics_Facility::Function()
{
	App->entityManager->StartBuildingCreation(ROBOTICS_FACILITY);
	LOG("Executing Build_Robotics_Facility");
}


void Build_Stargate::Function()
{
	App->entityManager->StartBuildingCreation(STARGATE);
	LOG("Executing Build_Stargate");
}

void Build_Photon_Cannon::Function()
{
	App->entityManager->StartBuildingCreation(PHOTON_CANNON);
	LOG("Executing Build_Photon");
}


void Build_Cybernetics::Function()
{
	App->entityManager->StartBuildingCreation(CYBERNETICS_CORE);
	LOG("Executing Build_Cybernetics");
}


void Advanced_Builds::Function()
{
	App->gui->SetCurrentGrid(G_ADVANCED_BUILDINGS);
	LOG("Executing Advanced_Builds");
}


void Gen_Observer::Function()
{
	App->entityManager->StartUnitCreation(OBSERVER);
	LOG("Executing Gen_Observer");
}


void Gen_Reaver::Function()
{
	App->entityManager->StartUnitCreation(REAVER);
	LOG("Executing Gen_Reaver");
}


void Gen_Shuttle::Function()
{
	App->entityManager->StartUnitCreation(SHUTTLE);
	LOG("Executing Gen_Shuttle");
}


void Gen_High_Templar::Function()
{
	App->entityManager->StartUnitCreation(HIGH_TEMPLAR);
	LOG("Executing Gen_High_Templar");
}


void Gen_Carrier::Function()
{
	App->entityManager->StartUnitCreation(CARRIER);
	LOG("Executing Gen_Carrier");
}


void Gen_Scout::Function()
{
	App->entityManager->StartUnitCreation(SCOUT);
	LOG("Executing Gen_Scout");
}

#pragma endregion

#pragma region Grid3x3

Grid_Coords::Grid_Coords()
{
	int w, h, scale;
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;
	scale = App->events->GetScale();
	//Frame definition!
	frame = App->gui->CreateUI_Rect({ w * 0.3875f, (h - 250) / scale, w * 0.10546875f, 118 }, 255, 0, 0, 0);
	frame->localPosition.x += (frame->localPosition.w - 135) / 6;

	//Forma de guardar les coordenades dels 9 rects
	//{506,358} {552,358} {598,358}
	//{506,398} {552,398} {598,398}
	//{506,438} {552,438} {598,438}

	//Button W 32
	//Buttons H 32
	//X entre butons 4 Tileset
	//Y entre butons 3 Tileset

	//X entre butons 15 UI
	//Y entre butons 8 UI

	//1st Button pos {10,3}

	measures.x = w * 0.025f;
	measures.y = 32;

	int posX = w * 0.0078125f;
	uint marginX = w * 0.0109375f;
	pos1 = { posX, 2 };

	margin = { marginX, 8 };

	button_distance.x = measures.x + margin.x;
	button_distance.y = measures.y + margin.y;
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
UI_Button2* Grid3x3::setOrder(Order& toAssign, const SDL_Rect & idle, const SDL_Rect & clicked, uint row_index, uint col_index, SDL_Texture& tex, bool _toRender, UI_Image* img, uint width, uint height, SDL_Rect collider)
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

		newButton = App->gui->CreateUI_Button2({ pX, pY, width, height }, &tex, idle, clicked, _toRender, collider);
		newButton->son = img;
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
			if (buttons[i]->required_build == _type)
				buttons[i]->setEnabled(state);
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