#include "UI_Panel_Queue.h"
#include "UI_Element.h"
#include "Building.h"
//#include "C_BuildingQueue.h"
//Less than optmial solution to delete the UI elemetns from the list
#include "j1App.h"
#include "M_GUI.h"
void UI_Panel_Queue::disableQueue()
{
	background->SetActive(false);
	for (int i = 0; i < QUEUE_SLOTS - 1; i++)
	{
		icons[i]->SetActive(false);
	}
}

void UI_Panel_Queue::removeSlot(uint index)
{
	if (current_slots != -1)
	{
		for (int i = index; i < current_slots; i++)
			icons[i]->SetRect(icons[i + 1]->getRect());

		icons[current_slots]->SetActive(false);
		current_slots--;
	}
	if (current_slots == -1)
		background->SetActive(false);
}

void UI_Panel_Queue::addSlot(Unit_Type _type)
{
	if (current_slots < QUEUE_SLOTS - 1)
	{
		if (current_slots == -1)
			background->SetActive(true);

		current_slots++;

		SDL_Rect rect = icon_rects->operator[](_type);

		icons[current_slots]->SetRect(rect);

		icons[current_slots]->SetActive(true);
	}
}
void UI_Panel_Queue::loadBuilding(const Building& build)
{
	if (build.queue.units.empty() == false)
	{
		current_slots = build.queue.units.size();
		std::list<Unit_Type>::const_iterator it = build.queue.units.begin();
		
		for (int i = 0; it != build.queue.units.end() && i < QUEUE_SLOTS - 1; it++, i++)
		{		
			SDL_Rect rect = icon_rects->operator[]((*it));
			icons[i]->SetActive(true);
			icons[i]->SetRect(rect);
		}

		background->SetActive(true);
	}
}
UI_Panel_Queue::~UI_Panel_Queue()
{
	App->gui->DeleteUIElement(background);
	for (int i = 0; i < QUEUE_SLOTS; i++)
	{
		App->gui->DeleteUIElement(icons[i]);
	}
	
}