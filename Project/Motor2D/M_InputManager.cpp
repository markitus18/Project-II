#include "M_InputManager.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_Input.h"
#include "M_Window.h"


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
		std::pair<int, e_events> toPush;
		toPush.first = path.attribute("id").as_int();
		toPush.second = static_cast<e_events>(path.attribute("event").as_int());
		eventsList.insert(toPush);
	}


	return true;
}

// Call before first frame
bool M_InputManager::Start()
{
	App->win->GetWindowSize(&screenSize.x, &screenSize.y);
	return true;
}

// Called before all Updates
bool M_InputManager::PreUpdate()
{


	return true;
}

bool M_InputManager::Update(float dt)
{
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

void M_InputManager::SendEvent(int id, e_eventState state)
{
	std::map<int, e_events>::iterator tmp =	eventsList.find(id);
	if (tmp != eventsList.end())
	{
		std::pair<e_events, e_eventState> toPush;
		toPush.first = tmp->second;
		toPush.second = state;
		currentEvents.insert(toPush);
	}
}

void M_InputManager::SendMouseEvent(int button, e_eventState state)
{
	e_events tmp;
	switch (button)
	{
	case SDL_BUTTON_LEFT:
	{
		tmp = E_LEFT_CLICK; break;
	}
	case SDL_BUTTON_RIGHT:
	{
		tmp = E_RIGHT_CLICK; break;
	}
	case SDL_BUTTON_MIDDLE:
	{
		tmp = E_MID_CLICK; break;
	}
	default:
	{
		return;
	}
	}
	SendEvent(tmp, state);
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

int M_InputManager::GetScale()
{
	return App->win->GetScale();
}

void M_InputManager::SetScale(uint scale)
{
	App->win->SetScale(scale);
}