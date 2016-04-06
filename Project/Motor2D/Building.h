#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include "Controlled.h"
#include "j1Timer.h"

enum Building_Type
{
	NEXUS = 0,
	PYLON,
	ASSIMILATOR,
	GATEWAY,
};
class Unit;
class Resource;

class Building : public Controlled
{
public:
	Building();
	Building(int x, int y, Building_Type _type);
	~Building();

	bool Start();
	bool Update(float dt);

	//Getters
	Building_Type GetType() const;

	void UpdateBarPosition();

	void ChangeTileWalkability(bool);

	void AskToEnter(Unit* unit);
	void CheckMouseHover();
	bool Hit(int amount);
	//Drawing methods
	void Draw();
	void DrawDebug();

	void LoadLibraryData();

public:
	int shield;
	int armor;

	int width_tiles;
	int height_tiles;

	int buildTime;

	Resource* gasResource = NULL;
	bool hasWaypoint = false;
	iPoint waypointTile;

private:
	Building_Type type;
	j1Timer gatheringTimer;
	Unit* gatheringUnit;

};
#endif //__BUILDING_H__