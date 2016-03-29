#ifndef __UNIT_H__
#define __UNIT_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"


#include "Entity.h"
#include "Controlled.h"

enum Unit_Type
{
	CARRIER = 0,
	OBSERVER,
	PROBE,
	SAPPER,
	SHUTTLE,
	ARBITER,
	INTERCEP,
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
	MOVEMENT_ATTACK,
	MOVEMENT_DIE,
};

enum Unit_State
{
	STATE_STAND,
	STATE_MOVE,
	STATE_ATTACK,
	STATE_GATHER
};

class Unit : public Controlled
{
public:
	Unit();
	Unit(float x, float y);
	Unit(fPoint);
	~Unit();

	bool Start();
	bool Update(float dt);

	//Movement functions---------------------------
	void SetTarget(int x, int y);
	void SetNewPath(std::vector<iPoint>& newPath);
	//---------------------------------------------

	//Setters
	void SetType(Unit_Type _type);
	void SetMaxSpeed(float speed);
	void SetPriority(int priority);

	//Getters
	C_Vec2<float> GetVelocity() const;
	Unit_Type GetType() const;
	Unit_Movement_State GetState() const;
	void GetTextureRect(SDL_Rect&, SDL_RendererFlip&) const;

	void UpdateCollider();

	void Destroy();

	//Drawing methods------------------------------
	void Draw(float dt);
	void DrawDebug();
	//---------------------------------------------

private:

	//Movement functions --------------------------
	void UpdateMovement(float dt);
	bool UpdateVelocity(float dt);
	void UpdateBarPosition();

	void GetDesiredVelocity();

	bool Rotate(float dt);
	bool Move(float dt);

	bool GetNewTarget();

	bool isTargetReached();
	bool isAngleReached();
	//--------------------------------------------

public:
	//Collision variables
	std::vector<iPoint> path;
	int priority;
	int colRadius = 5 * 4;
	
	//Path variables
	int currentNode = 0;
	bool targetReached = true;

	float currentFrame = 0;

private:


	iPoint target;
	Unit_Type type = ARBITER;
	Unit_Movement_State movement_state = MOVEMENT_IDLE;
	Unit_State state = STATE_STAND;
	//Velocities
	C_Vec2<float> currentVelocity = { 0, 0 };
	C_Vec2<float> desiredVelocity = { 0, 0 };

	//Movement variables
	float maxSpeed =  150.0f; //Big max speed could get bugged
	float rotationSpeed = 500.0f; //Used as angles / seconds
	float targetRadius = 2.0f;
};

#endif //__UNIT_H__