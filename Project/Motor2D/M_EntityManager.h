#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "j1Module.h"

class Entity;
class Unit;
class Building;
enum Unit_Type;
enum Unit_State;
enum Building_Type;

struct UnitSprite
{
	SDL_Texture* texture;

	int size;
	float animationSpeed;

	int run_line_start;
	int run_line_end;

	int idle_line_start;
	int idle_line_end;

	int attack_line_start;
	int attack_line_end;
};

struct UnitSpritesData
{
	std::vector<Unit_Type>	unitType;
	std::vector<UnitSprite> data;

	const UnitSprite* GetData(Unit_Type) const;
	void GetStateLimits(Unit_Type type, Unit_State state, int& min, int& max);
};

struct BuildingSprite
{
	SDL_Texture* texture;
	int size;
};

struct BuildingSpritesData
{
	std::vector<Building_Type> buildingType;
	std::vector<BuildingSprite> data;

	const BuildingSprite* GetData(Building_Type) const;
};


class M_EntityManager : public j1Module
{

public:
	M_EntityManager(bool);
	~M_EntityManager();

	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void DoUnitLoop(float dt);
	void DoBuildingLoop(float dt);
	void UpdateSelectionRect();

	void ManageInput();

	void StartBuildingCreation(Building_Type);

	Unit* CreateUnit(int x, int y, Unit_Type);
	Building* CreateBuilding(int x, int y, Building_Type);
	bool deleteUnit(std::list<Unit*>::iterator);
	bool deleteBuilding(std::list<Building*>::iterator);

	bool IsEntitySelected(Entity*) const;

	void SendNewPath(int x, int y);

	SDL_Texture* GetTexture(Unit_Type);
	SDL_Texture* GetTexture(Building_Type);

	const UnitSprite* GetUnitSprite(Unit_Type) const;
	const BuildingSprite* GetBuildingSprite(Building_Type) const;

	void UpdateSpriteRect(Unit* unit, SDL_Rect& rect, SDL_RendererFlip& flip, float dt);
	void UpdateCurrentFrame(Unit* unit);
	//	bool addBuilding(Entity& _entity);
	//	bool deleteBuilding();

	void DrawDebug();
private:

	void SelectUnit(Unit*);
	void UnselectUnit(Unit*);

	void SelectBuilding(Building*);
	void UnselectBuilding(Building*);

	bool LoadUnitSpritesData();
	bool LoadBuildingSpritesData();

	void AddUnit(Unit* unit);
	void AddBuilding(Building* building);

	//should be priv
public:
	bool continuous = true;
	bool smooth = true;

	bool selectUnits = false;
	bool createBuilding = false;
	Building_Type buildingCreationType;
	C_Sprite buildingCreationSprite;
	C_Sprite buildingTile;

	iPoint logicTile;

	SDL_Rect selectionRect;
	SDL_Rect groupRect;
	SDL_Rect destinationRect;

	std::list<Unit*> unitList;
	std::list<Unit*> unitsToDelete;

	std::list<Unit*> selectedUnits;
	Building*		 selectedBuilding = NULL;

	std::list<Building*> buildingList;

	SDL_Texture* walkable_tile;
	SDL_Texture* nonwalkable_tile;

	SDL_Texture* unit_base;
	SDL_Texture* path_tex;
	SDL_Texture* hpBar_empty;
	SDL_Texture* hpBar_filled;
	SDL_Texture* hpBar_mid;
	SDL_Texture* hpBar_low;

	SDL_Texture* building_base;

	//Collision variables
	int currentPriority = 1;

private:

	UnitSpritesData unitSpritesData;
	BuildingSpritesData buildingSpritesData;

};

#endif //_ENTITYMANAGER_H__