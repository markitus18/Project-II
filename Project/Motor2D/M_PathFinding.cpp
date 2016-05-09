#include "M_PathFinding.h"

#include "j1App.h"
#include "M_CollisionController.h"
#include "M_Console.h"

#include "M_InputManager.h"
#include "M_FileSystem.h"
#include "M_Render.h"

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
	startTile = iPoint{ -1, -1 };
	return true;
}
// Called each loop iteration
bool M_PathFinding::Update(float dt)
{
	if (displayPath)
	{
		std::list<node>::iterator it = debugList.begin();
		while (it != debugList.end())
		{
			iPoint start = MapToWorld((*it).tile.x, (*it).tile.y);
			App->render->AddRect({ start.x, start.y, 10, 10 }, true, 200, 0, 0, 100);
			it++;
		}
	}

	if (mapChanged)
	{
		App->collisionController->mapChanged = true;
		mapChanged = false;
	}

	if (working == false)
	{
		AssignNewPath();
	}
	else
	{
		FindPath();
		nFrames++;
		if (pathFound)
		{
			if (displayPath)
			{
				LOG("Took %i frames, Nodes Created: %i , Nodes Destroyed: %i, Nodes transfered: %i", nFrames, nodesCreated, nodesDestroyed, transfCount);
			}
			for (int i = tmpOutput.size() - 1; i >= 0; i--)
			{
				output->push_back(tmpOutput[i]);
			}
			working = false;
			stepCount = 1;
			nFrames = 0;
			ClearLists();
			while (!endTile.empty())
			{
				endTile.pop();
			}
			tmpOutput.clear();
		}

		//If the path is nos easily accesible through direct pathfinding, use waypoints
		if (usingSectors == false && stepCount >= NODES_PER_FRAME * 5)
		{
			usingSectors = true;
			StartPathFinding();
		}

		CheckMaxLength();
	}
	return true;
}

// Called before quitting
bool M_PathFinding::CleanUp()
{
	std::list<node*>::iterator item = openList.begin();
	while (item != openList.end())
	{
		RELEASE(*item);
		item++;
	}
	item = closedList.begin();
	while (item != closedList.end())
	{
		RELEASE(*item);
		item++;
	}
	tilesData.clear();

	while (queueHigh.empty() == false)
	{
		queueHigh.erase(queueHigh.begin());
	}

	while (queue.empty() == false)
	{
		queue.erase(queue.begin());
	}

	while (queueLow.empty() == false)
	{
		queueLow.erase(queueLow.begin());
	}
	working = false;

	return true;
}

int M_PathFinding::GetNewPath(iPoint start, iPoint end, std::vector<iPoint>* output, e_priority priority)
{
	pathIndex++;
	if (pathIndex > 500)
		pathIndex = 0;

	switch (priority)
	{
	case PRIORITY_HIGH:
	{
		queueHigh.push_back(queuedPath(start, end, output, pathIndex)); break;
	}
	case PRIORITY_LOW:
	{
		queueLow.push_back(queuedPath(start, end, output, pathIndex)); break;
	}
	default:
	{
		queue.push_back(queuedPath(start, end, output, pathIndex)); break;
	}
	}
	debugList.clear();

	return pathIndex;
}

bool M_PathFinding::IsWalkable(int x, int y) const
{
	if (x < width && x >= 0 && y < height && y >= 0)
	{
		return tilesData[y*width + x].walkable;
	}
	return false;
}

bool M_PathFinding::ValidSector(int x, int y) const
{
	bool ret = false;
	if (x < width && x >= 0 && y < height && y >= 0)
	{
		std::vector<int>::const_iterator it = allowedSectors.begin();
		if (tilesData[y*width + x].walkable)
		{
			while (it != allowedSectors.end() && ret == false)
			{
				if (tilesData[y*width + x].sector == (*it))
				{
					ret = true;
				}
				it++;
			}
		}
	}
	return ret;
}

void M_PathFinding::RemovePath(int index)
{
	bool found = false;

	if (currentPathIndex == index)
	{
		found = true;
		StopCurrent();
	}

	if (!found && !queueHigh.empty())
	{
		std::list<queuedPath>::iterator it = queueHigh.begin();
		while (it != queueHigh.end() && !found)
		{
			if (it->index == index)
			{
				found = true;
			}
		}
		if (found)
		{
			queueHigh.erase(it);
		}
	}

	if (!found && !queue.empty())
	{
		std::list<queuedPath>::iterator it2 = queue.begin();
		while (it2 != queue.end() && !found)
		{
			if (it2->index == index)
			{
				found = true;
			}
		}
		if (found)
		{
			queue.erase(it2);
		}
	}

	if (!found && !queueLow.empty())
	{
		std::list<queuedPath>::iterator it3 = queueLow.begin();
		while (it3 != queueLow.end() && !found)
		{
			if (it3->index == index)
			{
				found = true;
			}
		}
		if (found)
		{
			queueLow.erase(it3);
		}
	}	
}

void M_PathFinding::StopCurrent()
{
	working = false;
	atLeastOneWaypoint = false;
	ClearLists();
	pathStarted = false;
	pathFinished = true;
}

void M_PathFinding::AssignNewPath()
{
	queuedPath toWorkWith;
	if (!queueHigh.empty())
	{
		toWorkWith = queueHigh.front();
		queueHigh.erase(queueHigh.begin());
	}
	else if (!queue.empty())
	{
		toWorkWith = queue.front();
		queue.erase(queue.begin());
	}
	else if (!queueLow.empty())
	{
		toWorkWith = queueLow.front();
		queueLow.erase(queueLow.begin());
	}
	else
	{
		return;
	}


	startTile = toWorkWith.from;
	globalStart = startTile;
	endTile.push(toWorkWith.to);
	output = toWorkWith.output;
	currentPathIndex = toWorkWith.index;
	endTileExists = startTileExists = true;
	nodesCreated = nodesDestroyed = transfCount = 0;

	if (startTile.DistanceManhattan(endTile.top()) < RANGE_TO_IGNORE_WAYPOINTS)
	{
		usingSectors = false;
	}
	else
	{
		usingSectors = true;
	}

	StartPathFinding();
	working = true;
}

void M_PathFinding::FindPath()
{
	if (automaticIteration)
	{
		AutomaticPath();
	}	
}

void M_PathFinding::AutomaticPath()
{
	while (!pathFinished && stepCount < MAX_NODES && stepCount % NODES_PER_FRAME != 0)
	{
		StepUp();
		stepCount++;
	}
	stepCount++;
	if (stepCount >= MAX_NODES)
		LOG("Pathfinding: Time out");
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
			if (AddChilds(lowestFNode, endTile.top()))
			{
				FinishPathFinding();
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

void M_PathFinding::LoadWalkableMap(char* path)
{
	bool ret = true;
	C_String tmp(path);

	char* buf;
	int size = App->fs->Load(tmp.GetString(), &buf);
	pugi::xml_document file;
	pugi::xml_parse_result result = file.load_buffer(buf, size);

	RELEASE_ARRAY(buf);

	if (result == NULL)
	{
		LOG("Could not load collision map file %s. pugi error: %s", path, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		//Map data
		width = file.child("map").attribute("width").as_int();
		height = file.child("map").attribute("height").as_int();
		tile_width = file.child("map").attribute("tilewidth").as_int();
		tile_height = file.child("map").attribute("tileheight").as_int();

		//Finding walkability data tileset
		bool tilesetFound = false;
		pugi::xml_node tileset = file.child("map").child("tileset");
		while (!tilesetFound)
		{
			if (C_String(tileset.attribute("name").as_string()) == "Walkable")
			{
				tilesetFound = true;
			}
			else
			{
				tileset = tileset.next_sibling("tileset");
			}
		}
		//Loading tile ID's data
		if (tilesetFound)
		{
			std::vector<logicTile> tileIDs;
			pugi::xml_node tileData;

			for (tileData = tileset.child("tile"); tileData; tileData = tileData.next_sibling("tile"))
			{
				//Run through all properties from tile and adding "Walkable" property
				pugi::xml_node property;
				bool walkable = false;
				int sector = 0;
				for (property = tileData.child("properties").child("property"); property; property = property.next_sibling("property"))
				{
					if (C_String(property.attribute("name").as_string()) == "Sector")
					{
						sector = property.attribute("value").as_int();
					}
					if (C_String(property.attribute("name").as_string()) == "Walkable")
					{
						walkable = property.attribute("value").as_bool();
					}
				}
				tileIDs.push_back(logicTile(walkable, sector));
			}

			//Finding collision layer
			pugi::xml_node layer = file.child("map").child("layer");
			bool layerFound = false;
			while (!layerFound)
			{
				//COLLISION LAYER NAME
				if (C_String(layer.attribute("name").as_string()) == "Walkable")
				{
					layerFound = true;
				}
				else
				{
					layer = layer.next_sibling("layer");
				}
			}

			//Loading collision tile data
			if (layerFound)
			{
				pugi::xml_node tile = layer.child("data").child("tile");

				for (tile = layer.child("data").child("tile"); tile; tile = tile.next_sibling("tile"))
				{
					tilesData.push_back(tileIDs[tile.attribute("gid").as_int() - 1]);
				}
			}
			else
			{
				LOG("Could not find collision layer");
			}
			for (int n = 0; n < tileIDs.size(); n++)
			{
				sectors.push_back(sector(n));
			}

#pragma region setting_waypoints

			sectors[1].AddWaypoint(12, 152, 2);
			sectors[1].AddWaypoint(57, 150, 2);
			sectors[1].AddWaypoint(80, 150, 2);
			sectors[1].AddWaypoint(105, 154, 7);
			sectors[1].AddWaypoint(120, 163, 3);
			sectors[1].AddWaypoint(120, 181, 3);

			sectors[2].AddWaypoint(14, 66, 4);
			sectors[2].AddWaypoint(11, 151, 1);
			sectors[2].AddWaypoint(56, 149, 1);
			sectors[2].AddWaypoint(87, 149, 1);

			sectors[3].AddWaypoint(152, 127, 13);
			sectors[3].AddWaypoint(137, 123, 5);
			sectors[3].AddWaypoint(147, 110, 6);
			sectors[3].AddWaypoint(121, 163, 1);
			sectors[3].AddWaypoint(121, 181, 1);

			sectors[4].AddWaypoint(14, 65, 2);
			sectors[4].AddWaypoint(14, 30, 9);
			sectors[4].AddWaypoint(46, 25, 9);
			sectors[4].AddWaypoint(131, 14, 10);
			sectors[4].AddWaypoint(75, 52, 5);

			sectors[5].AddWaypoint(74, 53, 4);
			sectors[5].AddWaypoint(136, 123, 3);

			sectors[6].AddWaypoint(148, 111, 3);
			sectors[6].AddWaypoint(149, 65, 11);
			sectors[6].AddWaypoint(166, 53, 11);

			sectors[7].AddWaypoint(106, 154, 1);

			sectors[8].AddWaypoint(174, 120, 13);

			sectors[9].AddWaypoint(14, 29, 4);
			sectors[9].AddWaypoint(45, 25, 4);

			sectors[10].AddWaypoint(131, 13, 4);
			sectors[10].AddWaypoint(152, 17, 11);

			sectors[11].AddWaypoint(134, 61, 12);
			sectors[11].AddWaypoint(148, 65, 6);
			sectors[11].AddWaypoint(165, 53, 6);
			sectors[11].AddWaypoint(152, 18, 10);

			sectors[12].AddWaypoint(134, 62, 11);

			sectors[13].AddWaypoint(153, 127, 3);
			sectors[13].AddWaypoint(173, 120, 8);

			int tmp[13][13] =
			{
				//		 1  2  3  4  5  6  7  8  9  10 11 12 13
				/*  1*/{ 0, 1, 1, 2, 2, 2, 1, 3, 3, 3, 3, 4, 2},
				/*  2*/{ 1, 0, 2, 1, 2, 3, 2, 4, 2, 2, 3, 4, 3},
				/*  3*/{ 1, 2, 0, 2, 1, 1, 2, 2, 3, 3, 2, 3, 1},
				/*  4*/{ 2, 1, 2, 0, 1, 3, 3, 4, 1, 1, 2, 3, 3},
				/*  5*/{ 2, 2, 1, 1, 0, 2, 3, 3, 2, 2, 3, 4, 2},
				/*  6*/{ 2, 3, 1, 3, 2, 0, 3, 3, 4, 2, 1, 2, 2},
				/*  7*/{ 1, 2, 2, 3, 3, 3, 0, 4, 4, 4, 4, 5, 3},
				/*  8*/{ 3, 4, 2, 4, 3, 3, 4, 0, 5, 5, 4, 5, 1},
				/*  9*/{ 3, 2, 3, 1, 2, 4, 4, 5, 0, 2, 3, 4, 4},
				/* 10*/{ 3, 2, 3, 1, 2, 2, 4, 5, 2, 0, 1, 2, 4},
				/* 11*/{ 3, 3, 2, 2, 3, 1, 4, 4, 3, 1, 0, 1, 3},
				/* 12*/{ 4, 4, 3, 3, 4, 2, 5, 5, 4, 2, 1, 0, 4},
				/* 13*/{ 2, 3, 1, 3, 2, 2, 3, 1, 4, 4, 3, 4, 0}
			};
			for (int y = 0; y < 13; y++)
			{
				for (int x = 0; x < 13; x++)
				{
					sectorCost[x][y] = tmp[x][y];
				}
			}
#pragma endregion

		}
		else
		{
			LOG("Could not find walkability tileset");
		}
		
	}

}

void M_PathFinding::AsignSectors()
{
	allowedSectors.clear();

	if (usingSectors == false)
	{
		for (int n = 1; n < sectors.size(); n++)
		{
			allowedSectors.push_back(n);
		}
	}
	else
	{
		int startingSector = tilesData[startTile.y*width + startTile.x].sector;
		int endingSector = tilesData[endTile.top().y*width + endTile.top().x].sector;
		if (startingSector != endingSector && endingSector != 0)
		{
			//Find the sectors the unit will have to walk trough
			allowedSectors.push_back(startingSector);
			bool found = false;
			int safe = 0;
			while (!found && safe < 50)
			{
				int cost = INT_MAX;
				int toPush = 0;
				for (int n = 0; n < sectors[allowedSectors.back()].waypoints.size() && n < 40; n++)
				{
					if (sectorCost[sectors[allowedSectors.back()].waypoints[n].connectsWithSector - 1][endingSector - 1] < cost)
					{
						cost = sectorCost[sectors[allowedSectors.back()].waypoints[n].connectsWithSector - 1][endingSector - 1];
						toPush = sectors[allowedSectors.back()].waypoints[n].connectsWithSector;
					}
				}
				if (toPush == 0)
				{
					break;
					LOG("Trying to find a path for a dead unit");
				}
				allowedSectors.push_back(toPush);
				if (allowedSectors.back() == endingSector)
				{
					found = true;
				}
				safe++;
			}

			std::vector<iPoint> waypointsToPush;
			//Find closer waypoint for each sector
			for (int m = 0; m < allowedSectors.size() - 1; m++)
			{
				int distance = INT_MAX;
				iPoint closerWaypoint(-1, -1);
				for (int n = 0; n < sectors[allowedSectors[m]].waypoints.size(); n++)
				{
					if (sectors[allowedSectors[m]].waypoints[n].connectsWithSector == allowedSectors[m + 1])
					{
						//Checking if the waypoint is still walkable
						if (IsWalkable(sectors[allowedSectors[m]].waypoints[n].tile.x, sectors[allowedSectors[m]].waypoints[n].tile.y) == false)
						{
							int range = 1;
							bool found = false;
							iPoint wpToChange = sectors[allowedSectors[m]].waypoints[n].tile;
							while (found == false)
							{
								for (int y = -range; y <= range && found == false; y++)
								{
									for (int x = -range; x <= range && found == false; x++)
									{
										//Checking the tile we're checking is the same sector it was the waypoint
										if (allowedSectors[m] == tilesData[(wpToChange.x + x) + (wpToChange.y + y) * width].sector)
										{
											//Checking the new waypoint is walkable
											if (IsWalkable(wpToChange.x + x, wpToChange.y + y) == true)
											{
												//Checking it's adjacent to the other sector
												for (int y2 = -1; y2 < 2; y2++)
												{
													for (int x2 = -1; x2 < 2; x2++)
													{
														if (tilesData[(wpToChange.x + x + x2) + (wpToChange.y + y + y2) * width].sector == allowedSectors[m + 1])
														{
															found = true;
														}
													}
												}
												if (found)
												{
													sectors[allowedSectors[m]].waypoints[n].tile.x = wpToChange.x + x;
													sectors[allowedSectors[m]].waypoints[n].tile.y = wpToChange.y + y;
												}

											}
										}
									}
								}
								range++;
							}
						}
					

						if (sectors[allowedSectors[m]].waypoints[n].tile.DistanceManhattan(endTile.top()) < distance)
						{
							closerWaypoint = sectors[allowedSectors[m]].waypoints[n].tile;
							distance = sectors[allowedSectors[m]].waypoints[n].tile.DistanceManhattan(endTile.top());
						}
					}
				}
				waypointsToPush.push_back(closerWaypoint);
			}
			//Push waypoints
			for (int n = waypointsToPush.size() - 1; n >= 0; n--)
			{
				if (startTile != waypointsToPush[n])
				{
					endTile.push(waypointsToPush[n]);
				}
			}
		}
		else
		{
			allowedSectors.push_back(startingSector);
		}
	}
}

bool M_PathFinding::IfPathPossible()
{
	if (startTileExists && endTileExists && !(startTile.x == endTile.top().x && startTile.y == endTile.top().y))
	{
		if (IsWalkable(startTile.x, startTile.y))
		{
			if (IsWalkable(endTile.top().x, endTile.top().y) == false)
			{
				iPoint newEndTile = endTile.top();
				int n = 0;
				while (IsWalkable(newEndTile.x, newEndTile.y) == false)
				{
					if (startTile.y >= endTile.top().y)
					{
						newEndTile.y += 1;
					}
					else
					{
						newEndTile.y -= 1;
					}
					if (n >= 10)
					{
						return false;
					}
					n++;
				}
				endTile.pop();
				endTile.push(newEndTile);
			}
		}
		return true;;
	}
	return false;
}

bool M_PathFinding::StartPathFinding()
{
	bool ret = false;
	pathFound = false;
	atLeastOneWaypoint = false;

	//Find sectors to work with
	AsignSectors();

	if (IfPathPossible())
	{
		pathFinished = false;
		newLowest = false;
		lowestF = height * width;
		ClearLists();
		ret = CreateFirstNode();
		stepCount = 1;
		nFrames = 0;
		if (ret)
		{
			pathStarted = true;
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
		firstNode->h = (abs(endTile.top().x - firstNode->tile.x) + abs(endTile.top().y - firstNode->tile.y)) * 10;
		firstNode->f = firstNode->h;
		openList.push_back(firstNode);
		ret = true;
	}
	if (!ret)
	{
		LOG("-- Pathfinding: Could not create first node --");
	}
	return ret;
}

std::list<node*>::iterator M_PathFinding::GetLowestF()
{
	std::list<node*>::iterator it = openList.begin();
	std::list<node*>::iterator ret;
	int f = (height * width) * 10;
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
	nodesCreated++;
	if (isDiagonal && !allowCorners)
	{
		if (!ValidSector(nParent->tile.x, newNode->tile.y) || !ValidSector(newNode->tile.x, nParent->tile.y))
		{
			nodesDestroyed++;
			RELEASE(newNode);
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
				std::list<node*>::iterator it = openList.begin();
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
		{
			RELEASE(newNode);
			nodesDestroyed++;
		}

	}
	else
	{
		RELEASE(newNode);
		nodesDestroyed++;
	}


	return ret;
}

bool M_PathFinding::AddChild(node* nParent, int x, int y, iPoint end, int cost, bool isDiagonal)
{
	bool ret = false;
	if (x >= 0 && y >= 0)
	{
		if (ValidSector(x, y))
		{
			ret = CreateSideNode(nParent, x, y, endTile.top(), cost, isDiagonal);
		}
	}
	return ret;
}

bool M_PathFinding::AddChilds(std::list<node*>::iterator nParent, iPoint end)
{
	if (AddChild((*nParent), (*nParent)->tile.x + 1, (*nParent)->tile.y, endTile.top(), 10, false))
		return true;
	if (AddChild((*nParent), (*nParent)->tile.x, (*nParent)->tile.y + 1, endTile.top(), 10, false))
		return true;
	if (AddChild((*nParent), (*nParent)->tile.x - 1, (*nParent)->tile.y, endTile.top(), 10, false))
		return true;
	if (AddChild((*nParent), (*nParent)->tile.x, (*nParent)->tile.y - 1, endTile.top(), 10, false))
		return true;
	if (allowDiagonals)
	{
		if (AddChild((*nParent), (*nParent)->tile.x + 1, (*nParent)->tile.y + 1, endTile.top(), 14, true))
			return true;
		if (AddChild((*nParent), (*nParent)->tile.x + 1, (*nParent)->tile.y - 1, endTile.top(), 14, true))
			return true;
		if (AddChild((*nParent), (*nParent)->tile.x - 1, (*nParent)->tile.y + 1, endTile.top(), 14, true))
			return true;
		if (AddChild((*nParent), (*nParent)->tile.x - 1, (*nParent)->tile.y - 1, endTile.top(), 14, true))
			return true;
	}

	//Move the parent to the closed list
	TransferItem(nParent);

	return false;
}


//Check if the node is in the open list and leaves in the list the less cost value node
bool M_PathFinding::CheckIfExists(node* _node)
{
	bool nodeExists = false;
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
			nodesDestroyed++;
			std::list<node*>::iterator it2 = it;
			it2++;
			RELEASE(*it);	
			openList.erase(it);
			it = it2;
			openList.push_back(_node);
		}
		else
		{
			return true;
		}

	}
	else
	{
		openList.push_back(_node);
	}

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
	if (node->tile.x == endTile.top().x && node->tile.y == endTile.top().y)
	{
		goal = node;
		pathFinished = true;
		return true;
	}

	return false;
}

void M_PathFinding::FinishPathFinding()
{
	node* _node;
	
	//Passing onto the next waypoint
	startTile = endTile.top();
	endTile.pop();
	//ClearLists();
	if (endTile.empty())
	{
		for (_node = goal; _node->parent; _node = _node->parent)
		{
			tmpOutput.push_back(_node->tile);
		}
		pathFound = true;
		pathStarted = false;
		pathFinished = true;
	}
	else
	{
		atLeastOneWaypoint = true;
		pathStarted = true;
		pathFinished = false;
		pathFound = false;
		allowedSectors.clear();
		allowedSectors.push_back(tilesData[endTile.top().y*width + endTile.top().x].sector);
	}
}

void M_PathFinding::CheckMaxLength()
{
	if (stepCount > MAX_NODES || nFrames > MAX_FRAMES)
	{
		LOG("Couldn't find a path: Steps: %i / %i, Frames: %i / %i ", stepCount, MAX_NODES, nFrames, MAX_FRAMES);
		if (atLeastOneWaypoint)
		{
			node* _node = NULL;
			for (_node = goal; _node->parent; _node = _node->parent)
			{
				tmpOutput.push_back(_node->tile);
			}
			for (int n = tmpOutput.size() - 1; n >= 0; n--)
			{
				output->push_back(tmpOutput[n]);
			}
		}
		else
		{
			output->push_back(globalStart);
		}
		working = false;
		stepCount = 1;
		nFrames = 0;
		ClearLists();
		while (!endTile.empty())
		{
			endTile.pop();
		}
		tmpOutput.clear();
	}
}

void M_PathFinding::TransferItem(std::list<node*>::iterator it)
{
	transfCount++;
	closedList.push_back(*it);
	if (displayPath)
	{
		debugList.push_back(**it);
	}
	openList.erase(it);
}

void M_PathFinding::ClearLists()
{
	if (!openList.empty())
	{
		std::list<node*>::iterator it = openList.begin();
		while (it != openList.end())
		{
			nodesDestroyed++;
			RELEASE(*it);
			it++;
		}
		openList.clear();
	}
	if (!closedList.empty())
	{
		std::list<node*>::iterator it = closedList.begin();
		while (it != closedList.end())
		{
			nodesDestroyed++;
			RELEASE(*it);
			it++;
		}
		closedList.clear();
	}
}

void M_PathFinding::Draw()
{
	
	iPoint start = App->render->ScreenToWorld(0, 0);
	start.x /= tile_width;
	start.y /= tile_height;

	int endY = start.y + (App->render->camera.h / App->events->GetScale() / (tile_height)) + 1;
	int endX = start.x + (App->render->camera.w / App->events->GetScale() / (tile_width)) + 2;

	CAP(start.x, 0, width);
	CAP(endX, 0, width);
	CAP(start.y, 0, height);
	CAP(endY, 0, height);

	std::vector<sector>::iterator checkingSect = sectors.begin();
	while (checkingSect != sectors.end())
	{
		std::vector<waypoint>::iterator wp = checkingSect->waypoints.begin();
		while (wp != checkingSect->waypoints.end())
		{
			iPoint tmp = MapToWorld(wp->tile.x, wp->tile.y);
			App->render->AddDebugRect({ tmp.x, tmp.y, 16, 16 }, true, 0, 200, 200, 200);
			wp++;
		}
			checkingSect++;
	}

	for (int y = start.y; y < endY && y < height; y++)
	{
		for (int x = start.x; x < endX && x < width; x++)
		{
			iPoint pos = MapToWorld(x, y);
			SDL_Rect posR = { pos.x, pos.y, 16, 16 };
			//TO CHANGE: We should check if y / x are positive. Code broke here with y = -1
			if (tilesData[y*width + x].walkable)
			{
				switch (tilesData[y*width + x].sector)
				{
				case 0:
				{
					App->render->AddRect(posR, true, 255, 255, 255, 80); break;
				}
				case 1:
				{
					App->render->AddRect(posR, true, 0, 255, 0, 20); break;
				}
				case 2:
				{
					App->render->AddRect(posR, true, 255, 0, 0, 20); break;
				}
				case 3:
				{
					App->render->AddRect(posR, true, 0, 0, 255, 20); break;
				}
				case 4:
				{
					App->render->AddRect(posR, true, 255, 255, 0, 20); break;
				}
				case 5:
				{
					App->render->AddRect(posR, true, 255, 0, 255, 20); break;
				}
				case 6:
				{
					App->render->AddRect(posR, true, 0, 255, 255, 20); break;
				}
				case 7:
				{
					App->render->AddRect(posR, true, 255, 144, 0, 20); break;
				}
				case 8:
				{
					App->render->AddRect(posR, true, 144, 0, 255, 20); break;
				}
				case 9:
				{
					App->render->AddRect(posR, true, 144, 144, 144, 20); break;
				}
				case 10:
				{
					App->render->AddRect(posR, true, 255, 60, 0, 20); break;
				}
				case 11:
				{
					App->render->AddRect(posR, true, 255, 255, 50, 20); break;
				}
				case 12:
				{
					App->render->AddRect(posR, true, 255, 0, 200, 20); break;
				}
				}
			}
			else
			{
				App->render->AddRect(posR, true, 255, 255, 255, 125);
			}

			App->render->DrawLine(posR.x, posR.y, posR.x + 16, posR.y, true, 0, 0, 0, 100);
			App->render->DrawLine(posR.x, posR.y, posR.x, posR.y + 16, true, 0, 0, 0, 100);
		}
	}
}

iPoint M_PathFinding::MapToWorld(int x, int y) const
{
	iPoint ret;

	ret.x = x * tile_width;
	ret.y = y * tile_height;

	return ret;
}

iPoint M_PathFinding::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	ret.x = x / tile_width;
	ret.y = y / tile_height;

	return ret;
}

void M_PathFinding::ChangeWalkability(int x, int y, bool walkable)
{
	if (x < width && x >= 0 && y < height && y >= 0)
	{
		if (tilesData[y*width + x].walkable != walkable)
			mapChanged = true;
		tilesData[y*width + x].walkable = walkable;
	}
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