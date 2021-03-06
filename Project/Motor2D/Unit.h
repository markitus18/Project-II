#ifndef __UNIT_H__
#define __UNIT_H__

#include "C_Vec2.h"
#include "C_DynArray.h"
#include "M_Missil.h"

#include "Entity.h"
#include "Controlled.h"

#define TIME_TO_ERASE_UNIT 6.0f

enum Unit_Type
{
	CARRIER = 0,//---------Only for cinematics purpose
	OBSERVER,
	PROBE,
	SHUTTLE,//-------------Only for cinematics purpose
	SCOUT,
	REAVER,
	ZEALOT,
	HIGH_TEMPLAR,
	DARK_TEMPLAR,
	DRAGOON,
	ZERGLING,
	MUTALISK,
	HYDRALISK,
	ULTRALISK,
	INFESTED_TERRAN,
	KERRIGAN,
	GODMODE,
	SCOUT_CIN,//-------------Only for cinematics purpose
	UNIT_NONE,

};

enum Unit_Movement_State
{
	MOVEMENT_IDLE,
	MOVEMENT_MOVE,
	MOVEMENT_GATHER,
	MOVEMENT_WAIT,
	MOVEMENT_ATTACK_IDLE,
	MOVEMENT_ATTACK_ATTACK,
	MOVEMENT_DIE,
	MOVEMENT_DEAD,

	MOVEMENT_BOSS_STUNNED,
	MOVEMENT_BOSS_EXPLODING,
};

enum Unit_State
{
	STATE_STAND,
	STATE_MOVE,
	STATE_PATROL,
	STATE_ATTACK,
	STATE_GATHER,
	STATE_GATHER_RETURN,
	STATE_BUILD,
	STATE_DIE,

	STATE_BOSS_STUNNED,
	STATE_BOSS_EXPLOSION,
};

enum Attack_State
{
	ATTACK_STAND,
	ATTACK_DEFEND,
	ATTACK_ATTACK,
};

enum Player_Type;

struct UnitStats
{
	float speed = 150.0f;

	int attackRange = 100;
	float attackSpeed = 1;
	int attackDmg = 4;
	int canAttackFlying = false;

	int visionRange = 200;
	int armor = 0;
	int maxShield = 100;
	int shield = 100;

	Unit_Type type;
	Player_Type player;
};

class Resource;
class Building;
enum Resource_Type;
enum Building_Type;
struct UnitSpriteData;

class Unit : public Controlled
{
public:
	Unit();
	Unit(float x, float y, Unit_Type _type, Player_Type);
	Unit(fPoint);
	Unit(Unit&);

	~Unit();

	bool Start();
	virtual bool Update(float dt);

	//Movement functions---------------------------
	void SetTarget(int x, int y);
	void Move(iPoint dst, Attack_State, e_priority priority = PRIORITY_MEDIUM);
	bool SetNewPath(iPoint dst, e_priority priority = PRIORITY_MEDIUM);
	//---------------------------------------------

	//Gathering functions -------------------------
	void SetGathering(Resource* resource);
	void SetGathering(Building* building);
	void ExitAssimilator(bool hasResource);
	void ReturnResource();
	//---------------------------------------------

	//Building functions --------------------------
	void UpdateBuildState();
	void SendToBuild(Building_Type building, iPoint tile);

	//Attack functions ----------------------------
	void SetAttack(Unit* unit);
	void SetAttack(Building* unit, Attack_State);
	bool IsInRange(Unit* unit);
	bool IsInRange(Building* unit);
	bool IsInRange(Resource* resource);
	bool HasVision(Unit* unit);
	bool HasVision(Building* unit);
	bool Hit(int amount);

	void RegenShield();
	bool RegenHP();

	//Event functions -----------------------------
	void Horrified(int x, int y, int horrorRadius);

	//---------------------------------------------
	void Stop();

	//Setters
	void SetType(Unit_Type _type);
	void SetMaxSpeed(float speed);
	void SetPriority(int priority);

	//Getters
	C_Vec2<float> GetVelocity() const;
	Unit_Type GetType() const;
	Unit_State GetState() const;
	Unit_Movement_State GetMovementState() const;
	Attack_State GetAttackState() const;
	void UpdateCollider();

	void StartDeath();
	void Destroy();

	void CheckMouseHover();
	//Drawing methods------------------------------
	void Draw(float dt);
	void DrawDebug();
	//---------------------------------------------

protected:

	//Movement functions --------------------------
	void UpdateMovement(float dt);
	bool UpdatePosition(float dt);
	bool UpdateVelocity(float dt);

	void UpdateBarPosition();
	void UpdateSprite(float dt);
	void UpdateSpriteState();

	void GetDesiredVelocity();

	bool Rotate(float dt);

	bool GetNewTarget();

	bool isTargetReached();
	bool isAngleReached();
	//---------------------------------------------

	//Gather functions ----------------------------
	void UpdateGatherState();
	void UpdateGatherReturnState();
	void UpdateGather(float dt);
	void UpdateGatherSprite();
	void UpdateGatherSpark(float dt);

	//---------------------------------------------

	//Attack functions ----------------------------
	virtual void UpdateAttackState(float dt);
	virtual void UpdateAttack(float dt);
	virtual void Attack();
	// --------------------------------------------

	//Look functions-------------------------------
	void LookAt(Unit*);
	void LookAt(Building*);
	void LookAt(Resource*);
	//---------------------------------------------
	void UpdateDeath();
	bool EraseUnit();

	void LoadLibraryData();
public:
	UnitStats stats;
	const UnitSpriteData* spriteData;

	int pathIndex = 0;

	int base_offset_x;
	int base_offset_y;

	int shadow_offset_x;
	int shadow_offset_y;

	//Collision variables
	std::vector<iPoint> path;
	int priority;
	int colRadius = 5 * 4;

	//Path variables
	int currentNode = 0;
	bool targetReached = true;

	float currentFrame = 0;

	//Gathering variables
	float gatheredAmount = 0;
	Resource_Type gatheredType;
	int gatherSpeed = 1;
	Resource* gatheringResource = NULL;
	Building* gatheringBuilding = NULL;
	Building* gatheringNexus = NULL;

	//Building variables
	Building_Type buildingToCreate;
	iPoint tileToBuild;

	//Attacking variables
	Unit* attackingUnit;
	Building* attackingBuilding;

	int flyingOffset = 0;
	bool waitingForPath = false;
	Unit_Movement_State movement_state = MOVEMENT_IDLE;

	j1Timer actionTimer;
	j1Timer attackTimer;
	j1Timer logicTimer;

protected:

	Unit_State state = STATE_STAND;
	Attack_State attackState = ATTACK_ATTACK;

	C_Sprite gatherSprite;
	C_Sprite gatherShadow;
	C_Sprite gatherSpark;
	float gatherFrame = 0;
	bool  secondGatherSound = false;
	bool  thirdGatherSound = false;
	//Movement variables--------------------------
	iPoint target;
	//Velocities
	C_Vec2<float> currentVelocity = { 0, 0 };
	C_Vec2<float> desiredVelocity = { 0, 0 };

	float rotationSpeed = 500.0f; //Used as angles / seconds
	float targetRadius = 3.0f;
	//--------------------------------------------

};

#endif //__UNIT_H__