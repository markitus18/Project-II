#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"

#include "Entity.h"

enum Building_Type
{
	PYLON = 0,
};

class Building : public Entity
{
public:
	Building();
	Building(float x, float y);
	Building(fPoint);
	~Building();

	bool Start();
	bool Update(float dt);

	//Setters 
	void SetType(Building_Type _type);
	void SetPriority(int priority);

	//Getters
	Building_Type GetType();

	void UpdateCollider();

	//Drawing methods
	void Draw();
	void DrawDebug();

public:
	//Collision variables
	int priority;

private:
	Building_Type type = PYLON;
	bool walkable = false;
};
#endif //__BUILDING_H__