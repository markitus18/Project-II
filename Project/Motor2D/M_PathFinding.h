#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"
#include "M_Console.h"
#include <queue>
#include <stack>

#define NODES_PER_FRAME 20
#define MAX_NODES 1500
#define MAX_FRAMES 60
#define RANGE_TO_IGNORE_WAYPOINTS 25

struct MapLayer;
class Unit;

struct node
{
	node* parent;
	iPoint tile;
	int f, g, h;
};

struct queuedPath
{
	queuedPath(iPoint start, iPoint end, std::vector<iPoint>* _output) { from = start; to = end; output = _output; }
	iPoint from;
	iPoint to;
	std::vector<iPoint>* output;
};

struct logicTile
{
	logicTile(bool w, uint s) : walkable(w), sector(s) {}
	bool walkable;
	uint sector;
};

struct waypoint
{
	waypoint(int x, int y, int sectorConnected) : tile(x, y), connectsWithSector(sectorConnected) {}
	iPoint	tile;
	int		connectsWithSector;
};

struct sector
{
	sector(int sector) : sectorN(sector) {}
	int sectorN;
	std::vector<waypoint> waypoints;
	void AddWaypoint(int x, int y, int sectorConnected)
	{
		waypoints.push_back(waypoint(x, y, sectorConnected));
	}
};

struct sectorIterator
{
	sectorIterator(sector* _sector, sectorIterator* _parent = NULL) : pointer(_sector), parent(_parent)
	{
		if (parent) { cost = parent->cost + 1; }
	}
	sector* pointer;
	sectorIterator* parent;
	uint cost;
};

class M_PathFinding : public j1Module
{

public:
	M_PathFinding(bool);

	//Destructor
	~M_PathFinding();

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	void GetNewPath(iPoint start, iPoint end, std::vector<iPoint>* output);
	bool IsWalkable(int x, int y) const;
	bool ValidSector(int x, int y) const;

	void LoadWalkableMap(char* path);
	void Draw();

	iPoint MapToWorld(int, int) const;
	iPoint WorldToMap(int, int) const;

	void ChangeWalkability(int x, int y, bool walkable);

private:
	//Path finder methods
	std::list<node*>::iterator  GetLowestF();

	void AutomaticPath();
	bool StepUp();

	void AssignNewPath();
	void FindPath();

	void AsignSectors();
	bool IfPathPossible();
	bool StartPathFinding();
	bool CreateFirstNode();

	//Adding new nodes methods
	bool CreateSideNode(node* nParent, int x, int y, iPoint end, int cost, bool isDiagonal);
	bool AddChild(node* nParent, int x, int y, iPoint end, int cost, bool isDiagonal);
	bool AddChilds(std::list<node*>::iterator nParent, iPoint end);

	bool IsNodeClosed(node* node);
	bool CheckIfExists(node* node);
	bool CheckIfEnd(node* node, iPoint end);
	void FinishPathFinding();
	void CheckMaxLength();

	void TransferItem(std::list<node*>::iterator it);
	void ClearLists();

private:

	//Paths queue
	std::queue<queuedPath> queue;
	bool working = false;
	std::vector<iPoint>* output;
	std::vector<iPoint> tmpOutput;
	uint nFrames = 0;

	//Path finder variables
	int				lowestF;
	bool			newLowest = false;
	node*			goal;
	node*			lastParent;
	std::list<node*>::iterator lowestFNode;

	//tmp variables
	int nodesCreated = 0;
	int nodesDestroyed = 0;
	int transfCount = 0;

	//-----

	int stepCount = 0;

public:

	//Path finder variables
	std::list<node*>	openList;
	std::list<node*>	closedList;
	std::vector<int>	allowedSectors;
	bool usingSectors = true;

	std::vector<sector> sectors;

	std::list<node>	debugList;

	bool displayPath = false;

	bool		atLeastOneWaypoint = false;
	bool		startTileExists = false;
	bool		endTileExists = false;
	iPoint		startTile;
	iPoint		globalStart;
	std::stack<iPoint>		endTile;
	bool		allowDiagonals = true;
	bool		allowCorners = false;
	bool		mapChanged = false;
	bool		automaticIteration = true;
	bool		pathStarted = false;
	bool		pathFinished = false;
	bool		pathFound = false;

	//Map collision variables
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	std::vector<logicTile>	tilesData;

#pragma region Commands
	struct C_Path_Corners: public Command
	{
		C_Path_Corners() : Command("pathfinding_corner", "Enable / Disable pathfinding corners", 1, "pf_c", "Pathfinding"){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_Path_Corners c_Path_Corners;

	struct C_Path_Diag : public Command
	{
		C_Path_Diag() : Command("pathfinding_diag", "Enable / Disable pathfinding diagonals", 1, "pf_d", "Pathfinding"){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_Path_Diag c_Path_Diag;

#pragma endregion
};
#endif // __j1PATHFINDING_H__