#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"
#include "M_Console.h"

struct MapLayer;
class Unit;

class M_PathFinding : public j1Module
{
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

	bool GetNewPath(iPoint start, iPoint end, C_DynArray<iPoint>& pathOutput);
	bool IsWalkable(int x, int y) const;

private:
	C_List_item<node*>*  GetLowestF() const;

	void AutomaticPath();
	bool StepUp();

	void FindPath();
	bool LoadMapData();
	bool IfPathPossible();
	bool StartPathFinding();
	bool CreateFirstNode();
	//Adding new nodes methods
	bool CreateSideNode(node* nParent, int x, int y, iPoint end, int cost, bool isDiagonal);
	bool AddChild(node* nParent, int x, int y, iPoint end, int cost, bool isDiagonal);
	bool AddChilds(C_List_item<node*>* nParent, iPoint end);

	bool IsNodeClosed(node* node);
	bool CheckIfExists(node* node);
	bool CheckIfEnd(node* node, iPoint end);
	void FinishPathFinding(C_DynArray<iPoint>& pathRef);

private:

	int				lowestF;
	bool			newLowest = false;
	node*			goal;
	node*			lastParent;
	C_List_item<node*>* lowestFNode;


public:
	C_List<node*>	openList;
	C_List<node*>	closedList;

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

	bool		wallUp = false;

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