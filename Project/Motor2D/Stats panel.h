#ifndef _STATS_PANEL_H_
#define _STATS_PANEL_H_

#include "SDL/include/SDL.h"
#include <map>
#include "Defs.h"
#define MAX_UNITS_M 12

enum Unit_Type;
enum Building_Type;

class UI_Image;
class UI_Label;
class Unit;

enum UPGRADES
{
	GROUND_ARMOR,
	GROUND_WEAPONS,
	GROUND_WEAPONS_2,
	PLASMA_SHIELDS
};

struct UnitWireframe
{
	~UnitWireframe();
	UI_Image* wireframe;
	const Unit* unit;
};

// Single
class Stats_Panel_Single
{
public:
	Stats_Panel_Single(){};
	~Stats_Panel_Single(){};

	//setState();

	void setStatsPanelSingle(Unit_Type, bool building = false);
	//void setStatsPanel(Building_Type);
public:
	//std::list<SDL_Rect> wireframe_rects;
	std::map<UPGRADES,SDL_Rect> upgradeIcons_rects;

	//3 is the maxium amount of upgradable tech an entity has
	UI_Label* unitName;
	UI_Label* upgrades_level[3];
	UI_Label* values[3];
	//killAmount

	UI_Image* upgrades_frames[3];
	UI_Image* upgrades_icons[3];
	UI_Image* wireframe;
};

class Stats_Panel_Mult
{
private:

public:
	
	Stats_Panel_Mult(){};
	~Stats_Panel_Mult();
	
	//Debug TO DELETE
//void setStatsWireframesMult(unsigned int index, Unit*);

	//Use at entity manager
	//void setStatsWireframesMult(const Unit*);
	void SelectUnit(const Unit*);
	void UnselectUnit(const Unit*);
	void unSelectUnitDebug(uint);

	void setSelectNone();
	unsigned int getI_Sel();
public:
	std::map<Unit_Type, SDL_Rect> unitWireframe_rects;

	UI_Image* unitSelect_frames[MAX_UNITS_M];
	UnitWireframe unitSelect_wires[MAX_UNITS_M];
private:
	unsigned int i_sel = 0;
};

#endif // !_STATS_PANEL_H_