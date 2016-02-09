#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "j1Module.h"

class Unit;
enum UnitType;

class EntityManager : public j1Module
{
public:
	EntityManager(bool);
	~EntityManager();

	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void ManageInput();

	Unit* CreateUnit(int x, int y, UnitType);
	bool deleteUnit();

	bool IsUnitSelected(p2List_item<Unit*>*);
	void SendNewPath(int x, int y);
	//	bool addBuilding(Entity& _entity);
	//	bool deleteBuilding();
	//should be priv+
	SDL_Rect selectionRect;
	p2List<Unit*> unitList;
	p2List<Unit*> selectedUnits;

	SDL_Texture* entity_tex;
	SDL_Texture* unit_base;
	SDL_Texture* path_tex;
private:

	//p2List<Building*> buildingList;

};

#endif //_ENTITYMANAGER_H__