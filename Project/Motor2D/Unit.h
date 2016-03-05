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

enum Collision_State
{
	NONE,
	RESOLVING,
	RESOLVED
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
	C_Vec2<float> GetcurrentVelocity();

	void Rotate(float dt);
	bool Move(float dt);
	void Freeze();
	void Unfreeze();

	bool GetNewTarget();
	bool isTargetReached();
	bool isAngleReached();

	//Setters
	void SetTarget(int x, int y);
	void SetNewPath(C_DynArray<PathNode>& newPath);
	void SetType(Unit_Type _type);
	void SetMaxSpeed(float speed);
	void SetPriority(int priority);
	void SetCollider(SDL_Rect);
	void SetSoftCollider(SDL_Rect);

	//Getters
	void GetTextureRect(SDL_Rect&, SDL_RendererFlip&) const;
	Unit_Type GetType();
	
	//Collision controllers
	bool CheckCollisions();
	void UpdateCollider();
	void Destroy();


	//Drawing methods
	void Draw();
	void DrawDebug();

private:
	Unit_Type type = ARBITER;


	//Path variables
	C_DynArray<PathNode> path;
	iPoint target;
	bool targetReached = true;
	int currentNode = 0;

	//Velocities
	C_Vec2<float> currentVelocity = { 0, 0 };
	C_Vec2<float> desiredVelocity = { 0, 0 };

	//Movement variables
	float maxSpeed =  150.0f; //Big max speed could get bugged
	float rotationSpeed = 180.0f; //Used as angles / seconds
	float targetRadius = 2.0f;
	bool frozen = false;

	//Collision variables
	int priority;

	SDL_Rect collider;
	SDL_Rect softCollider;
public:
};

#endif //__UNIT_H__