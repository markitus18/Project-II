#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"
#include "j1Console.h"

struct MapLayer;

struct PathNode
{
	iPoint point;
	bool converted;

	PathNode(){};
	PathNode(iPoint newPoint){ point = newPoint; converted = false; };
	PathNode(iPoint newPoint, bool conv){ point = newPoint; converted = conv; };
};

class j1PathFinding : public j1Module
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
	j1PathFinding(bool);

	//Destructor
	virtual ~j1PathFinding();

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	bool GetNewPath(iPoint start, iPoint end, p2DynArray<PathNode>& pathOutput);

private:
	p2List_item<node*>*  GetLowestF() const;

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
	bool AddChilds(p2List_item<node*>* nParent, iPoint end);

	bool IsNodeClosed(node* node);
	bool CheckIfExists(node* node);
	bool CheckIfEnd(node* node, iPoint end);
	void FinishPathFinding(p2DynArray<PathNode>& pathRef);

private:

	int				lowestF;
	bool			newLowest = false;
	node*			goal;
	node*			lastParent;
	p2List_item<node*>* lowestFNode;
	MapLayer*		layer;

public:
	p2List<node*>	openList;
	p2List<node*>	closedList;
	map*		mapData;
	bool		startTileExists = false;
	bool		endTileExists = false;
	iPoint		startTile;
	iPoint		endTile;
	bool		allowDiagonals = false;
	bool		allowCorners = false;
	bool		mapChanged = false;
	bool		automaticIteration = true;
	bool		pathStarted = false;
	bool		pathFinished = false;
	bool		pathFound = false;
	p2DynArray<PathNode> path;

#pragma region Commands
	struct C_Path_Corners: public Command
	{
		C_Path_Corners() : Command("pathfinding_corner", "Enable / Disable pathfinding corners", 1, "pf_c", "Pathfinding"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	C_Path_Corners c_Path_Corners;

	struct C_Path_Diag : public Command
	{
		C_Path_Diag() : Command("pathfinding_diag", "Enable / Disable pathfinding diagonals", 1, "pf_d", "Pathfinding"){}
		void function(const p2DynArray<p2SString>* arg);
	};
	C_Path_Diag c_Path_Diag;

#pragma endregion
};
#endif // __j1PATHFINDING_H__