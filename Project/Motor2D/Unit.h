#ifndef __UNIT_H__
#define __UNIT_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"
#include "M_Missil.h"


#include "Entity.h"
#include "Controlled.h"

#include "j1Timer.h"

enum Unit_Type
{
	CARRIER = 0,
	OBSERVER,
	PROBE,
	SHUTTLE,
	ARBITER,
	SCOUT,
	REAVER,
	ZEALOT,
	ARCHON_T,
	HIGH_TEMPLAR,
	DARK_TEMPLAR,
	DRAGOON,
//	INTERCEPTOR,
//	CORSAIR,
	ZERGLING,
	MUTALISK,
	HYDRALISK,
	ULTRALISK,
};

enum Unit_Movement_State
{
	MOVEMENT_IDLE,
	MOVEMENT_MOVE,
	MOVEMENT_GATHER,
	MOVEMENT_WAIT,
	MOVEMENT_ATTACK,
	MOVEMENT_DIE,
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
	int visionRange = 200;
	int armor = 0;
	int shield = 100;

	Unit_Type type;
	Player_Type player;
};

class Resource;
class Building;

class Unit : public Controlled
{
public:
	Unit();
	Unit(float x, float y, Unit_Type _type, Player_Type);
	Unit(fPoint);
	~Unit();

	bool Start();
	bool Update(float dt);

	//Movement functions---------------------------
	void SetTarget(int x, int y);
	void Move(iPoint dst, Attack_State);
	bool SetNewPath(iPoint dst);
	//---------------------------------------------

	//Gathering functions -------------------------
	void SetGathering(Resource* resource);
	void SetGathering(Building* building);
	void ExitAssimilator(bool hasResource);
	void ReturnResource();
	//---------------------------------------------

	//Attack functions ----------------------------
	void SetAttack(Unit* unit);
	void SetAttack(Building* unit);
	bool IsInRange(Unit* unit);
	bool IsInRange(Building* unit);
	bool HasVision(Unit* unit);
	bool HasVision(Building* unit);
	bool Hit(int amount);
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

private:

	//Movement functions --------------------------
	void UpdateMovement(float dt);
	bool UpdatePosition(float dt);
	bool UpdateVelocity(float dt);

	void UpdateBarPosition();

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
	//---------------------------------------------

	//Attack functions ----------------------------
	void UpdateAttackState(float dt);
	void UpdateAttack(float dt);
	// --------------------------------------------

	bool UpdateDeath(float dt);

	void LoadLibraryData();
public:
	UnitStats stats;

	int base_offset_x;
	int base_offset_y;

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
	int gatherSpeed = 1;
	Resource* gatheringResource = NULL;
	Building* gatheringBuilding = NULL;
	Building* gatheringNexus = NULL;

	//Attacking variables
	Unit* attackingUnit;
	Building* attackingBuilding;

	int flyingOffset = 0;
	bool waitingForPath = false;
private:
	Unit_Movement_State movement_state = MOVEMENT_IDLE;
	Unit_State state = STATE_STAND;
	Attack_State attackState = ATTACK_ATTACK;

	//Movement variables--------------------------
	iPoint target;


	//Velocities
	C_Vec2<float> currentVelocity = { 0, 0 };
	C_Vec2<float> desiredVelocity = { 0, 0 };

	//Movement variables
	float maxSpeed = 150.0f; //Big max speed could get bugged
	float rotationSpeed = 500.0f; //Used as angles / seconds
	float targetRadius = 3.0f;
	//--------------------------------------------

	j1Timer actionTimer;
	j1Timer logicTimer;
};

#endif //__UNIT_H__