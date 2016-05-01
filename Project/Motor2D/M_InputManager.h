#ifndef __M_EVENTS__
#define __M_EVENTS__

#include "j1Module.h"
#include <map>

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
	TMP20,
	TMP21,
	TMP22,
	TMP23,
	
	//Orders
	E_ATTACK,
	E_GATHER,
	E_PATROL,
	E_MOVE,
	E_STOP,
	TMP1,
	TMP2,
	TMP3,
	TMP4,
	TMP5,
	TMP6,

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
	TMP7,
	//Zerg units
	E_SPAWN_ZERGLING,
	E_SPAWN_MUTALISK,
	E_SPAWN_HYDRALISK,
	E_SPAWN_ULTRALISK,
	E_SPAWN_INFESTED_TERRAN,
	E_SPAWN_KERRIGAN,
	TMP8,
	//Debug units
	E_SPAWN_GODMODE,
	TMP9,

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
	TMP10,

	//Debugging systems
	E_DEBUG_ENTITY_MANAGER,
	E_DEBUG_PATHFINDING,
	E_DEBUG_UI,
	E_DEBUG_EXPLOSIONS,
	E_DEBUG_FOW,
	E_DEBUG_ADD_MINERAL,
	E_DEBUG_ADD_GAS,
	E_DEBUG_ADD_PSI,
	E_DEBUG_WIN,
	E_DEBUG_LOOSE,

	E_DEBUG_ZOOM_OUT,
	E_DEBUG_ZOOM_IN,

	E_DEBUG_ADD_EXPLOSION,

	E_DEBUG_ADD_EXPLOSION_SYSTEM1,
	E_DEBUG_ADD_EXPLOSION_SYSTEM2,
	TMP11,
	TMP12,
	TMP13,
	TMP14,
	TMP15,

	//Mouse
	E_RIGHT_CLICK,
	E_LEFT_CLICK,
	E_MID_CLICK,

};

enum e_eventState
{
	EVENT_DOWN,
	EVENT_REPEAT,
	EVENT_UP,
	EVENT_NONE,
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
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	void SendEvent(int id, e_eventState state);

	void SendMouseEvent(int button, e_eventState state);

	e_eventState GetEvent(e_events _event);

	void UnfreezeInput();
	void FreezeInput();
	bool IsInputFrozen();

	iPoint GetMouseOnScreen() { return mouseScreen; }
	iPoint GetMouseOnWorld() { return mouseWorld; }

	iPoint GetScreenSize() { return{ screenSize.x, screenSize.y }; }
	iPoint GetScreenSizeScale() { return{ screenSize.x / GetScale(), screenSize.y / GetScale() }; }
	int GetScale();
	void SetScale(uint scale = 2);

	void EnableCursorImage(bool enabled);

private:
	//All events that may happen
	std::map<int, e_events> eventsList;
	//Events that are happening during this frame
	std::map<e_events, e_eventState> currentEvents;

	iPoint mouseScreen;
	iPoint mouseWorld;

	iPoint screenSize;

};

#endif // __j1INPUT_H__