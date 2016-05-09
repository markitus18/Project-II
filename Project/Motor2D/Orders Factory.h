#ifndef _ORDERS_FACTORY_H_
#define _ORDERS_FACTORY_H_

#define GRID_TOTAL 9

#include "C_Point.h"
#include "SDL\include\SDL.h"

#include "M_GUI.h"
enum Unit_Type;
enum Grid_Type
{
	G_NONE,
	G_BASIC_UNIT,
	G_NEXUS,
	G_BASIC_BUILDINGS,
	G_ADVANCED_BUILDINGS,
	G_PROBE,
	G_GATEWAY,
	G_STARGATE
};

//enum Building_Dependecies
//{
//	BD_CYBERNETICS,
//	BD_STARGATE,
//	BD_FORGE,
//	BD_NONE
//};

class Order
{
public:
	Order(){ button = NULL;}
	~Order(){ button = NULL; };
	virtual void Function(){}
	bool CreateUnit(Unit_Type);
	bool CreateBuilding(Building_Type);
	//Setters & Getters
	void SetButton(UI_Button2& nButt){ button = &nButt;  button->SetLayer(1); }

	const UI_Button2* getButton() const { return button; }

private:
	UI_Button2* button;

};
#pragma region Orders

struct Set_RallyPoint : public Order
{
	Set_RallyPoint() :Order(){}
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

struct Hold_Pos : public Order
{
	Hold_Pos() :Order(){}
	void Function();
};

struct Stop : public Order
{
	Stop() :Order(){}
	void Function();
};

struct Ret_Cargo : public Order
{
	Ret_Cargo() :Order(){}
	void Function();
};

struct Build_Nexus : public Order
{
	Build_Nexus() :Order(){}
	void Function();
};

struct Build_Gateaway : public Order
{
	Build_Gateaway() :Order(){}
	void Function();
};

struct Build_Pylon : public Order
{
	Build_Pylon() :Order(){}
	void Function();
};

struct Build_Assimilator : public Order
{
	Build_Assimilator() :Order(){}
	void Function();
};

struct Gen_Zealot : public Order
{
	Gen_Zealot() :Order(){}
	void Function();
};

struct Gen_Dragoon : public Order
{
	Gen_Dragoon() :Order(){}
	void Function();
};

struct Gen_Probe : public Order
{
	Gen_Probe() :Order(){}
	void Function();
};

struct Basic_Builds : public Order
{
	Basic_Builds() :Order(){}
	void Function();
};

struct Cancel_Current_Action : public Order
{
	Cancel_Current_Action() :Order(){}
	void Function();
};

struct Return_Builds_Menu : public Order
{
	Return_Builds_Menu() :Order(){}
	void Function();
};

struct Advanced_Builds : public Order
{
	Advanced_Builds() :Order(){}
	void Function();
};

struct Build_Cybernetics : public Order
{
	Build_Cybernetics() :Order(){}
	void Function();
};

struct Build_Photon_Cannon : public Order
{
	Build_Photon_Cannon() :Order(){}
	void Function();
};

struct Build_Stargate : public Order
{
	Build_Stargate() :Order(){}
	void Function();
};

struct Build_Robotics_Facility : public Order
{
	Build_Robotics_Facility() :Order(){}
	void Function();
};

struct Build_Citadel_Adun : public Order
{
	Build_Citadel_Adun() :Order(){}
	void Function();
};

struct Build_Robotics_Support_Bay : public Order
{
	Build_Robotics_Support_Bay() :Order(){}
	void Function();
};

struct Build_Templar_Archives : public Order
{
	Build_Templar_Archives() :Order(){}
	void Function();
};

struct Gen_Scout : public Order
{
	Gen_Scout() :Order(){}
	void Function();
};

struct Gen_Carrier : public Order
{
	Gen_Carrier() :Order(){}
	void Function();
};

struct Gen_High_Templar : public Order
{
	Gen_High_Templar() :Order(){}
	void Function();
};

struct Gen_Dark_Templar : public Order
{
	Gen_Dark_Templar() :Order(){}
	void Function();
};

struct Gen_Shuttle : public Order
{
	Gen_Shuttle() :Order(){}
	void Function();
};

struct Gen_Reaver : public Order
{
	Gen_Reaver() :Order(){}
	void Function();
};

struct Gen_Observer : public Order
{
	Gen_Observer() :Order(){}
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
};

class Grid3x3
{
public:

	Grid3x3(Grid_Coords&, Grid_Type = G_NONE);
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

	void setButtonOnDepencencies(Building_Type _type, bool state);

	void copyButtons(const Grid3x3 &);

	void cleanUp();
public:
	UI_Button2* buttons[GRID_TOTAL];
	Grid_Type type;
	int i_total = -1;
private:
	
	Grid_Coords* coords;
};

#pragma endregion
#endif // !_ORDERS_FACTORY_H_
