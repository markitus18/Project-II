#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"
#include "M_Console.h"

struct MapLayer;
class Unit;

struct node
{
	node* parent;
	iPoint tile;
	int f, g, h;
};

struct map
{
	int width;
	int height;
	uint* data;

	bool isWalkable(int x, int y) const;
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

	bool GetNewPath(iPoint start, iPoint end, std::vector<iPoint>& pathOutput);
	bool IsWalkable(int x, int y) const;

	bool LoadMapData();
private:
	std::list<node*>::iterator  GetLowestF();

	void AutomaticPath();
	bool StepUp();

	void FindPath();

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
	void FinishPathFinding(C_DynArray<iPoint>& pathRef);

	void TransferItem(std::list<node*> src, std::list<node*> dst, std::list<node*>::iterator it);
	void ClearLists();
private:

	int				lowestF;
	bool			newLowest = false;
	node*			goal;
	node*			lastParent;
	std::list<node*>::iterator lowestFNode;

	int stepCount = 0;

public:
	std::list<node*>	openList;
	std::list<node*>	closedList;

	bool		startTileExists = false;
	bool		endTileExists = false;
	iPoint		startTile;
	iPoint		endTile;
	bool		allowDiagonals = true;
	bool		allowCorners = false;
	bool		mapChanged = false;
	bool		automaticIteration = true;
	bool		pathStarted = false;
	bool		pathFinished = false;
	bool		pathFound = false;
	C_DynArray<iPoint> path;
	map					mapData;

	Unit*		currentUnit = NULL;


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