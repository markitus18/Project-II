#include "M_PathFinding.h"

#include "j1App.h"
#include "M_Map.h"
#include "M_CollisionController.h"
#include "Unit.h"
#include "M_Input.h"

M_PathFinding::M_PathFinding(bool start_enabled) : j1Module(start_enabled)
{
	name.create("pathFinding");
}

//Destructor
M_PathFinding::~M_PathFinding()
{

}

// Called before render is available
bool M_PathFinding::Awake(pugi::xml_node& node)
{
	App->console->AddCommand(&c_Path_Corners);
	App->console->AddCommand(&c_Path_Diag);
	return true;
}

bool M_PathFinding::Start()
{
	startTile = endTile = iPoint{ -1, -1 };
	LoadMapData();

	return true;
}
// Called each loop iteration
bool M_PathFinding::Update(float dt)
{
	if (mapChanged)
	{
		App->collisionController->mapChanged = true;
		mapChanged = false;
	}
	return true;
}

// Called before quitting
bool M_PathFinding::CleanUp()
{
	delete[] mapData.data;
	return true;
}

bool M_PathFinding::GetNewPath(iPoint start, iPoint end, std::vector<iPoint>& pathOutput)
{
	startTile = start;
	endTile = end;
	endTileExists = startTileExists = true;
	FindPath();
	stepCount = 0;
	if (pathFound)
	{
		for (int i = path.Count() - 1; i >= 0; i--)
		{
			pathOutput.push_back(path[i]);
		}
		return true;
	}
	return false;
}

bool M_PathFinding::IsWalkable(int x, int y) const
{
	bool ret = mapData.isWalkable(x, y);
	return ret;
}

void M_PathFinding::FindPath()
{
	StartPathFinding();
	if (automaticIteration)
	{
		AutomaticPath();
	}
}

void M_PathFinding::AutomaticPath()
{
	while (!pathFinished && stepCount < 700)
	{
		StepUp();
		stepCount++;
	}
}

bool M_PathFinding::StepUp()
{
	bool ret = true;
	if (!pathFinished)
	{
		if (!openList.empty())
		{
			if (!newLowest)
			{
				lowestFNode = GetLowestF();
				lowestF = (*lowestFNode)->f;
			}
			newLowest = false;
			if (AddChilds(lowestFNode, endTile))
			{
				FinishPathFinding(path);
			}
		}
		else
		{
			if (pathStarted)
				LOG("-- Pathfinding: Could not find a path --");
			pathFinished = true;
			pathStarted = false;

		}
	}
	return ret;
}

bool M_PathFinding::LoadMapData()
{
	bool ret = true;
	LOG("-- Pathfinding: Loading meta data");
	std::list<MapLayer*>::iterator item;
	for (item = App->map->data.layers.begin(); item != App->map->data.layers.end(); item++)
	{
		if ((*item)->name == "Test")
		{
			mapData.height = (*item)->height;
			mapData.width = (*item)->width;
			mapData.data = new uint[mapData.height*mapData.width];
			for (int i = 0; i < mapData.width * mapData.height; i++)
			{
				int id = (*item)->data[i];
				TileSet* tileset = App->map->GetTilesetFromTileId(id);
				Tile* tile = tileset->GetTileFromId(id);
				if (tile)
				{
					if (tile->properties.GetProperty("Walkable") == 1)
						mapData.data[i] = 1;
					else
						mapData.data[i] = 0;
				}

			}
			ret = true;
		}
	}
	if (!ret)
		LOG("-- Pathfinding: Could not load meta tileset --");
	else
		LOG("-- Pathfinding: Meta tilesed loaded correctly --");
	return ret;

}

bool M_PathFinding::IfPathPossible()
{
	bool ret = false;
	if (startTileExists && endTileExists && !(startTile.x == endTile.x && startTile.y == endTile.y))
	{
		if (startTile.x >= 0 && startTile.x < App->map->data.width && startTile.y >= 0 && startTile.y < App->map->data.height)
			if (mapData.isWalkable(startTile.x, startTile.y) && mapData.isWalkable(endTile.x, endTile.y))
				ret = true;
	}
	return ret;
}

bool M_PathFinding::StartPathFinding()
{
	bool ret = false;
	pathFound = false;
	if (mapChanged)
	{
		LoadMapData();
		mapChanged = false;
	}

	if (IfPathPossible())
	{
		pathStarted = false;
		pathFinished = false;
		newLowest = false;
		lowestF = App->map->data.height * App->map->data.width;
		if (!openList.empty())
			openList.clear();
		if (!openList.empty())
			closedList.clear();
		if (path.Count() != 0)
			path.Clear();
		ret = CreateFirstNode();

		if (ret)
		{
			pathStarted = true;
			//LOG("-- Pathfinding: Correct pathfinding start --");
		}
	}
	else
		LOG("-- Pathfinding: Could not start the process, not possible path to be found");
	return ret;
}

bool M_PathFinding::CreateFirstNode()
{
	bool ret = false;
	if (startTileExists && endTileExists)
	{
		node* firstNode = new node;
		firstNode->parent = NULL;
		firstNode->tile.x = startTile.x;
		firstNode->tile.y = startTile.y;
		firstNode->g = 0;
		firstNode->h = (abs(endTile.x - firstNode->tile.x) + abs(endTile.y - firstNode->tile.y)) * 10;
		firstNode->f = firstNode->h;
		openList.push_back(firstNode);
		ret = true;
	}
	if (ret)
	{
		//LOG("-- Pathfinding: First node created --");
	}
	else
	{
		LOG("-- Pathfinding: Could not create first node --");
	}
	return ret;
}

std::list<node*>::const_iterator M_PathFinding::GetLowestF() const
{
	std::list<node*>::const_iterator it = openList.begin();
	std::list<node*>::const_iterator ret;
	int f = (App->map->data.height * App->map->data.width) * 10;
	while (it != openList.end())
	{
		if ((*it)->f <= f)
		{
			f = (*it)->f;
			ret = it;
		}
		it++;
	}

	return ret;
}

bool M_PathFinding::CreateSideNode(node* nParent, int x, int y, iPoint end, int amount, bool isDiagonal)
{
	bool ret = false;
	node* newNode = new node;
	newNode->parent = nParent;
	newNode->tile.x = x;
	newNode->tile.y = y;

	if (isDiagonal && !allowCorners)
	{
		if (!mapData.isWalkable(nParent->tile.x, newNode->tile.y) || !mapData.isWalkable(newNode->tile.x, nParent->tile.y))
		{
			delete newNode;
			return false;
		}
	}

	//Initial cost from the start
	newNode->g = nParent->g + amount;

	//Estimated cost to the end using Manhatan Distance
	newNode->h = (abs(end.x - x) + abs(end.y - y)) * 10;

	//Result of f + g
	newNode->f = newNode->g + newNode->h;

	//Compare if the current node already exists and it is not closed
	if (!IsNodeClosed(newNode))
	{
if (!CheckIfExists(newNode))
{
	if (newNode->f <= (*lowestFNode)->f)
	{
		newLowest = true;
		lowestF = newNode->f;
		std::list<node*>::const_iterator it = openList.begin();
		bool endLoop = false;
		while (it != openList.end() && !endLoop)
		{
			if (*(it) == newNode)
			{
				lowestFNode = it;
				endLoop = true;
			}
			it++;
		}
	}
	ret = CheckIfEnd(newNode, end);
}
else
delete newNode;
	}
	else
		delete newNode;

		return ret;
}

bool M_PathFinding::AddChild(node* nParent, int x, int y, iPoint end, int cost, bool isDiagonal)
{
	bool ret = false;
	if (x >= 0 && y >= 0)
	{
		if (mapData.isWalkable(x, y))
		{
			ret = CreateSideNode(nParent, x, y, endTile, cost, isDiagonal);
		}
	}
	return ret;
}

bool M_PathFinding::AddChilds(std::list<node*>::const_iterator nParent, iPoint end)
{
	if (AddChild((*nParent), (*nParent)->tile.x + 1, (*nParent)->tile.y, endTile, 10, false))
		return true;
	if (AddChild((*nParent), (*nParent)->tile.x, (*nParent)->tile.y + 1, endTile, 10, false))
		return true;
	if (AddChild((*nParent), (*nParent)->tile.x - 1, (*nParent)->tile.y, endTile, 10, false))
		return true;
	if (AddChild((*nParent), (*nParent)->tile.x, (*nParent)->tile.y - 1, endTile, 10, false))
		return true;
	if (allowDiagonals)
	{
		if (AddChild((*nParent), (*nParent)->tile.x + 1, (*nParent)->tile.y + 1, endTile, 14, true))
			return true;
		if (AddChild((*nParent), (*nParent)->tile.x + 1, (*nParent)->tile.y - 1, endTile, 14, true))
			return true;
		if (AddChild((*nParent), (*nParent)->tile.x - 1, (*nParent)->tile.y + 1, endTile, 14, true))
			return true;
		if (AddChild((*nParent), (*nParent)->tile.x - 1, (*nParent)->tile.y - 1, endTile, 14, true))
			return true;
	}

	//Move the parent to the closed list
	//openList.transfer(nParent, closedList);
	TransferItem(openList, closedList, nParent);

	return false;
}


//Check if the node is in the open list and leaves in the list the less cost value node
bool M_PathFinding::CheckIfExists(node* _node)
{
	bool nodeExists = false;
	int nodeIndex;
	std::list<node*>::iterator it = openList.begin();
	while (it != openList.end() && !nodeExists)
	{
		if ((*it)->tile.x == _node->tile.x && (*it)->tile.y == _node->tile.y)
		{
			nodeExists = true;
		}
		else
			it++;
	}

	if (nodeExists)
	{
		if (_node->f < (*it)->f)
		{
			openList.erase(it);
			openList.push_back(_node);
		}
		else
		{
			return true;
		}

	}
	else
		openList.push_back(_node);
	return false;
}

bool M_PathFinding::IsNodeClosed(node*  _node)
{
	bool ret = false;
	std::list<node*>::iterator it = closedList.begin();
	while (it != closedList.end())
	{
		if ((*it)->tile.x == _node->tile.x && (*it)->tile.y == _node->tile.y)
		{
			ret = true;
		}
		it++;
	}
	return ret;
}

bool M_PathFinding::CheckIfEnd(node* node, iPoint end)
{
	if (node->tile.x == endTile.x && node->tile.y == endTile.y)
	{
		goal = node;
		pathStarted = false;
		pathFinished = true;
		//LOG("-- Pathfinding: path finished --");
		return true;
	}

	return false;
}

void M_PathFinding::FinishPathFinding(C_DynArray<iPoint>& pathRef)
{
	node* _node;
	int i = 0;
	for (_node = goal; _node->parent; _node = _node->parent)
	{
		pathRef.PushBack(_node->tile);
		i++;
	}
	if (!openList.empty())
	{
		std::list<node*>::iterator it = openList.begin();
		while (it != openList.end())
		{
			RELEASE (*it);
			it++;
		}
		openList.clear();
	}

	if (!closedList.empty())
	{
		std::list<node*>::iterator it2 = closedList.begin();
		while (it2 != closedList.end())
		{
			RELEASE(*it2);
			it2++;
		}
		closedList.clear();
	}


	pathFound = true;
}

void M_PathFinding::TransferItem(std::list<node*> src, std::list<node*> dst, std::list<node*>::const_iterator it)
{
	dst.push_back((*it));
	src.erase(it);
}

bool map::isWalkable(int x, int y) const
{
	if (x < App->map->data.height && x >= 0 && y < App->map->data.height && y >= 0)
	{
		return data[y*width + x];
	}
	return false;
}

#pragma region Commands
void M_PathFinding::C_Path_Corners::function(const C_DynArray<C_String>* arg)
{
	if (arg->Count() > 1)
	{
		C_String str = arg->At(1)->GetString();
		if (str == "enable")
		{
			App->pathFinding->allowCorners = true;
			LOG("-- Pathfinding: Corners enabled --");
		}
		else if (str == "disable")
		{
			App->pathFinding->allowCorners = false;
			LOG("-- Pathfinding: Corners disabled --");
		}
		else
			LOG("pathfinding_corner: unexpected command '%s', expecting enable / disable", arg->At(1)->GetString());
	}
	else
		LOG("'%s': not enough arguments, expecting enable / disable", arg->At(0)->GetString());
}

void M_PathFinding::C_Path_Diag::function(const C_DynArray<C_String>* arg)
{
	if (arg->Count() > 1)
	{
		C_String str = arg->At(1)->GetString();
		if (str == "enable")
		{
			App->pathFinding->allowDiagonals = true;
			LOG("-- Pathfinding: Diagonals enabled --");
		}
		else if (str == "disable")
		{
			App->pathFinding->allowDiagonals = false;
			LOG("-- Pathfinding: Diagonals disabled --");
		}
		else
			LOG("pathfinding_diag: unexpected command '%s', expecting enable / disable", arg->At(1)->GetString());
	}
	else
		LOG("'%s': not enough arguments, expecting enable / disable", arg->At(0)->GetString());

}
#pragma endregion