#ifndef _STATS_PANEL_H_
#define _STATS_PANEL_H_

class UI_Image;
class UI_Label;
class Stats_Panel
{
public:
	Stats_Panel();
	~Stats_Panel();

	UI_Image* wireframe;
	UI_Image* upgrades[3];

	UI_Label* unitName;
	UI_Label* stats[3];
	UI_Label* upgrades_Lvl[3];
};

Stats_Panel::Stats_Panel()
{
}

Stats_Panel::~Stats_Panel()
{
}
#endif // !_STATS_PANEL_H_