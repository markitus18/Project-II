#ifndef _STATS_PANEL_H_
#define _STATS_PANEL_H_

#include <list>
enum Unit_Type;
enum Building_Type;
class UI_Image;
class UI_Label;

class Stats_Panel
{
public:
	Stats_Panel();
	~Stats_Panel();

	void LoadInfo();

	//void setStatsPanel(Unit_Type);
	//void setStatsPanel(Building_Type);
private:
	std::list<UI_Image*> wireframes;
	std::list<UI_Image*> upgrades;

	//killAmount
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