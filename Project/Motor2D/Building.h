#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include "M_EntityManager.h"
#include "Controlled.h"
#include "j1Timer.h"
#include "C_BuildingQueue.h"

#define TIME_TO_ERASE_BUILDING 3.0f

enum Building_Type
{
	NEXUS = 0,
	PYLON,
	ASSIMILATOR,
	GATEWAY,
	ZERG_SAMPLE,
	LAIR,
	SPAWNING_POOL,
	HYDRALISK_DEN,
	SPIRE,
	ULTRALISK_CAVERN,
	INFESTED_COMMAND_CENTER,
	CYBERNETICS_CORE,
	FORGE,
	PHOTON_CANNON,
};

enum Player_Type;
enum Unit_Type;

struct BuildingData
{
	Player_Type player;
	int maxShield = 100;
	int shield = 100;
};

enum BuildingState
{
	BS_DEFAULT,
	BS_ATTACKING,
	BS_DEAD,
};

class Unit;
class Resource;

class Building : public Controlled
{
public:
	Building();
	Building(int x, int y, Building_Type _type);
	~Building();

	bool Start();
	bool Update(float dt);

	//Getters
	Building_Type GetType() const;

	void UpdateBarPosition();

	void ChangeTileWalkability(bool);

	void AskToEnter(Unit* unit);
	void CheckMouseHover();
	bool Hit(int amount);
	void RegenShield();

	void AddNewUnit(Unit_Type type, int creationTime, int unitPsi);
	Unit* CreateUnit(Unit_Type type, Player_Type controller = PLAYER);
	void UpdateQueue();

	iPoint FindCloseWalkableTile();

	void StartDeath();
	void Destroy();

	//Drawing methods
	void Draw();
	void DrawDebug();

	void LoadLibraryData();

	iPoint GetWorldPosition();

public:
	BuildingData stats;
	BuildingState state = BS_DEFAULT;

	C_Animation fire;

	int armor;

	int width_tiles;
	int height_tiles;

	int buildTime;

	C_BuildingQueue queue;

	Resource* gasResource = NULL;
	bool hasWaypoint = false;
	iPoint waypointTile;

private:
	Building_Type type;
	j1Timer logicTimer;
	j1Timer gatheringTimer;
	Unit* gatheringUnit;

};
#endif //__BUILDING_H__