#include "UI_Panel_Queue.h"
#include "UI_Element.h"
#include "Building.h"
#include "C_BuildingQueue.h"
//Less than optmial solution to delete the UI elemetns from the list
#include "j1App.h"
#include "M_GUI.h"

void UI_Panel_Queue::disableQueue()
{
	background->SetActive(false);
	progress_background->SetActive(false);
	progress_bar->SetActive(false);

	for (int i = 0; i < QUEUE_SLOTS; i++)
	{
		if (icons[i])
		{
			icons[i]->SetActive(false);
		}
	}
		current_slots = -1;
}

void UI_Panel_Queue::removeSlot(uint index)
{
	if (current_slots != -1)
	{
		for (int i = index; i < current_slots; i++)
			icons[i]->SetRect(icons[i + 1]->getRect());

		icons[current_slots]->SetActive(false);
		current_slots--;
	
		if (current_slots == -1)
			background->SetActive(false);
	}
}

void UI_Panel_Queue::addSlot(Unit_Type _type)
{
	if (current_slots < QUEUE_SLOTS)
	{
		if (current_slots == -1)
			background->SetActive(true);

		current_slots++;

		if (current_slots == 0)
		{
		//	int current = (int)current_build->queue.timer.ReadSec();
		//	int max = 1;
		}

		SDL_Rect rect = icon_rects->operator[](_type);

		icons[current_slots]->SetRect(rect);

		icons[current_slots]->SetActive(true);
	}
}
void UI_Panel_Queue::addSlot(Unit* unit)
{
	if (current_slots < QUEUE_SLOTS)
	{
		if (current_slots == -1)
			background->SetActive(true);
		
		const UnitStatsData* stats = App->entityManager->GetUnitStats(unit->GetType());
	//	int max = (int)stats->buildTime;
		current_slots++;
	//	if (current_slots == 0)
	//		progress_bar->SetPointers(&max);
		SDL_Rect rect = icon_rects->operator[](unit->GetType());

		icons[current_slots]->SetRect(rect);

		icons[current_slots]->SetActive(true);
	}
}
void UI_Panel_Queue::loadBuilding(Building* build)
{
	current_build = build;

	const std::list<Unit_Type>& unitList = build->queue.units;
	if (unitList.empty() == false)
	{
		current_slots = unitList.size() - 1;
		std::list<Unit_Type>::const_iterator it = unitList.begin();

		for (int i = 0; it != unitList.end() && i < QUEUE_SLOTS; it++, i++)
		{
			SDL_Rect rect = icon_rects->operator[]((*it));
			icons[i]->SetActive(true);
			icons[i]->SetRect(rect);
		}
		background->SetActive(true);
	}
	else
		current_slots = -1;
}

void UI_Panel_Queue::UpdateQueue()
{
	if (current_slots != -1)
	{
		bar_current = current_build->GetQueuePercentage();
	}
}

UI_Panel_Queue::~UI_Panel_Queue()
{
	App->gui->DeleteUIElement(background);
	App->gui->DeleteUIElement(progress_bar);
	App->gui->DeleteUIElement(progress_background);
	for (int i = 0; i < QUEUE_SLOTS ; i++)
	{
		App->gui->DeleteUIElement(icons[i]);
	}
	
}