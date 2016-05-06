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
	selectedIndex = -1;
	for (int i = 0; i < MAX_UNITS_M; i++)
	{
		unitSelect_frames[i]->SetActive(false);
		unitSelect_wires[i].unit = NULL;
	}
}

void Stats_Panel_Mult::SelectUnit(const Unit* _unit)
{
	if (selectedIndex < MAX_UNITS_M - 1)
	{
		selectedIndex++;
		unitSelect_frames[selectedIndex]->SetActive(true);
		unitSelect_wires[selectedIndex].wireframe->SetRect(unitWireframe_rects[_unit->GetType()]);
		unitSelect_wires[selectedIndex].unit = _unit;
	}
}

void Stats_Panel_Mult::UnselectUnit(const Unit* _unit)
{
	int i;
	/*for (i = 0; i < MAX_UNITS_M; i++)
	{
		if (unitSelect_wires[i].unit == _unit)
		{
			UnselectUnit(i);
			break;
		}
	}*/
}
//
//if (current_slots != -1)
//{
//	for (int i = index; i < current_slots; i++)
//		icons[i]->SetRect(icons[i + 1]->getRect());
//
//	icons[current_slots]->SetActive(false);
//	current_slots--;
//}

void Stats_Panel_Mult::UnselectUnit(uint index)
{
	for (int i = index; i < selectedIndex; i++)
	{
		unitSelect_wires[i].wireframe->SetRect(unitSelect_wires[i + 1].wireframe->getRect());
		unitSelect_wires[i].unit = unitSelect_wires[i + 1].unit;
	}
		

	unitSelect_frames[selectedIndex]->SetActive(false);
	unitSelect_wires[selectedIndex].unit = NULL;
	selectedIndex--;
}

Stats_Panel_Mult::~Stats_Panel_Mult()
{
	for (int i = 0; i < MAX_UNITS_M; i++)
	{
		App->gui->DeleteUIElement(unitSelect_frames[i]);
	}
};

uint Stats_Panel_Mult::getIndex()
{
	return selectedIndex;
}