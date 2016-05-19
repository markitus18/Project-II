#include "M_InputManager.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_Input.h"
#include "M_Window.h"
#include "M_Map.h"


M_InputManager::M_InputManager(bool start_enabled) : j1Module(start_enabled)
{
	name.create("inputManager");
}

// Destructor
M_InputManager::~M_InputManager()
{
}

// Called when before render is available
bool M_InputManager::Awake(pugi::xml_node& config)
{
	//Loading keys and events from from xml
	for (pugi::xml_node path = config.child("key"); path; path = path.next_sibling("key"))
	{
		int scancode = SDL_GetScancodeFromName(path.attribute("id").as_string());
		if (scancode != SDL_SCANCODE_UNKNOWN)
		{
			std::pair<int, e_events> toPush;
			toPush.first = scancode;
			toPush.second = static_cast<e_events>(path.attribute("event").as_int());
			eventsList.insert(toPush);
		}
	}

	return true;
}

// Call before first frame
bool M_InputManager::Start()
{
	App->win->GetWindowSize(&screenSize.x, &screenSize.y);
	LOG("INPUT size %d %d", screenSize.x, screenSize.y);
	clickedUI = false;
	hoveringUI = false;

	return true;
}

// Called before all Updates
bool M_InputManager::PreUpdate()
{
	return true;
}

bool M_InputManager::Update(float dt)
{
	//TO CHANGE: Is this going to be something useful? We can remove render.h otherwise
	int x, y;
	App->input->GetMousePosition(x, y);
	mouseScreen = { x, y };
	mouseWorld = App->render->ScreenToWorld(x, y);

	return true;
}

// Called after all Updates
bool M_InputManager::PostUpdate(float dt)
{
	currentEvents.clear();

	return true;
}

// Called before quitting
bool M_InputManager::CleanUp()
{
	return true;
}


// Load Game State
bool M_InputManager::Load(pugi::xml_node& data)
{
	eventsList.clear();
	for (pugi::xml_node event = data.child("event"); event; event = event.next_sibling("event"))
	{
		int key = event.attribute("key").as_int();
		e_events e_event = static_cast<e_events>(event.attribute("event").as_int());
		std::pair<int, e_events> toPush;
		toPush.first = key;
		toPush.second = e_event;
		eventsList.insert(toPush);
	}

	return true;
}

// Save Game State
bool M_InputManager::Save(pugi::xml_node& data) const
{
	for (std::map<int, e_events>::const_iterator it = eventsList.cbegin(); it != eventsList.cend(); it++)
	{
		pugi::xml_node event = data.append_child("event");
		event.append_attribute("key") = it->first;
		event.append_attribute("event") = it->second;
	}

	return true;
}



void M_InputManager::SendEvent(int id, e_eventState state)
{
	if (savingNextKey == false)
	{
		std::map<int, e_events>::iterator tmp = eventsList.find(id);
		while (tmp != eventsList.end())
		{
			if (tmp->first == id)
			{
				std::pair<e_events, e_eventState> toPush;
				toPush.first = tmp->second;
				toPush.second = state;
				currentEvents.insert(toPush);
			}
			tmp++; 
		}
	}
	else
	{
		SetEventKey(savingEventToChange, static_cast<SDL_Scancode>(id));
		savingNextKey = false;
	}

}

void M_InputManager::SendMouseEvent(int button, e_eventState state)
{
	std::pair<e_events, e_eventState> toPush;
	switch (button)
	{
	case SDL_BUTTON_LEFT:
	{
		toPush.first = E_LEFT_CLICK; break;
	}
	case SDL_BUTTON_RIGHT:
	{
		toPush.first = E_RIGHT_CLICK; break;
	}
	case SDL_BUTTON_MIDDLE:
	{
		toPush.first = E_MID_CLICK; break;
	}
	default:
	{
		return;
	}
	}
	
	toPush.second = state;
	currentEvents.insert(toPush);
}

void M_InputManager::EraseEvent(e_events eventToErase)
{
	if (currentEvents.empty() == false)
	{
		std::map<e_events, e_eventState>::iterator tmp = currentEvents.find(eventToErase);
		if (tmp != currentEvents.end())
		{
			currentEvents.erase(tmp);
		}
	}
}

e_eventState M_InputManager::GetEvent(e_events _event)
{
	if (currentEvents.empty() == false)
	{
		std::map<e_events, e_eventState>::iterator tmp = currentEvents.find(_event);
		if (tmp != currentEvents.end())
		{
			return tmp->second;
		}
	}
	return EVENT_NONE;
}

const char* M_InputManager::GetEventKeyName(e_events _event)
{
	if (eventsList.empty() == false)
	{
		std::map<int, e_events>::iterator tmp = eventsList.begin();
		while (tmp != eventsList.end())
		{
			if (tmp->second == _event)
			{
				return SDL_GetScancodeName(static_cast<SDL_Scancode>(tmp->first));
			}
			tmp++;
		}
	}
	return "";
}

SDL_Scancode M_InputManager::GetEventKey(e_events _event)
{
	if (eventsList.empty() == false)
	{
		std::map<int, e_events>::iterator tmp = eventsList.begin();
		while (tmp != eventsList.end())
		{
			if (tmp->second == _event)
			{
				return static_cast<SDL_Scancode>(tmp->first);
			}
			tmp++;
		}
	}
	return SDL_SCANCODE_UNKNOWN;
}

void M_InputManager::SetEventToNextKeyPress(e_events _event)
{
	savingNextKey = true;
	savingEventToChange = _event;
}

bool M_InputManager::SetEventKey(e_events _event, SDL_Scancode key)
{
	if (eventsList.empty() == false)
	{
		std::map<int, e_events>::iterator tmp = eventsList.begin();
		while (tmp != eventsList.end())
		{
			if (tmp->second == _event)
			{
				std::pair<int, e_events> toPush;
				toPush.first = key;
				toPush.second = _event;

				eventsList.erase(tmp);

				eventsList.insert(toPush);

				std::pair<e_events, e_eventState> notification;
				notification.first = E_UPDATED_KEY;
				notification.second = EVENT_DOWN;
				currentEvents.insert(notification);

				LOG("Succesfully assigned %s key to event %i", SDL_GetScancodeName(key), _event);
				return true;
			}
			tmp++;
		}

		std::pair<int, e_events> toPush;
		toPush.first = key;
		toPush.second = _event;
		eventsList.insert(toPush);

		std::pair<e_events, e_eventState> notification;
		notification.first = E_UPDATED_KEY;
		notification.second = EVENT_DOWN;
		currentEvents.insert(notification);
		LOG("Created new event %s to %i", SDL_GetScancodeName(key), _event);
		return true;
	}
	return false;
}

void M_InputManager::UnfreezeInput()
{
	App->input->UnFreezeInput();
}

void M_InputManager::FreezeInput()
{
	App->input->FreezeInput();
}


bool M_InputManager::IsInputFrozen()
{
	return App->input->GetInputState();
}

iPoint M_InputManager::GetMouseMotion()
{
	iPoint  tmp;
	App->input->GetMouseMotion(tmp.x, tmp.y);
	return tmp;
}

iPoint M_InputManager::GetMapSize()
{
	return{ App->map->data.width * App->map->data.tile_width, App->map->data.height * App->map->data.tile_height };
}
iPoint M_InputManager::GetMapSizeScaled()
{
	iPoint ret = GetMapSize();
	ret.x *= GetScale();
	ret.y *= GetScale();
	return ret;
}

int M_InputManager::GetScale()
{
	return App->win->GetScale();
}


int M_InputManager::GetRealScale()
{
	return App->win->GetRealScale();
}

void M_InputManager::SetScale(uint scale)
{
	App->win->SetScale(scale);
}

void M_InputManager::EnableCursorImage(bool enabled)
{
	if (enabled)
	{
		App->input->EnableCursorImage();
	}
	else
	{
		App->input->DisableCursorImage();
	}
}