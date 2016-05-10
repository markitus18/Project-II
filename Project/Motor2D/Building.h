#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Controlled.h"

#include "C_BuildingQueue.h"

#define TIME_TO_ERASE_BUILDING 3.0f

enum Building_Type
{
	NEXUS = 0,
	PYLON,		
	ASSIMILATOR,
	GATEWAY,	
	CYBERNETICS_CORE,	
	PHOTON_CANNON,	
	STARGATE,	
	ROBOTICS_FACILITY,	
	CITADEL,	
	TEMPLAR_ARCHIVES,	
	ROBOTICS_BAY,	
	ZERG_SAMPLE,	
	LAIR,
	SPAWNING_POOL,
	HYDRALISK_DEN,
	SPIRE,
	ULTRALISK_CAVERN,
	INFESTED_COMMAND_CENTER,
	HIVE,
	SUNKEN_COLONY,
	SPORE_COLONY,
};

enum Player_Type;
enum Unit_Type;

struct BuildingData
{
	Player_Type player;
	int maxShield = 100;
	int shield = 100;
	int visionRange = 300;
	int damage = 20;
	int attackRange = 100;
};

enum BuildingState
{
	BS_DEFAULT,
	BS_SPAWNING,
	BS_ATTACKING,
	BS_DEAD,
};

class Unit;
class Resource;

class Building : public Controlled
{
public:
	Building();
	Building(int x, int y, Building_Type _type, Player_Type player);
	Building(Building& toCopy);
	~Building();

	bool Start();
	bool Update(float dt);

	//Getters
	Building_Type GetType() const;

	void UpdateBarPosition();

	bool UpdateDeath(float dt);
	void ChangeTileWalkability(bool);

	void AskToEnter(Unit* unit);
	void CheckMouseHover();
	bool HasVision(Unit*);
	void SetAttack(Unit*);
	//void Attack();
	void UpdateAttack();
	bool Hit(int amount);
	void RegenShield();
	bool RegenHP();

	void AddNewUnit(Unit_Type type, int creationTime, int unitPsi);
	Unit* CreateUnit(Unit_Type type, Player_Type);
	void UpdateQueue();
	float GetQueuePercentage();
	void RemoveFromQueue(int position);

	void UpdateSpawn(float dt);
	void FinishSpawn();

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
	
	C_Sprite pylonArea;

	C_Animation spawn_animation;
	C_Animation fire;
	C_Animation fire2;
	C_Animation fire3;
	C_Animation death_animation;

	int armor;

	int width_tiles;
	int height_tiles;

	int buildTime;

	C_BuildingQueue queue;

	Resource* gasResource = NULL;
	bool hasWaypoint = false;
	iPoint waypointTile;
	bool finished = false;
private:

	Building_Type type;
	j1Timer logicTimer;
	j1Timer gatheringTimer;
	j1Timer attackTimer;
	Unit* gatheringUnit = NULL;
	Unit* attackingUnit = NULL;


};
#endif //__BUILDING_H__