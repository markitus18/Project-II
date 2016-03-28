#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "j1Module.h"

class Entity;
class Unit;
class Building;
class Resource;

enum Unit_State;

enum Unit_Type;
enum Building_Type;
enum Resource_Type;

struct UnitStats
{
	int HP;
};

struct UnitSprite
{
	SDL_Texture* texture;
	SDL_Texture* shadow;
	SDL_Texture* base;

	int size;
	float animationSpeed;

	int run_line_start;
	int run_line_end;

	int idle_line_start;
	int idle_line_end;

	int attack_line_start;
	int attack_line_end;
};

struct UnitsLibrary
{
	std::vector<Unit_Type>	types;
	std::vector<UnitSprite> sprites;
	std::vector<UnitStats>	stats;

	const UnitStats*  GetStats(Unit_Type) const;
	const UnitSprite* GetSprite(Unit_Type) const;
	void GetStateLimits(Unit_Type type, Unit_State state, int& min, int& max);
};

struct BuildingStats
{
	int HP;
	int shield;
	int armor;

	int cost;
//	Resource_Type costType;

	int width_tiles;
	int height_tiles;

	int buildTime;
	int psi;
};

struct BuildingSprite
{
	SDL_Texture* texture;
	SDL_Texture* shadow;
	SDL_Texture* base;

	int size_x;
	int size_y;

	int offset_x;
	int offset_y;

	int shadow_size_x;
	int shadow_size_y;
	int shadow_offset_x;
	int shadow_offset_y;
};

struct BuildingsLibrary
{
	std::vector<Building_Type>	types;
	std::vector<BuildingStats>	stats;
	std::vector<BuildingSprite> sprites;

	const BuildingStats*		GetStats(Building_Type) const;
	const BuildingSprite*		GetSprite(Building_Type) const;
};

struct ResourceStats
{
	int maxAmount;

	int width_tiles;
	int height_tiles;
};

struct ResourceSprite
{
	SDL_Texture* texture;
	SDL_Texture* shadow;
	SDL_Texture* base;
	int size_x;
	int size_y;

	int offset_x;
	int offset_y;

	int shadow_size_x;
	int shadow_size_y;
	int shadow_offset_x;
	int shadow_offset_y;
};

struct ResourcesLibrary
{
	std::vector<Resource_Type>	types;
	std::vector<ResourceStats>	stats;
	std::vector<ResourceSprite> sprites;

	const ResourceStats*		GetStats(Resource_Type) const;
	const ResourceSprite*		GetSprite(Resource_Type) const;
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
	void DoResourceLoop(float dt);

	void UpdateSelectionRect();

	void ManageInput();

	Unit* CreateUnit(int x, int y, Unit_Type);

	void StartBuildingCreation(Building_Type);
	Building* CreateBuilding(int x, int y, Building_Type);
	void UpdateCreationSprite();
	bool IsBuildingCreationWalkable(int x, int y, Building_Type type) const;
	bool IsResourceCreationWalkable(int x, int y, Resource_Type type) const;

	Resource* CreateResource(int x, int y, Resource_Type type);

	bool deleteUnit(std::list<Unit*>::iterator);
	bool deleteBuilding(std::list<Building*>::iterator);

	bool IsEntitySelected(Entity*); //const;

	void SendNewPath(int x, int y);

	const UnitStats* GetUnitStats(Unit_Type) const;
	const UnitSprite* GetUnitSprite(Unit_Type) const;

	const BuildingStats* GetBuildingStats(Building_Type) const;
	const BuildingSprite* GetBuildingSprite(Building_Type) const;

	const ResourceStats* GetResourceStats(Resource_Type) const;
	const ResourceSprite* GetResourceSprite(Resource_Type) const;

	void UpdateSpriteRect(Unit* unit, SDL_Rect& rect, SDL_RendererFlip& flip, float dt);
	void UpdateCurrentFrame(Unit* unit);

	void DrawDebug();

private:

	void SelectUnit(Unit*);
	void UnselectUnit(Unit*);

	void SelectBuilding(Building*);
	void UnselectBuilding(Building*);

	void SelectResource(Resource*);
	void UnselectResource(Resource*);

	void AddUnit(Unit* unit);
	void AddBuilding(Building* building);
	void AddResource(Resource* resource);
	//Libraries load methods --------------
	bool LoadUnitsLibrary(char* stats, char* sprites);
	bool LoadBuildingsLibrary(char* stats, char* sprites);
	bool LoadResourcesLibrary(char* stats, char* sprites);
	bool LoadUnitsStats(char* path);
	bool LoadBuildingsStats(char* path);
	bool LoadResourcesStats(char* path);
	bool LoadUnitsSprites(char* path);
	bool LoadBuildingsSprites(char* path);
	bool LoadResourcesSprites(char* path);
	//------------------------------------


	//should be priv
public:
	bool render = true;
	bool debug = false;
	bool shadows = true;

	bool selectEntities = false;

	//Building creation variables
	bool createBuilding = false;
	bool buildingWalkable = false;
	Building_Type buildingCreationType;
	C_Sprite buildingCreationSprite;
	C_Sprite buildingTile;
	C_Sprite buildingTileN;

	iPoint logicTile;

	SDL_Rect selectionRect;
	SDL_Rect groupRect;
	SDL_Rect destinationRect;

	std::list<Unit*> unitList;
	std::list<Unit*> unitsToDelete;

	std::list<Unit*> selectedUnits;
	Building*		 selectedBuilding = NULL;

	std::list<Building*> buildingList;
	std::list<Resource*> resourceList;

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

	UnitsLibrary		unitsLibrary;
	BuildingsLibrary	buildingsLibrary;
	ResourcesLibrary	resourcesLibrary;

};

#endif //_ENTITYMANAGER_H__