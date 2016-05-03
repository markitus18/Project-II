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

//void Stats_Panel_Mult::setStatsWireframesMult(uint index, Unit* _type)
//{
//	unitSelect_wires[index].wireframe->SetRect(unitWireframe_rects[_type]);
//	unitSelect_wires[i_sel].type = _type;
//	i_sel++;
//}

UnitWireframe::~UnitWireframe()
{
	App->gui->DeleteUIElement(wireframe);
}

void Stats_Panel_Mult::setSelectNone()
{
	i_sel = 0;
	for (int i = 0; i < MAX_UNITS_M; i++)
	{
		unitSelect_frames[i]->SetActive(false);
		unitSelect_wires[i_sel].unit = NULL;
	}
}

void Stats_Panel_Mult::SelectUnit(const Unit* _unit)
{
	unitSelect_frames[i_sel]->SetActive(true);
	unitSelect_wires[i_sel].wireframe->SetRect(unitWireframe_rects[_unit->GetType()]);
	unitSelect_wires[i_sel].unit = _unit;
	i_sel++;
}

void Stats_Panel_Mult::UnselectUnit(const Unit* _unit)
{
	int i;
	for (i = 0; i < MAX_UNITS_M; i++)
	{
		if (unitSelect_wires[i].unit == _unit)
		{
			unitSelect_frames[i]->SetActive(false);
			unitSelect_wires[i].unit = NULL;
			break;
		}
	}
}
void Stats_Panel_Mult::unSelectUnitDebug(uint index)
{
	unitSelect_frames[index]->SetActive(false);
	unitSelect_wires[index].unit = NULL;
}

Stats_Panel_Mult::~Stats_Panel_Mult()
{
	for (int i = 0; i < MAX_UNITS_M; i++)
	{
		App->gui->DeleteUIElement(unitSelect_frames[i]);
	}
};

uint Stats_Panel_Mult::getI_Sel()
{
	return i_sel;
}