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