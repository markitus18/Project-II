#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "j1Module.h"

class Unit;
enum Unit_Type;

class M_EntityManager : public j1Module
{
public:
	M_EntityManager(bool);
	~M_EntityManager();

	bool PreStart(pugi::xml_node& node);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void DoUnitLoop(float dt);
	void UpdateSelectionRect();

	void ManageInput();

	Unit* CreateUnit(int x, int y, Unit_Type);
	bool deleteUnit(C_List_item<Unit*>* item);

	bool IsUnitSelected(C_List_item<Unit*>*);

	void SendNewPath(int x, int y);

	SDL_Texture* GetTexture(Unit_Type);

	//	bool addBuilding(Entity& _entity);
	//	bool deleteBuilding();

	void DrawDebug();
	//should be priv
public:
	bool continuous = true;
	bool smooth = true;

	bool selectUnits = false;

	SDL_Rect selectionRect;
	SDL_Rect groupRect;
	SDL_Rect destinationRect;

	C_List<Unit*> unitList;
	C_List<Unit*> selectedUnits;
	C_List<Unit*> unitsToDelete;

	SDL_Texture* entity_tex;
	SDL_Texture* unit_base;
	SDL_Texture* path_tex;
	SDL_Texture* hpBar_empty;
	SDL_Texture* hpBar_filled;
	SDL_Texture* hpBar_mid;
	SDL_Texture* hpBar_low;

	//Collision variables
	int currentPriority = 1;

private:

	void AddUnit(Unit* unit);
	//C_List<Building*> buildingList;

};

#endif //_ENTITYMANAGER_H__