#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include "Controlled.h"

enum Building_Type
{
	PYLON = 0,
};

class Building : public Controlled
{
public:
	Building();
	Building(int x, int y);
	Building(fPoint);
	~Building();

	bool Start();
	bool Update(float dt);

	//Setters 
	void SetType(Building_Type _type);

	//Getters
	Building_Type GetType();

	void UpdateBarPosition();
	//Drawing methods
	void Draw();
	void DrawDebug();

public:


private:
	Building_Type type = PYLON;

};
#endif //__BUILDING_H__