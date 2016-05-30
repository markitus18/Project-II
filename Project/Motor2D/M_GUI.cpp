#include "M_GUI.h"
#include "M_InputManager.h"
#include "j1App.h"
#include "M_Textures.h"
#include "Orders Factory.h"
#include "S_SceneMap.h"
#include "Building.h"
#include "Unit.h"
#include "Stats Panel.h"
#include "UI_Panel_Queue.h"
// --------------- GUI MODULE --------------------------------------------------------


M_GUI::M_GUI(bool enabled) : j1Module(enabled)
{
	name.create("gui");
}

// Destructor
M_GUI::~M_GUI()
{}

// Called before render is available
bool M_GUI::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

//	atlas_file_name = conf.child("atlas").attribute("file").as_string();
	
//	atlas = App->tex->Load(atlas_file_name.GetString());

	return ret;
}

// Called before the first frame
bool M_GUI::Start()
{
	focus = NULL;
	return true;
}

// Update all guis
bool M_GUI::PreUpdate()
{
	if (App->events->hoveringUI && !App->sceneMap->onEvent)
	{
		App->events->hoveringUI = false;
		App->entityManager->UnfreezeInput();
	}

	App->events->clickedUI = false;
	//Input update, focus management
	if (focus != NULL)
	{
		if (focus->IsActive() == false)
		{
			focus = NULL;
		}
		UI_Element* focused = focus;

		std::list<UI_Element*>::reverse_iterator reverse_item;
		reverse_item = UI_Elements.rbegin();
		//Input for elements above the focus
		while (reverse_item != UI_Elements.rend() && focus == focused)
		{
			(*reverse_item)->InputManager();
			reverse_item++;
			if (reverse_item != UI_Elements.rend() && focus == (*reverse_item))
			{
				focus->InputManager();
				reverse_item++;
				break;
			}
		}
		//If the mouse isn't inside the focused object, check the ones below
		while (reverse_item != UI_Elements.rend() && focus)
		{
			if (focus->GetLastEvent() == UI_MOUSE_EXIT)
			{
				(*reverse_item)->InputManager();
			}
			else
			{
				(*reverse_item)->ForceLastEvent(UI_MOUSE_EXIT);
			}
			reverse_item++;
		}
	}
	//If there's no focus, check all elements until finding one
	else
	{
		std::list<UI_Element*>::reverse_iterator reverse_item = UI_Elements.rbegin();
		while (reverse_item != UI_Elements.rend() && focus == NULL)
		{
			(*reverse_item)->InputManager();
			reverse_item++;
		}
	}

	if (App->events->clickedUI)
	{
		App->events->EraseEvent(E_LEFT_CLICK);
	}
	return true;
}

// Called after all Updates
bool M_GUI::PostUpdate(float dt)
{
	std::list<UI_Element*>::iterator item;
	//Item update
	for (int n = 0; n <= N_GUI_LAYERS; n++)
	{
		item = UI_Elements.begin();
		while (item != UI_Elements.end())
		{
			if ((*item)->sprite.layer == n)
			{
				(*item)->Update(dt);
			}
			item++;
		}
	}

	return true;
}

// Called before quitting
bool M_GUI::CleanUp()
{
	LOG("Freeing GUI");
	std::list<UI_Element*>::iterator item = UI_Elements.begin();
	while (item != UI_Elements.end())
	{
		RELEASE((*item));
		item++;
	}
	UI_Elements.clear();
	return true;
}

void OnGui(GUI_EVENTS event, UI_Element* element)
{

}

// const getter for atlas
SDL_Texture* M_GUI::GetAtlas() const
{
	return atlas;
}

void M_GUI::SendNewInput(char* text)
{
	if (focus)
	{
		UI_InputText* inputText = (UI_InputText*)focus;
		inputText->GetNewInput(text);
	}
}

const Grid3x3* M_GUI::GetCurrentGrid()
{
	return currentGrid;
}

bool M_GUI::changeCurrentGrid(Grid3x3 * newCurrent)
{
	if (currentGrid == newCurrent)
		return false;

	if (currentGrid != NULL)
	{
		currentGrid->changeState(false);
	}
	if (newCurrent != NULL)
	{
		newCurrent->changeState(true);
	}
	currentGrid = newCurrent;

	return true;
}

bool M_GUI::SetCurrentGrid(Grid_Type _type)
{
	bool ret = true;
	if (_type == G_NONE)
	{
		ret = changeCurrentGrid(NULL);
	}
	else
	{
		std::vector<Grid_Type>::iterator it = App->sceneMap->gridTypes.begin();
		bool find = false;
		uint count = 0;
		while (it != App->sceneMap->gridTypes.end() && !find)
		{
			if ((*it) == _type)
				find = true;
			else
			{
				++it;
				++count;
			}

		}
		if (find)
			ret = changeCurrentGrid(App->sceneMap->grids[count]);
	}
	return ret;
}

bool M_GUI::SetCurrentGrid(const Unit* unit, bool multiple)
{
	bool ret = true;
	Grid_Type use = G_BASIC_UNIT;

	if (multiple == false)
	{
		switch (unit->GetType())
		{
		case PROBE:
			  use = G_PROBE;
			break;
		}
	}
	ret = SetCurrentGrid(use);
	return ret;
}

bool M_GUI::SetCurrentGrid(const Building* build)
{
	bool ret = true;
	Grid_Type use = G_NONE;

	if (build->finished == true)
	{
		switch (build->GetType())
		{
		case NEXUS:
		{
					  use = G_NEXUS;
					  break;
		}
		case GATEWAY:
		{
						use = G_GATEWAY;
						break;
		}
		case STARGATE:
			use = G_STARGATE;
			break;
		}
	}
	ret = this->SetCurrentGrid(use);

	return ret;
}

void M_GUI::setButtonStateOnBuildingType(Building_Type _type, bool state)
{
	std::vector<Grid3x3*>::iterator it = App->sceneMap->grids.begin();
	while (it != App->sceneMap->grids.end())
	{
		(*it)->setButtonOnDepencencies(_type, state);
		it++;
	}
		
}

void M_GUI::setProductionQueue(Building* build)
{
	if (App->sceneMap->panel_queue)
	{
		if (build == NULL)
			App->sceneMap->panel_queue->disableQueue();
		else
			App->sceneMap->panel_queue->loadBuilding(build);
	}
}

void M_GUI::addQueueSlot(Unit_Type _type)
{
	App->sceneMap->panel_queue->addSlot(_type);
}

void M_GUI::removeQueueSlot(Building *_build)
{
	if (App->sceneMap->panel_queue != NULL)
	{
		if (_build && App->sceneMap->panel_queue && App->sceneMap->panel_queue->current_build)
		{
			if (_build == App->sceneMap->panel_queue->current_build)
				App->sceneMap->panel_queue->removeSlot();
		}
	}
}

void M_GUI::UI_Unselect()
{
	if (App->sceneMap->statsPanel_m)
	{
		App->sceneMap->statsPanel_m->setSelectNone();
	}
}

void M_GUI::UI_UnitSelect(const Unit* _unit)
{
	if (_unit->race == PROTOSS)
	App->sceneMap->statsPanel_m->SelectUnit(_unit);
	//App->sceneMap->statsPanel_m->setStatsWireframesMult(_unit);
}

void M_GUI::UI_UnitUnselect(const Unit* _unit)
{
	App->sceneMap->statsPanel_m->UnselectUnit(_unit);
}

void M_GUI::UI_UnitUnselect(uint i)
{
	App->sceneMap->statsPanel_m->UnselectUnit(i);
}
void M_GUI::AddBossBar()
{
	App->sceneMap->AddBossBar();
}
