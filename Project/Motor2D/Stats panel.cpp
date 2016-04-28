#include "Stats panel.h"
#include "Unit.h"
#include "M_GUI.h"
#include "j1App.h"

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
	i_sel++;
	unitSelect_wires[index].wireframe->SetRect(unitWireframe_rects[_type]);
	unitSelect_wires[i_sel].type = _type;
}

void Stats_Panel_Mult::setStatsWireframesMult(Unit_Type _type)
{
	unitSelect_wires[i_sel].wireframe->SetRect(unitWireframe_rects[_type]);
	unitSelect_wires[i_sel].type = _type;
	i_sel++;
}

void Stats_Panel_Mult::setSelectNone()
{
	i_sel = 0;
	for (int i = 0; i < max_unit_m; i++)
	{
		unitSelect_wires[i].wireframe->SetActive(false);
		unitSelect_wires[i_sel].type = CARRIER;
	}
}

void Stats_Panel_Mult::unselectUnit(Unit_Type _Type)
{
	int i;
	for (i = 0; i < max_unit_m; i++)
	{
		if (unitSelect_wires[i].type == _Type)
		{

		}
	}
}

Stats_Panel_Mult::~Stats_Panel_Mult()
{
	for (int i = 0; i < max_unit_m; i++)
	{
		App->gui->DeleteUIElement(unitSelect_frames[i]);
		App->gui->DeleteUIElement(unitSelect_wires[i]);
	}
};

uint Stats_Panel_Mult::getI_Sel()
{
	return i_sel;
}