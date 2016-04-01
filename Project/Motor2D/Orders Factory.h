#ifndef _ORDERS_FACTORY_H_
#define _ORDERS_FACTORY_H_

#define GRID_TOTAL 9

#include "C_Point.h"
#include "SDL\include\SDL.h"

class UI_Button2;
class UI_Rect;
class UI_Image;

#pragma region Orders
class Order
{
public:
	Order(){ button = nullptr; }

	virtual void Function(){}

	//Setters & Getters
	void SetButton(UI_Button2& nButt){ button = &nButt; }

	const UI_Button2* getButton() const { return button; }

private:
	UI_Button2* button;


};

struct Gen_probe : public Order
{
	Gen_probe() :Order(){}
	void Function();
	
};

struct Set_rallyPoint : public Order
{
	Set_rallyPoint() :Order(){}
	void Function();
};

struct Move : public Order
{
	Move() :Order(){}
	void Function();
};

struct Attack : public Order
{
	Attack() :Order(){}
	void Function();
};

struct Gather : public Order
{
	Gather() :Order(){}
	void Function();
};

struct Patrol : public Order
{
	Patrol() :Order(){}
	void Function();
};

struct Hold_pos : public Order
{
	Hold_pos() :Order(){}
	void Function();
};

struct Stop : public Order
{
	Stop() :Order(){}
	void Function();
};
#pragma endregion

#pragma region Grid3x3

struct Grid_Coords
{
	//Invisible frame, parent of all the buttons
	UI_Rect*  frame;
public:

	Grid_Coords();
	~Grid_Coords();

	// Position of the first button
	C_Point<int> pos1;

	//Margin between buttons
	C_Point<uint> margin;

	//Width and height of the buttons
	C_Point <uint> measures;

	//Measures + margin sum
	C_Point <uint> button_distance;

	void cleanUp();
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
	UI_Button2* setOrder(Order& toAssign, const SDL_Rect & idle, const SDL_Rect & clicked, uint row_index, uint col_index, char* path = NULL, bool _toRender = true, UI_Image* = NULL, uint width = 0, uint height = 0, SDL_Rect collider = { 0, 0, 0, 0 });
	/*
	Declare an order and assign it a position into de 3x3 Grid using a texture
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
	UI_Button2* setOrder(Order& toAssign, const SDL_Rect & idle, const SDL_Rect & clicked, uint row_index, uint col_index, SDL_Texture& tex, bool _toRender = true, UI_Image* = NULL, uint width = 0, uint height = 0, SDL_Rect collider = { 0, 0, 0, 0 });

	/*
	Change the location of a pregenerated button into a grid, not good for testing
	(Columns and rows go from 0 to 2)
	NOT TESTED
	It seems that it does not work well
	*/
	void setOrder(Order& toAssign, uint row_index, uint col_index, UI_Button2 & button);

	//Calls setActive on all the buttons that are not null
	void changeState(bool change);

	void cleanUp();
public:
	UI_Button2* buttons[GRID_TOTAL];

private:
	int i_total = -1;
	Grid_Coords* coords;

};

#pragma endregion
#endif // !_ORDERS_FACTORY_H_
