#ifndef __UNIT_H__
#define __UNIT_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"

#include "Entity.h"
#include "Controlled.h"

enum Unit_Type
{
	ARBITER = 0,
};

class UIBar;

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
	void GetDesiredVelocity();
	C_Vec2<float> GetcurrentVelocity();

	bool Rotate(float dt);
	bool Move(float dt, bool& col);

	bool GetNewTarget();
	bool isTargetReached();
	bool isAngleReached();

	//Setters
	void SetTarget(int x, int y);
	void SetNewPath(C_DynArray<iPoint>& newPath);
	void SetType(Unit_Type _type);
	void SetMaxSpeed(float speed);
	void SetPriority(int priority);

	//Getters
	void GetTextureRect(SDL_Rect&, SDL_RendererFlip&) const;
	Unit_Type GetType();
	
	void UpdateCollider();
	void Destroy();


	//Drawing methods
	void Draw();
	void DrawDebug();

public:
	//Collision variables
	int priority;
	int colRadius;

private:
	Unit_Type type = ARBITER;

	//Velocities
	C_Vec2<float> currentVelocity = { 0, 0 };
	C_Vec2<float> desiredVelocity = { 0, 0 };

	//Movement variables
	float maxSpeed =  150.0f; //Big max speed could get bugged
	float rotationSpeed = 360.0f; //Used as angles / seconds
	float targetRadius = 2.0f;


public:
	C_DynArray<iPoint> path;
	iPoint target;
	int currentNode = 0;
	bool targetReached = true;

};

#endif //__UNIT_H__