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
	C_List_item<UI_Element*>* item = UI_Elements.start;
	//Item update
	for (int n = 0; n <= N_GUI_LAYERS; n++)
	{
		item = UI_Elements.start;
		while (item)
		{
			if (item->data->layer == n)
			{
				item->data->Update(dt);
			}
			item = item->next;
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
		item = UI_Elements.end;
		//Input for elements above the focus
		while (item && focus == focused)
		{
			item->data->InputManager();
			item = item->prev;
			if (item && focus == item->data)
			{
				focus->InputManager();
				item = item->prev;
				break;
			}
		}
		//If the mouse isn't inside the focused object, check the ones below
		while (item && focus)
		{
			if (focus->GetLastEvent() == UI_MOUSE_EXIT)
			{
				item->data->InputManager();
			}
			else
			{
				item->data->ForceLastEvent(UI_MOUSE_EXIT);
			}
			item = item->prev;
		}
	}
	//If there's no focus, check all elements until finding one
	else
	{
		item = UI_Elements.end;
		while (item && focus == NULL)
		{
			item->data->InputManager();
			item = item->prev;
		}
	}

	return true;
}

// Called before quitting
bool M_GUI::CleanUp()
{
	LOG("Freeing GUI");
	C_List_item<UI_Element*>* item = UI_Elements.start;
	while (item)
	{
		C_List_item<UI_Element*>* nextItem = item->next;
		RELEASE(item->data);
		item = nextItem;
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
	UI_InputText* inputText = (UI_InputText*)focus;
	inputText->GetNewInput(text);
}
