#include "Orders Factory.h"

#include "Log.h"
#include "j1App.h"
#include "M_EntityManager.h"
#include "Building.h"
#include "M_GUI.h"
#include "Unit.h"

#pragma region Orders
void Set_RallyPoint::Function()
{
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
	App->entityManager->StartBuildingCreation(PYLON);
	LOG("Executing Hold_pos");
}

void Gather::Function()
{
	LOG("Executing Gather");
}


void Patrol::Function()
{
	App->entityManager->StartBuildingCreation(NEXUS);
	LOG("Executing Patrol");
}

void Build_Nexus::Function()
{
	LOG("Executing Build_Nexus");
}

void Build_Gateaway::Function()
{
	LOG("Executing Build_Gateaway");
}

void Build_Pylon::Function()
{
	LOG("Executing Build_Pylon");
}

void Build_Assimilator::Function()
{
	LOG("Executing Build_Assimilator");
}
void Gen_Zealot::Function()
{
	LOG("Executing Gen_Zealot");
}

void Gen_Dragoon::Function()
{
	LOG("Executing Gen_Dragoon");
}

void Gen_Probe::Function()
{
	App->entityManager->StartUnitCreation(PROBE);
	LOG("Generate Probe");
}

#pragma endregion

#pragma region Grid3x3

Grid_Coords::Grid_Coords()
{
	//Frame definition!
	frame = App->gui->CreateUI_Rect({ 496, 355, 135, 118 }, 255, 0, 0, 0);

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

	measures.x = 32;
	measures.y = 32;

	pos1 = { 10, 3 };

	margin = { 15, 8 };

	button_distance.x = measures.x + margin.x;
	button_distance.y = measures.y + margin.y;
}

Grid_Coords::~Grid_Coords()
{
	App->gui->DeleteUIElement(frame);
}

void Grid_Coords::cleanUp()
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
	if (row_index > 2 || col_index > 2 || i_total > 7)
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

void Grid3x3::cleanUp()
{
	for (uint i = 0; i < GRID_TOTAL; i++)
	{
		//The loop will jump if buttons[i] is null
		if (buttons[i] != NULL)
		{
			//Remove the Images used
			App->gui->DeleteUIElement(buttons[i]->son);
			//Remove the button texture from UI_Elements list
			App->gui->DeleteUIElement(buttons[i]);
		}
	}
}
Grid3x3::~Grid3x3()
{
	//Just in case despite most likely unnecessary
	cleanUp();
}
#pragma endregion