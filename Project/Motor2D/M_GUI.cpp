#include "M_GUI.h"
#include "M_Input.h"
#include "j1App.h"
#include "M_Textures.h"
#include "Orders Factory.h"
#include "S_SceneMap.h"
#include "Building.h"
#include "Unit.h"
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
	debugHover = CreateUI_Label({ 50, 50, 0, 0 }, "Mouse hovering!");
	return true;
}

// Update all guis
bool M_GUI::PreUpdate()
{
	mouseClicked = false;
	//Input update, focus management
	if (focus != NULL)
	{
		if (focus->GetActive() == false)
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

	if (App->input->clickedGUI != debugHover->GetActive())
	{
		debugHover->SetActive(App->input->clickedGUI);
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

bool M_GUI::SetCurrentGrid( Grid3x3 * newCurrent)
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
		ret = SetCurrentGrid(NULL);
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
			ret = SetCurrentGrid(App->sceneMap->grids[count]);
	}
	return ret;
}

bool M_GUI::SetCurrentGrid(Unit_Type type, bool multiple)
{
	bool ret = true;
	Grid_Type use = G_BASIC_UNIT;

	if (multiple == false)
	{
		switch (type)
		{

		case PROBE:
			  use = G_PROBE;
			break;
		}
	}
	ret = SetCurrentGrid(use);
	return ret;
}
bool M_GUI::SetCurrentGrid(Building_Type _type)
{
	bool ret = true;
	Grid_Type use = G_NONE;
	switch (_type)
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
	}

	ret = SetCurrentGrid(use);

	return ret;
}