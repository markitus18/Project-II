#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include "Controlled.h"
#include "Resource.h"

enum Building_Type
{
	NEXUS = 0,
	PYLON,
	ASSIMILATOR,
};

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

	//Drawing methods
	void Draw();
	void DrawDebug();

	void LoadLibraryData();

public:
	int shield;
	int armor;

	int cost;
	//Resource_Type costType;

	int width_tiles;
	int height_tiles;

	int buildTime;
	int psi;

	Resource* gasResource = NULL;

private:
	Building_Type type;

};
#endif //__BUILDING_H__