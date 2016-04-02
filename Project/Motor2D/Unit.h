#ifndef __UNIT_H__
#define __UNIT_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"


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
};
class Resource;
class Building;

class Unit : public Controlled
{
public:
	Unit();
	Unit(float x, float y, Unit_Type _type);
	Unit(fPoint);
	~Unit();

	bool Start();
	bool Update(float dt);

	//Movement functions---------------------------
	void SetTarget(int x, int y);
	void Move(iPoint dst);
	bool SetNewPath(iPoint dst);
	//---------------------------------------------

	//Gathering functions -------------------------
	void SetGathering(Resource* resource);
	void SetGathering(Building* building);
	void ExitAssimilator(bool hasResource);
	void ReturnResource();
	//---------------------------------------------
	void Stop();

	//Setters
	void SetType(Unit_Type _type);
	void SetMaxSpeed(float speed);
	void SetPriority(int priority);

	//Getters
	C_Vec2<float> GetVelocity() const;
	Unit_Type GetType() const;
	Unit_Movement_State GetState() const;

	void UpdateCollider();

	void Destroy();

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

	void LoadLibraryData();
public:
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

	int flyingOffset = 0;

private:
	Unit_Type type;
	Unit_Movement_State movement_state = MOVEMENT_IDLE;
	Unit_State state = STATE_STAND;

	//Movement variables--------------------------
	iPoint target;

	//Velocities
	C_Vec2<float> currentVelocity = { 0, 0 };
	C_Vec2<float> desiredVelocity = { 0, 0 };

	//Movement variables
	float maxSpeed =  150.0f; //Big max speed could get bugged
	float rotationSpeed = 500.0f; //Used as angles / seconds
	float targetRadius = 2.0f;
	//--------------------------------------------

	//Gathering variables
	j1Timer gatheringTimer;
};

#endif //__UNIT_H__