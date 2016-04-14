#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "j1Module.h"

#include "Orders Factory.h"
class Entity;
class Unit;
class Building;
class Resource;

enum Unit_Movement_State;
enum Unit_State;
enum Attack_State;

enum Unit_Movement_Type;
enum Unit_Type;

enum Building_Type;
enum Resource_Type;

enum Player_Type;

struct HPBarData
{
	SDL_Texture* empty;
	SDL_Texture* fill;
	SDL_Texture* shield;

	int size_x;
	int size_y = 7;
};

struct BaseSpriteData
{
	SDL_Texture* texture;
	int size_x;
	int size_y;

	int offset_x;
	int offset_y;
};

struct ShadowSpriteData
{
	SDL_Texture* texture;
	int size_x;
	int size_y;

	int offset_x;
	int offset_y;
};

struct UnitStatsData
{
	int type;  // Size
	bool invisible;
	Unit_Movement_Type movementType;

	int HP;
	int shield;
	int energy;
	int armor;
	int psi;

	int mineralCost;
	int gasCost;
	/*Combat stats goes here*/
	float cooldown;
	
	float speed;
	int visionRange;
	int detectionRange;
	int attackRange;

	bool canAttackFlying;

	int buildTime;
	int damage;
};

struct UnitSpriteData
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

	int					HPBar_type;

	ShadowSpriteData	shadow;
	BaseSpriteData		base;
};

struct UnitsLibrary
{
	std::vector<Unit_Type>	types;
	std::vector<UnitSpriteData> sprites;
	std::vector<UnitStatsData>	stats;

	const UnitStatsData*  GetStats(Unit_Type) const;
	const UnitSpriteData* GetSprite(Unit_Type) const;
	void GetStateLimits(Unit_Type type, Unit_Movement_State state, int& min, int& max);
};

struct BuildingStatsData
{
	int HP;
	int shield;
	int armor;

	int mineralCost;
	int gasCost;

	int width_tiles;
	int height_tiles;

	int visionRange;
	int buildTime;
	int psi;
};

struct BuildingSpriteData
{
	SDL_Texture* texture;

	int size_x;
	int size_y;

	int offset_x;
	int offset_y;

	int shadow_size_x;
	int shadow_size_y;
	int shadow_offset_x;
	int shadow_offset_y;

	int HPBar_type;

	ShadowSpriteData	shadow;
	BaseSpriteData		base;
};

struct BuildingsLibrary
{
	std::vector<Building_Type>		types;
	std::vector<BuildingStatsData>	stats;
	std::vector<BuildingSpriteData> sprites;

	const BuildingStatsData*		GetStats(Building_Type) const;
	const BuildingSpriteData*		GetSprite(Building_Type) const;
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

	int size_x;
	int size_y;

	int offset_x;
	int offset_y;

	int shadow_size_x;
	int shadow_size_y;
	int shadow_offset_x;
	int shadow_offset_y;

	ShadowSpriteData	shadow;
	BaseSpriteData		base;
};

struct ResourcesLibrary
{
	std::vector<Resource_Type>	types;
	std::vector<ResourceStats>	stats;
	std::vector<ResourceSprite> sprites;

	const ResourceStats*		GetStats(Resource_Type) const;
	const ResourceSprite*		GetSprite(Resource_Type) const;
};

enum Player_Type
{
	PLAYER,
	COMPUTER,
};

enum Mouse_State
{
	M_DEFAULT,
	M_SELECTION,
	M_ALLY_HOVER,
	M_RESOURCE_HOVER,
	M_ENEMY_HOVER,
	M_UP,
	M_UP_RIGHT,
	M_RIGHT,
	M_RIGHT_DOWN,
	M_DOWN,
	M_DOWN_LEFT,
	M_LEFT,
	M_LEFT_UP,
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

	void UpdateMouseSprite(float dt);
	void SetMouseState(Mouse_State, bool);
	void UpdateMouseAnimation(float dt);

	void UpdateSelectionRect();

	void ManageInput();

	void UpdateCreationSprite();

	bool IsBuildingCreationWalkable(int x, int y, Building_Type type) const;
	bool IsResourceCreationWalkable(int x, int y, Resource_Type type) const;
	bool IsEntitySelected(Entity*) const;

	void SetUnitHover(Unit*);
	void SetBuildingHover(Building*);
	void SetResourceHover(Resource*);

	void SendNewPath(int x, int y, Attack_State state);
	void SendToGather(Resource* resource);
	void SendToGather(Building* building);
	void SendToAttack(Unit* unit);
	void SendToAttack(Building* unit);
	void SendToAttack(int x, int y);

	Building* FindClosestNexus(Unit* unit);
	Resource* FindClosestResource(Unit* unit);
	iPoint GetClosestCorner(Unit* unit, Building* building);
	iPoint GetClosestCorner(Unit* unit, Resource* resource);

	//External Factory methods ------------------------------------------------
	void StartUnitCreation(Unit_Type);
	Unit* CreateUnit(int x, int y, Unit_Type, Player_Type, Building* = NULL);
	void StartBuildingCreation(Building_Type);
	Building* CreateBuilding(int x, int y, Building_Type, Player_Type);
	Resource* CreateResource(int x, int y, Resource_Type type);

	bool deleteUnit(std::list<Unit*>::iterator);
	bool deleteBuilding(std::list<Building*>::iterator);
	bool deleteResource(std::list<Resource*>::iterator);
	//-------------------------------------------------------------------------

	//Library data load -------------------------------------------------------
	const UnitStatsData* GetUnitStats(Unit_Type) const;
	const UnitSpriteData* GetUnitSprite(Unit_Type) const;

	const BuildingStatsData* GetBuildingStats(Building_Type) const;
	const BuildingSpriteData* GetBuildingSprite(Building_Type) const;

	const ResourceStats* GetResourceStats(Resource_Type) const;
	const ResourceSprite* GetResourceSprite(Resource_Type) const;

	const HPBarData*	GetHPBarSprite(int type) const;

	void UpdateSpriteRect(Unit* unit, C_Sprite& sprite, float dt);
	void UpdateCurrentFrame(Unit* unit);
	//------------------------------------------------------------------------

	//Orders methods ---------------------------------------------------------
	void MoveSelectedUnits();
	void StopSelectedUnits();
//	void addOrder(Order& nOrder, UI_Button2* nButt = NULL);
	//------------------------------------------------------------------------
	void DrawDebug();

private:

	void DoUnitLoop(float dt);
	void DoBuildingLoop(float dt);
	void DoResourceLoop(float dt);

	//Selection methods -------------------
	void SelectUnit(Unit*);
	void UnselectUnit(Unit*);

	void SelectBuilding(Building*);
	void UnselectBuilding(Building*);

	void SelectResource(Resource*);
	void UnselectResource(Resource*);

	void UnselectAllUnits();
	void DoSingleSelection();
	//-------------------------------------

	//Internal Factory methods ---------------------
	void AddUnit(Unit* unit);
	void AddBuilding(Building* building);
	void AddResource(Resource* resource);
	//-------------------------------------

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
	bool LoadHPBars();
	//------------------------------------


	//should be priv
public:
	bool render = true;
	bool debug = false;
	bool shadows = true;

	//Orders bools ------------
	bool executedOrder = false;
	bool moveUnits = false;
	bool selectEntities = false;
	bool attackUnits = false;
	bool startSelection = false;
	bool setWaypoint = false;
	//-------------------------

	//Building creation variables
	bool createBuilding = false;
	bool buildingWalkable = false;
	Building_Type buildingCreationType;
	C_Sprite buildingCreationSprite;
	C_Sprite buildingTile;
	C_Sprite buildingTileN;

	//Mouse position controllers
	Unit*		hoveringUnit;
	Resource*	hoveringResource;
	Building*	hoveringBuilding;

	std::vector< SDL_Texture*>	mouseTextures;
	std::vector<int>			mouseTexturesNumber;
	int							mouseMinRect = 0;
	int							mouseMaxRect = 4;
	float						mouseAnimationSpeed = 7.0f;
	float						mouseRect = 0;
	C_Sprite mouseSprite;
	Mouse_State mouseState;
	//--------------------------

	iPoint logicTile;

	SDL_Rect selectionRect;
	SDL_Rect groupRect;
	SDL_Rect destinationRect;

	std::list<Unit*> unitList;
	std::list<Building*> buildingList;
	std::list<Resource*> resourceList;

	std::list<Unit*> unitsToDelete;
	std::list<Building*> buildingsToDelete;
	std::list<Resource*> resourcesToDelete;


	std::list<Unit*> selectedUnits;
	Unit*			 selectedEnemyUnit;
	Building*		 selectedBuilding = NULL;
	Resource*		 selectedResource = NULL;

	SDL_Texture* walkable_tile;
	SDL_Texture* nonwalkable_tile;

	SDL_Texture* path_tex;



	//Collision variables
	int currentPriority = 1;

	//Orders

	//std::list<Order*> orders;

	Set_RallyPoint o_Set_rallyPoint;
	Move o_Move;
	Attack o_Attack;
	Gather o_Gather;
	Patrol o_Patrol;
	Hold_Pos o_Hold_pos;
	Stop o_Stop;
	Ret_Cargo o_Ret_Cargo;
	Gen_Probe o_GenProbe_toss;
	Gen_Dragoon o_Gen_Dragoon;
	Gen_Zealot o_Gen_Zealot;
	Build_Gateaway o_Build_Gateaway;
	Build_Nexus o_Build_Nexus;
	Build_Pylon o_Build_Pylon;
	Build_Assimilator o_Build_Assimilator;
	Basic_Builds o_Basic_Builds;
	Return_Builds_Menu o_Return_Builds_Menu;
	Cancel_Current_Action o_Cancel_Current_Action;

private:

	UnitsLibrary		unitsLibrary;
	BuildingsLibrary	buildingsLibrary;
	ResourcesLibrary	resourcesLibrary;
	std::vector<HPBarData> HPBars;
};

#endif //_ENTITYMANAGER_H__