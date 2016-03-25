#ifndef _ORDERS_H_
#define _ORDERS_H_

#define GRID_TOTAL 9

#include "j1Module.h"

#include "C_DynArray.h"

#include "C_Point.h"

#include "Orders Factory.h"

#include "SDL\include\SDL.h"




class M_Orders: public j1Module
{
public:

	M_Orders(bool enable_start) :j1Module(enable_start)
	{
		name.create("orders");
	}
	// Called when before render is available
	bool Awake(pugi::xml_node&);

	void OnGUI(GUI_EVENTS event, UI_Element* element);

	/*
	Add an order to the list and assign it a button
	Recomendended to use with grid3x3 setOrder!!!!!
	*/
	void addOrder(Order&, UI_Button2* = NULL);

	Gen_probe o_GenProbe_toss;
	Set_rallyPoint o_Set_rallyPoint;
	Move o_Move;
	Attack o_Attack;
	Gather o_Gather;
	Patrol o_Patrol;
	Hold_pos o_Hold_pos;
	Stop o_Stop;

private:
	C_DynArray<Order*> orders;
};

#pragma region Grid3x3

struct Grid_Coords
{
	//Invisible frame, parent of all the buttons
	UI_Rect*  frame;
public:

	Grid_Coords();
	//~Grid_Coords();

	// Position of the first button
	C_Point<int> pos1;

	//Margin between buttons
	C_Point<unsigned int> margin;

	//Width and height of the buttons
	C_Point <unsigned int> measures;

	//Measures + margin sum
	C_Point <unsigned int> button_distance;
};
class Grid3x3
{
public:

	Grid3x3(Grid_Coords&);
	~Grid3x3();
	
	/*
	Declare an order and assign it a position into de 3x3 Grid using a texture path
	(Columns and rows go from 0 to 2)	
	-Idle: The rect that will normally be displayed
	-Click: The one that will appear when using the button
	-Index_col: Column of the button.
	-Index_row: Row of the button.
	-Icon: The image that will be parented to the button
	-Path: The path of the file where the button is if NULL will use atlas
	-ToRedender: Will set the bool active to the button and it's sons 
	- Width & Heigh: In case you want to resize the button
	RETURN: Returns a pointer to the created button so it can be edited NULL on error
	Order's button WILL be changed
	*/
	UI_Button2* setOrder(Order& toAssign, const SDL_Rect & idle, const SDL_Rect & clicked, unsigned int row_index, unsigned int col_index, char* path = NULL, bool _toRender = true, unsigned int width = 0, unsigned int height = 0, SDL_Rect collider = { 0, 0, 0, 0 });
	/*
	Declare an order and assign it a position into de 3x3 Grid using
	(Columns and rows go from 0 to 2)
	-Idle: The rect that will normally be displayed
	-Click: The one that will appear when using the button
	-Index_col: Column of the button.
	-Index_row: Row of the button.
	-Tex: The texture that will use the button so we just load it once
	- Width & Heigh: In case you want to resize the button
	RETURN: Returns a pointer to the created button so it can be edited NULL on error
	Order's button WILL be changed
	*/
	UI_Button2* setOrder(Order& toAssign, const SDL_Rect & idle, const SDL_Rect & clicked, unsigned int row_index, unsigned int col_index, SDL_Texture& tex, bool _toRender = true, unsigned int width = 0, unsigned int height = 0, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Change the location of a pregenerated button into a grid, not good for testing
	(Columns and rows go from 0 to 2)
	NOT TESTED
	It seems that it does not work well
	*/
	void setOrder(Order& toAssign, unsigned int row_index, unsigned int col_index, UI_Button2 & button);
	//Calls setActive on all the buttons that are not null
	void changeState(bool change);
public:
	UI_Button2* buttons[GRID_TOTAL];
	
private:
	int i_total = -1;
	Grid_Coords* coords;

};

#pragma endregion
#endif // !_ORDERS_H_
