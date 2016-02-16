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
	Unit(int x, int y);
	~Unit();

	bool Start();
	bool Update(float dt);

	//Movement methods
	bool UpdateVelocity(float dt);
	bool Unit::GetDesiredVelocity(p2Vec2<float>& newDesiredVelocity);
	p2Vec2<float> GetSteeringVelocity();
	p2Vec2<float> GetcurrentVelocity(float dt, bool isRotating);

	bool GetNewTarget();

	//Setters
	void SetTarget(int x, int y);
	void SetNewPath(p2DynArray<PathNode>& newPath);
	void SetType(UnitType _type);
	void SetMaxSpeed(float speed);

	//Getters
	Entity_Directions GetDirection();
	float GetSlowRad();
	UnitType GetType();

	//Drawing methods
	void Draw();
	void DrawDebug();

	//HP controllers
	void CreateBar();
private:
	UnitType type = RED;

	UIBar* HPBar;

	//float speed = 300.0f;
	float maxSpeed = 0.5f;
	float maxForce = 0.5f; //MaxForce is capped at 1;
	float slowingRadius = 5.0f;

	p2DynArray<PathNode> path;
	iPoint target;
	bool targetChange = false;
	int currentNode = 0;
	p2Vec2<float> currentVelocity = { 0, 0 };
	p2Vec2<float> steeringVelocity = { 0, 0 };
	p2Vec2<float> desiredVelocity = { 0, 0 };
public:
};

#endif //__UNIT_H__