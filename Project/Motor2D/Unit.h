#ifndef __UNIT_H__
#define __UNIT_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"

#include "Entity.h"
#include "Controlled.h"

enum Unit_Directions
{
	UP = 0,
	RIGHT,
	DOWN,
	LEFT,
};

enum Unit_Type
{
	RED = 0,
	YELLOW,
	GREEN,
	BLUE,
};

class UIBar;
struct PathNode;

class Unit : public Controlled
{
public:
	Unit();
	Unit(float x, float y);
	Unit(fPoint);
	~Unit();

	bool Start();
	bool Update(float dt);

	//Movement methods
	bool UpdateVelocity(float dt);
	bool GetDesiredVelocity(C_Vec2<float>& newDesiredVelocity);
	C_Vec2<float> GetSteeringVelocity();
	C_Vec2<float> GetcurrentVelocity();

	void Rotate(float dt);
	bool Move(float dt);

	bool GetNewTarget();
	bool isTargetReached();
	bool isAngleReached();

	//Setters
	void SetTarget(int x, int y);
	void SetNewPath(C_DynArray<PathNode>& newPath);
	void SetType(Unit_Type _type);
	void SetMaxSpeed(float speed);
	void SetDirection(Unit_Directions dir);

	//Getters
	Unit_Directions GetDirection();
	float GetTargetRad();
	Unit_Type GetType();

	//Drawing methods
	void Draw();
	void DrawDebug();

	//HP controllers
	//void CreateBar();

private:
	Unit_Type type = RED;

	//Path variables
	C_DynArray<PathNode> path;
	iPoint target;
	bool targetReached = true;
	int currentNode = 0;

	//Velocities
	C_Vec2<float> currentVelocity = { 0, 0 };
	C_Vec2<float> desiredVelocity = { 0, 0 };

	//Movement variables
	float maxSpeed =  80.0f; //Big max speed could get bugged
	float rotationSpeed = 50.0f; //Used as angles / seconds
	float targetRadius = 2.0f;
public:
};

#endif //__UNIT_H__