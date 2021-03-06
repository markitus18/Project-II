#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Entity.h"
#include "Controlled.h"

enum Resource_Type
{
	MINERAL = 0,
	GAS,
};

class Unit;

class Resource : public Entity
{
public:
	Resource();
	Resource(int x, int y, Resource_Type _type);
	~Resource();

	bool Start();
	bool Update(float dt);
	void Destroy();
	//Setters 
	void SetType(Resource_Type _type);

	//Getters
	Resource_Type GetType();

	float	Extract(float amount);
	void	UpdateTexture();
	void	ChangeTileWalkability(bool);
	void	CheckMouseHover();

	iPoint FindCloseWalkableTile();

	//Drawing methods
	void Draw();
	void DrawDebug();

	void LoadLibraryData();

private:
	Resource_Type type;
	C_Sprite assimilatorSprite;

public:
	bool ocupied = false;
	float resourceMaxAmount = 100;
	float resourceAmount = 100;
	
	Unit* gatheringUnit = NULL;

	int width_tiles;
	int height_tiles;

};
#endif //__RESOURCE_H__