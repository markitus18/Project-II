#include "M_CollisionController.h"

#include "j1App.h"

#include "M_Input.h"
#include "M_Textures.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_Map.h"
#include "M_PathFinding.h"
#include "Unit.h"
#include "Entity.h"
#include "M_EntityManager.h"
#include "UIElements.h"


M_CollisionController::M_CollisionController(bool start_enabled) : j1Module(start_enabled)
{
	name.create("collision_controller");
}

// Destructor
M_CollisionController::~M_CollisionController()
{}

// Called before render is available
bool M_CollisionController::Awake(pugi::xml_node& node)
{

	return true;
}

// Called before the first frame
bool M_CollisionController::Start()
{
	CreateMap();
	return true;
}

// Called each loop iteration
bool M_CollisionController::PreUpdate()
{

	return true;
}

// Called each loop iteration
bool M_CollisionController::Update(float dt)
{

	return true;
}

// Called each loop iteration
bool M_CollisionController::PostUpdate(float dt)
{
	bool ret = true;
	
	return ret;
}

// Called before quitting
bool M_CollisionController::CleanUp()
{
	RELEASE(mapData.data);
	return true;
}

void M_CollisionController::ManageInput(float dt)
{

}

void M_CollisionController::CreateMap()
{
	mapData.width = App->pathFinding->mapData.width;
	mapData.height = App->pathFinding->mapData.height;
	mapData.data = new uint[mapData.width * mapData.height];
}