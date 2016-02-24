#ifndef __UNIT_H__
#define __UNIT_H__

#include "p2Point.h"
#include "p2Vec2.h"
#include "p2DynArray.h"

#include "Entity.h"

enum UnitType
{
	RED = 0,
	YELLOW,
	GREEN,
	BLUE,
};

class UIBar;
struct PathNode;

class Unit : public Entity
{
public:
	Unit();
	Unit(float x, float y);
	~Unit();

	bool Start();
	bool Update(float dt);

	//Movement methods
	bool UpdateVelocity(float dt);
	bool GetDesiredVelocity(p2Vec2<float>& newDesiredVelocity);
	p2Vec2<float> GetSteeringVelocity();
	p2Vec2<float> GetcurrentVelocity();

	void Rotate(float dt);
	bool Move(float dt);

	bool GetNewTarget();
	bool isTargetReached();
	bool isAngleReached();

	//Setters
	void SetTarget(int x, int y);
	void SetNewPath(p2DynArray<PathNode>& newPath);
	void SetType(UnitType _type);
	void SetMaxSpeed(float speed);

	//Getters
	Entity_Directions GetDirection();
	float GetTargetRad();
	UnitType GetType();

	//Drawing methods
	void Draw();
	void DrawDebug();

	//HP controllers
	void CreateBar();

private:
	UnitType type = RED;
	UIBar* HPBar;

	//Path variables
	p2DynArray<PathNode> path;
	iPoint target;
	bool targetReached = true;
	int currentNode = 0;

	//Velocities
	p2Vec2<float> currentVelocity = { 0, 0 };
	p2Vec2<float> desiredVelocity = { 0, 0 };
//	p2Vec2<float> steeringVelocity = { 0, 0 };

	//Movement variables
	float maxSpeed =  800000.0f; //Big max speed could get bugged
	float rotationSpeed = 5000.0f; //Used as angles / seconds
	float targetRadius = 5.0f;
	//float maxForce = 0.5f;
public:
};

#endif //__UNIT_H__