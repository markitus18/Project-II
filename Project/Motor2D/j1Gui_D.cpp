#include "j1Gui_D.h"
#include "j1Input.h"


// --------------- GUI MODULE --------------------------------------------------------


j1Gui_D::j1Gui_D(bool enabled) : j1Module(enabled)
{
	name.create("gui");
}

// Destructor
j1Gui_D::~j1Gui_D()
{}

// Called before render is available
bool j1Gui_D::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string();
	
	atlas = App->tex->Load(atlas_file_name.GetString());

	return ret;
}

// Called before the first frame
bool j1Gui_D::Start()
{
	focus = NULL;
	
	return true;
}

// Update all guis
bool j1Gui_D::PreUpdate()
{
	return true;
}

// Called after all Updates
bool j1Gui_D::PostUpdate(float dt)
{
	p2List_item<UI_D_Element*>* item = UI_D_Elements.start;
	//Item update
	for (int n = 0; n <= N_GUI_LAYERS; n++)
	{
		item = UI_D_Elements.start;
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
		UI_D_Element* focused = focus;
		item = UI_D_Elements.end;
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
		item = UI_D_Elements.end;
		while (item && focus == NULL)
		{
			item->data->InputManager();
			item = item->prev;
		}
	}

	return true;
}

// Called before quitting
bool j1Gui_D::CleanUp()
{
	LOG("Freeing GUI");
	p2List_item<UI_D_Element*>* item = UI_D_Elements.start;
	while (item)
	{
		p2List_item<UI_D_Element*>* nextItem = item->next;
		RELEASE(item->data);
		item = nextItem;
	}
	UI_D_Elements.clear();
	return true;
}

// const getter for atlas
SDL_Texture* j1Gui_D::GetAtlas() const
{
	return atlas;
}