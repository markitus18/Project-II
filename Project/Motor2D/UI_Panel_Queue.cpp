#include "UI_Panel_Queue.h"
#include "UI_Element.h"
//Less than optmial solution to delete the UI elemetns from the list
#include "j1App.h"
#include "M_GUI.h"
void UI_Panel_Queue::setActiveQueue(bool state)
{
	background->SetActive(state);
	for (int i=0; i < QUEUE_SLOTS; i++)
	{
		icons[i]->SetActive(state);
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