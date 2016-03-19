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

	//Getters
	Building_Type GetType();

	//Drawing methods
	void Draw();
	void DrawDebug();

public:


private:
	Building_Type type = PYLON;

};
#endif //__BUILDING_H__