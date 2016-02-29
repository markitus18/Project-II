#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "j1Module.h"

class Unit;
enum Unit_Type;

class EntityManager : public j1Module
{
public:
	EntityManager(bool);
	~EntityManager();

	bool PreStart(pugi::xml_node& node);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void ManageInput();

	Unit* CreateUnit(int x, int y, Unit_Type);
	bool deleteUnit();

	bool IsUnitSelected(C_List_item<Unit*>*);
	void SendNewPath(int x, int y);

	SDL_Texture* GetTexture(Unit_Type);
	//	bool addBuilding(Entity& _entity);
	//	bool deleteBuilding();
	//should be priv+
	bool continuous = true;
	bool smooth = true;

	SDL_Rect selectionRect;
	C_List<Unit*> unitList;
	C_List<Unit*> selectedUnits;

	SDL_Texture* entity_tex;
	SDL_Texture* unit_base;
	SDL_Texture* path_tex;
private:

	//C_List<Building*> buildingList;

};

#endif //_ENTITYMANAGER_H__