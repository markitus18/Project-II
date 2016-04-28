#ifndef _STATS_PANEL_H_
#define _STATS_PANEL_H_

#include "SDL/include/SDL.h"
#include <map>
#define max_unit_m 12
enum Unit_Type;
enum Building_Type;

class UI_Image;
class UI_Label;


enum UPGRADES
{
	GROUND_ARMOR,
	GROUND_WEAPONS,
	GROUND_WEAPONS_2,
	PLASMA_SHIELDS
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
public:
	Stats_Panel_Mult(){};
	~Stats_Panel_Mult();
	
	void setStatsWireframesMult(unsigned int index, Unit_Type);
public:
	std::map<Unit_Type, SDL_Rect> unitWireframe_rects;

	UI_Image* unitSelect_frames[max_unit_m];
	UI_Image* unitSelect_wires[max_unit_m];
};

#endif // !_STATS_PANEL_H_