#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Entity.h"
#include "Controlled.h"

enum Resource_Type
{
	MINERAL = 0,
	GAS,
};

class Resource : public Entity
{
public:
	Resource();
	Resource(int x, int y, Resource_Type _type);
	~Resource();

	bool Start();
	bool Update(float dt);

	//Setters 
	void SetType(Resource_Type _type);

	//Getters
	Resource GetType();

	void ChangeTileWalkability(bool);

	//Drawing methods
	void Draw();
	void DrawDebug();

	void LoadLibraryData();

private:
	int resourceMaxAmount = 100;
	int resourceAmount = 100;

	int width_tiles;
	int height_tiles;

	Resource_Type type;

};
#endif //__RESOURCE_H__