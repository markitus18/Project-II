#ifndef _STATS_PANEL_H_
#define _STATS_PANEL_H_

#include <list>
enum Unit_Type;
enum Building_Type;

class UI_Image;
class UI_Label;
class SDL_Texture;
class SDL_Rect;

class Stats_Panel
{
public:
	Stats_Panel();
	~Stats_Panel();

	//void setStatsPanel(Unit_Type);
	//void setStatsPanel(Building_Type);

	std::list<SDL_Rect*> wireframesRect;
	std::list<SDL_Rect*> upgradesRect;

	//killAmount
	UI_Image* upgrades;
	UI_Image* wireframes;

	UI_Label* unitName;
	UI_Label* values[3];
	UI_Label* upsLvl[3];


};

Stats_Panel::Stats_Panel()
{
}

Stats_Panel::~Stats_Panel()
{
}
#endif // !_STATS_PANEL_H_