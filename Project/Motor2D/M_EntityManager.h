#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "j1Module.h"
#include "Orders Factory.h"
#include "C_RenderObjects.h"
#include "Command.h"
#include "j1PerfTimer.h"

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

enum E_Race;

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

	int column_start;
	int column_end;
	int animation_speed;
};

struct UnitStatsData
{
	C_String name;
	E_Race race;
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
	SDL_Texture* corpse = NULL;
	SDL_Texture* birth = NULL;

	int size;
	float animationSpeed;

	int run_line_start;
	int run_line_end;

	int idle_line_start;
	int idle_line_end;

	int attack_line_start;
	int attack_line_end;

	int death_line;
	int death_column_start;
	int death_column_end;

	int deathNFrames = 0;
	float deathDuration = 0;
	iPoint deathSize;

	int birthNFrames = 0;
	float birthDuration = 0;
	iPoint birthSize = { 0, 0 };
	

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
	C_String name;
	E_Race race;
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

	int damage;
	int attackRange;
};

struct BuildingSpriteData
{
	SDL_Texture* texture;

	int size_x;
	int size_y;

	int anim_column_start;
	int anim_column_end;
	int animSpeed;

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
	std::vector<uint>				buildingQuantities;

	const BuildingStatsData*		GetStats(Building_Type) const;
	const BuildingSpriteData*		GetSprite(Building_Type) const;
	uint*							GetBuildingQuantity(Building_Type);
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

	bool Awake(pugi::xml_node&);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void UpdateMouseSprite(float dt);
	void SetMouseState(Mouse_State, bool);
	void UpdateMouseAnimation(float dt);

	void UpdateSelectionRect();

	void ManageInput();
	void FreezeInput();
	void UnfreezeInput();
	void UpdateCreationSprite();

	void UpdatePower(int startX, int startY, bool activate);
	void ChangePowerTile(int tileX, int tileY, bool activate);
	bool HasPower(int tileX, int tileY, Building_Type type);

	bool IsBuildingCreationWalkable(int x, int y, Building_Type type);
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
	Resource* FindRes(int x, int y);

	iPoint GetClosestCorner(Unit* unit, Building* building);
	iPoint GetClosestCorner(Unit* unit, Resource* resource);

	//External Factory methods ------------------------------------------------
	void StartUnitCreation(Unit_Type);
	bool CanBeCreated(Unit_Type);
	void PayUnitcosts(Unit_Type);
	Unit* CreateUnit(int x, int y, Unit_Type, Player_Type, Building* = NULL);
	void StartBuildingCreation(Building_Type);
	void FinishBuildingSpawn(Building*);
	Building* CreateBuilding(int x, int y, Building_Type, Player_Type, bool force = false);
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

	uint*				GetBuildingQuantity(Building_Type);
	void				RemoveBuildingCount(Building_Type);

	const HPBarData*	GetHPBarSprite(int type) const;

	void UpdateCurrentFrame(Unit* unit);
	//------------------------------------------------------------------------

	//Orders methods ---------------------------------------------------------
	void MoveSelectedUnits(int x, int y);
	void StopSelectedUnits();
//	void addOrder(Order& nOrder, UI_Button2* nButt = NULL);
	//------------------------------------------------------------------------
	void DrawDebug();

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


private:
	void UpdateFogOfWar();
	void DoUnitLoop(float dt);
	void DoBuildingLoop(float dt);
	void DoResourceLoop(float dt);

	//Internal Factory methods ---------------------
	Unit* AddUnit(Unit& unit);
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

	//Command functions-------------------
	void SpawnBuildings();
	//------------------------------------

	//should be priv
public:
	bool render = true;
	bool debug = false;
	bool shadows = true;
	bool stopLoop = false;

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
	Player_Type builidingCreationPlayer;
	C_Sprite buildingCreationSprite;
	C_Sprite buildingTile;
	C_Sprite buildingTileN;

	//Mouse position controllers
	bool		freezeInput;
	Unit*		hoveringUnit;
	Resource*	hoveringResource;
	Building*	hoveringBuilding;
	int			currentTile_x;
	int			currentTile_y;

	//Mouse textures variables
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

	bool selectionStarted = false;
	SDL_Rect selectionRect;
	SDL_Rect groupRect;
	SDL_Rect destinationRect;

	std::vector<Unit> unitList;
	std::list<Building*> buildingList;
	std::list<Resource*> resourceList;

	std::list<Unit*> unitsToDelete;
	std::list<Building*> buildingsToDelete;
	std::list<Resource*> resourcesToDelete;


	std::list<Unit*> selectedUnits;
	Unit*			 selectedEnemyUnit;
	Building*		 selectedBuilding = NULL;
	Resource*		 selectedResource = NULL;
	
	//Textures --------------------------------
	SDL_Texture* walkable_tile;
	SDL_Texture* nonwalkable_tile;

	SDL_Texture* gather_mineral_tex;
	SDL_Texture* gather_gas_tex;
	SDL_Texture* gather_mineral_shadow_tex;
	SDL_Texture* gather_gas_shadow_tex;

	SDL_Texture* probe_spark_tex;

	SDL_Texture* path_tex;

	C_Sprite highTemplarTrail;
	C_Sprite pylonArea;

	C_Animation building_spawn_animation;
	C_Animation fire1;
	C_Animation fire2;
	C_Animation fire3;
	C_Animation blood1;
	C_Animation blood2;
	C_Animation blood3;

	C_Animation protoss_rubble_s;
	C_Animation protoss_rubble_l;
	C_Animation zerg_rubble_s;
	C_Animation zerg_rubble_l;
	//----------------------------------------

	//Collision variables
	int currentPriority = 2;

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
	Gen_Scout o_Gen_Scout;
	Gen_Carrier o_Gen_Carrier;
	Gen_High_Templar o_Gen_High_Templar;
	Gen_Dark_Templar o_Gen_Dark_Templar;
	Gen_Shuttle o_Gen_Shuttle;
	Gen_Reaver o_Gen_Reaver;
	Gen_Observer o_Gen_Observer;
	Advanced_Builds o_Advanced_Builds;
	Build_Cybernetics o_Build_Cybernetics;
	Build_Photon_Cannon o_Build_Photon;
	Build_Stargate o_Build_Stargate;
	Build_Robotics_Facility o_Build_Robotics_Facility;
	Build_Robotics_Support_Bay o_Build_Robotics_Support_Bay;
	Build_Citadel_Adun o_Build_Citadel_Adun;
	Build_Templar_Archives o_Build_Templar_Archives;
	int unitCount;
private:

	j1PerfTimer performanceTimer;


	UnitsLibrary		unitsLibrary;
	BuildingsLibrary	buildingsLibrary;
	ResourcesLibrary	resourcesLibrary;
	std::vector<HPBarData> HPBars;

	int fogUnitIt;
	std::list<Building*>::iterator fogBuildingIt;
	bool unitsFogReady = false;
	bool buildingsFogReady = false;

	uint* powerTiles;

	struct C_SpawnBuildings : public Command
	{
		C_SpawnBuildings() : Command("spawn_buildings", "Spawn all constructing bulidings", 0, NULL, "Entity Manager "){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_SpawnBuildings c_SpawnBuildings;
};

#endif //_ENTITYMANAGER_H__