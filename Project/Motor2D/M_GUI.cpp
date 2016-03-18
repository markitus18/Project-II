#include "M_GUI.h"
#include "M_Input.h"
#include "j1App.h"
#include "M_Textures.h"

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

	atlas_file_name = conf.child("atlas").attribute("file").as_string();
	
	atlas = App->tex->Load(atlas_file_name.GetString());

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
			if ((*item)->layer == n)
			{
				(*item)->Update(dt);
			}
			item++;
		}
	}

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
