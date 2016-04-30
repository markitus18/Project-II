#include "M_InputManager.h"

#include "j1App.h"


M_InputManager::M_InputManager(bool start_enabled) : j1Module(start_enabled)
{
	name.create("inputManager");
}

// Destructor
M_InputManager::~M_InputManager()
{

}

// Called when before render is available
bool M_InputManager::Awake(pugi::xml_node&)
{
	return true;
}

// Call before first frame
bool M_InputManager::Start()
{
	return true;
}

// Called before all Updates
bool M_InputManager::PreUpdate()
{
	return true;
}

bool M_InputManager::Update(float dt)
{
	return true;
}

// Called after all Updates
bool M_InputManager::PostUpdate()
{
	return true;
}

// Called before quitting
bool M_InputManager::CleanUp()
{
	return true;
}