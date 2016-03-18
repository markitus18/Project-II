#include "M_Orders.h"

#include "M_GUI.h"
#include "j1App.h"

#pragma region Orders
void M_Orders::addOrder(Order& nOrder, UI_Button2* nButt)
{
	if (nButt != NULL)
		nOrder.SetButton(*nButt);
	orders.PushBack(&nOrder);
}

bool M_Orders::Awake(pugi::xml_node&)
{
	addOrder(o_genProbe_toss);
	addOrder(o_set_rallyPoint);
	/*
	panel.setOrder(o_genProbe_toss, SDL_Rect{ 468, 102, 32, 32 }, SDL_Rect{ 467, 102, 32, 32 }, SDL_Rect{ 466, 102, 32, 32 }, 0, 0, "graphics/cmdicons.png");
	panel.setOrder(o_attack, SDL_Rect{ 252, 442, 32, 32 }, SDL_Rect{ 252, 443, 32, 32 }, SDL_Rect{ 252, 441, 32, 32 }, 1, 0, "graphics/cmdicons.png");*/


	//UIButton2 test declaration
	/*
	SDL_Texture* backbutton = App->tex->Load("graphics/pcmdbtns.png");


	UIButton2* test = App->gui->CreateUIButton2(SDL_Rect{ 50, 50, 0, 0 }, backbutton,  SDL_Rect{ 1, 0, 33, 34 }, SDL_Rect{ 74, 1, 33, 34 }, true);

	UIImage* bad = App->gui->CreateUIImage(SDL_Rect{ 1, 1, 32, 32 }, "graphics/cmdicons.png", SDL_Rect{ 468, 102, 32, 32 }, SDL_Rect{ 0,0,0,0 });
	bad->SetParent(test);

	// Testing with path NULL
	
	UIButton2* test_path = App->gui->CreateUIButton2(SDL_Rect{ 90, 50, 0, 0 }, (char*) NULL,  SDL_Rect{ 1, 0, 33, 34 }, SDL_Rect{ 74, 1, 33, 34 }, true, SDL_Rect{ 0, 0, 0, 0 });

	// Testing with path NULL

	UIButton2* test_path2 = App->gui->CreateUIButton2(SDL_Rect{ 120, 50, 0, 0 }, (char*) "graphics/pcmdbtns.png", SDL_Rect{ 1, 0, 33, 34 }, SDL_Rect{ 74, 1, 33, 34 }, true, SDL_Rect{ 0, 0, 0, 0 });

	/*UIButton* test = App->gui->CreateUIButton({ 20, 50, 0, 0 }, { 0, 113, 229, 69 }, { 411, 169, 229, 69 }, { 642, 169, 229, 69 }, { 12, 10, 200, 47 });
	test->movable = false;
	
	test->AddListener(this);

	test->order = &o_genZergling;
	
	
	
	UIButton* test2;
	test2 = App->gui->CreateUIButton({ 0, 0, 0, 0 }, { 0, 113, 229, 69 }, { 411, 169, 229, 69 }, { 642, 169, 229, 69 }, { 12, 10, 200, 47 });

	test2->AddListener(this);

	test->order = &o_attack;*/
	
	
	//App->gui->panel.Initialize();
	return true;
}

/*
If mouse is pressed iterate all the orders untill element is equal to an orders' button
*/
void  M_Orders::OnGUI(GUI_EVENTS event, UI_Element* element)
{
	if (event == UI_MOUSE_DOWN)
	{
		for (unsigned int i = 0; i < orders.Count(); i++)
		{
			if (orders[i]->getButton() == element)
			{
				orders[i]->Function();
				break;
			}
		}		
	}
}
#pragma endregion

//Grid-------------
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

Grid3x3::Grid3x3(Grid_Coords& _origin)
{
	coords = &_origin;
}

UI_Button2* Grid3x3::setOrder(Order& toAssign, const SDL_Rect & idle, const SDL_Rect & clicked, unsigned int row_index, unsigned int col_index, char* path, bool _toRender, unsigned int width, unsigned int height, SDL_Rect collider)
{
	UI_Button2* generated = NULL;
	// Do not change these indexs!
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
		generated = App->gui->CreateUI_Button2({ pX, pY, width, height }, path, idle, clicked, _toRender, collider);
		buttons[i_total] = generated;
		toAssign.SetButton(*generated);
	}
	return generated;

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
	*/
}

UI_Button2* Grid3x3::setOrder(Order& toAssign, const SDL_Rect & idle, const SDL_Rect & clicked, unsigned int row_index, unsigned int col_index, SDL_Texture& tex, bool _toRender, unsigned int width, unsigned int height, SDL_Rect collider)
{
	UI_Button2* generated = NULL;
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
		
		generated = App->gui->CreateUI_Button2({ pX, pY, width, height }, &tex,  idle, clicked, _toRender, collider);
		buttons[i_total] = generated;
		toAssign.SetButton(*generated);

		generated->SetParent(coords->frame);
	}
	return generated;
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
		//	buttons[i_total] = button;
		
	}

}
Grid3x3::~Grid3x3()
{
	//Just in case despite most likely unnecessary
	for (unsigned int i = GRID_TOTAL; i < GRID_TOTAL; i++)
	{
		RELEASE(buttons[i]);
	}
}
#pragma endregion