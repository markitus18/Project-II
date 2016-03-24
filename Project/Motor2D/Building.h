#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include "Controlled.h"

enum Building_Type
{
	NEXUS = 0,
	PYLON,
};

class Building : public Controlled
{
public:
	Building();
	Building(int x, int y, Building_Type _type);
	~Building();

	bool Start();
	bool Update(float dt);

	//Setters 
	void SetType(Building_Type _type);

	//Getters
	Building_Type GetType();

	void UpdateBarPosition();

	void ChangeTileWalkability();

	//Drawing methods
	void Draw();
	void DrawDebug();

public:
	int shield;
	int armor;

	int cost;
	//Resource_Type costType;

	int width_tiles;
	int height_tiles;

	int buildTime;
	int psi;

private:
	Building_Type type;

};
#endif //__BUILDING_H__