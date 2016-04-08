#include "Stats panel.h"
#include "Unit.h"
#include "M_GUI.h"

void Stats_Panel_Single::setStatsPanelSingle(Unit_Type type, bool building)
{
	switch (type)
	{
	case ZEALOT:
		upgrades_icons[0]->SetRect(upgradeIcons_rects[GROUND_ARMOR]);
		upgrades_icons[1]->SetRect(upgradeIcons_rects[PLASMA_SHIELDS]);
		upgrades_icons[2]->SetRect(upgradeIcons_rects[GROUND_WEAPONS]);
		break;
	case DRAGOON:
		upgrades_icons[0]->SetRect(upgradeIcons_rects[GROUND_ARMOR]);
		upgrades_icons[1]->SetRect(upgradeIcons_rects[PLASMA_SHIELDS]);
		upgrades_icons[2]->SetRect(upgradeIcons_rects[GROUND_WEAPONS_2]);
		break;
	}
}