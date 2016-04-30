#ifndef __M_EVENTS__
#define __M_EVENTS__

#include "j1Module.h"

enum e_events
{
	//Moving camera
	E_CAMERA_UP = 0,
	E_CAMERA_RIGHT,
	E_CAMERA_DOWN,
	E_CAMERA_LEFT,

	//UI interactions
	E_OPEN_MENU,
	E_OPEN_CONSOLE,
	
	//Orders
	E_ATTACK,
	E_GATHER,
	E_PATROL,
	E_MOVE,
	E_STOP,

	//Units
	//Protoss units
	E_SPAWN_CARRIER,
	E_SPAWN_OBSERVER,
	E_SPAWN_PROBE,
	E_SPAWN_SHUTTLE,
	E_SPAWN_SCOUT,
	E_SPAWN_REAVER,
	E_SPAWN_ZEALOT,
	E_SPAWN_HIGH_TEMPLAR,
	E_SPAWN_DARK_TEMPLAR,
	E_SPAWN_DRAGOON,
	//Zerg units
	E_SPAWN_ZERGLING,
	E_SPAWN_MUTALISK,
	E_SPAWN_HYDRALISK,
	E_SPAWN_ULTRALISK,
	E_SPAWN_INFESTED_TERRAN,
	E_SPAWN_KERRIGAN,
	//Debug units
	E_SPAWN_GODMODE,

	//Buildings
	//Protoss buildings
	E_SPAWN_NEXUS,
	E_SPAWN_PYLON,
	E_SPAWN_ASSIMILATOR,
	E_SPAWN_GATEWAY,
	E_SPAWN_CYBERNETICS_CORE,
	E_SPAWN_PHOTON_CANNON,
	E_SPAWN_STARGATE,
	E_SPAWN_ROBOTICS_FACILITY,
	E_SPAWN_CITADEL,
	E_SPAWN_TEMPLAR_ARCHIVES,
	E_SPAWN_ROBOTICS_BAY,
	E_SPAWN_ZERG_SAMPLE,
	//Zerg buildings
	E_SPAWN_LAIR,
	E_SPAWN_SPAWNING_POOL,
	E_SPAWN_HYDRALISK_DEN,
	E_SPAWN_SPIRE,
	E_SPAWN_ULTRALISK_CAVERN,
	E_SPAWN_INFESTED_COMMAND_CENTER,
	E_SPAWN_HIVE,
	E_SPAWN_SUNKEN_COLONY,
	E_SPAWN_SPORE_COLONY,

};

class M_InputManager : public j1Module
{

public:

	M_InputManager(bool);

	// Destructor
	virtual ~M_InputManager();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();
};

#endif // __j1INPUT_H__