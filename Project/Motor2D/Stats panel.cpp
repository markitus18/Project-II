#include "Stats panel.h"
#include "Unit.h"
#include "M_GUI.h"

void Stats_Panel_Single::setStatsPanelSingle(Unit_Type _type, bool building)
{
	switch (_type)
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

void Stats_Panel_Mult::setStatsWireframesMult(uint index, Unit_Type _type)
{
	if (index == 0)
		unitSelect_wires[index]->localPosition.y = 50;
	unitSelect_wires[index]->parent;
	unitSelect_wires[index]->SetRect(unitWireframe_rects[_type]);
}