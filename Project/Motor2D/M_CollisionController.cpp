#include "M_CollisionController.h"

#include "j1App.h"

#include "M_PathFinding.h"
#include "Building.h"
#include "Unit.h"
#include "Entity.h"
#include "M_EntityManager.h"
#include "M_Player.h"

M_CollisionController::M_CollisionController(bool start_enabled) : j1Module(start_enabled)
{
	name.create("collision_controller");
}

// Destructor
M_CollisionController::~M_CollisionController()
{}

// Called before render is available
bool M_CollisionController::Awake(pugi::xml_node& node)
{

	return true;
}

// Called before the first frame
bool M_CollisionController::Start()
{
	timer.Start();
	return true;
}

// Called each loop iteration
bool M_CollisionController::PreUpdate()
{

	return true;
}

// Called each loop iteration
bool M_CollisionController::Update(float dt)
{
	if (timer.ReadSec() >= 0.1)
	{
		performanceTimer.Start();
		DoUnitLoop();
		//LOG("Collision controller unit loop took %f ms with %i units", performanceTimer.ReadMs(), App->entityManager->unitCount);
		DoBuildingLoop();
		timer.Start();
	}

	return true;
}

// Called each loop iteration
bool M_CollisionController::PostUpdate(float dt)
{
	bool ret = true;
	
	return ret;
}

bool M_CollisionController::CleanUp()
{
	return true;
}

void M_CollisionController::ManageInput(float dt)
{

}

void M_CollisionController::DoUnitLoop()
{
	for (int i = 0; i < App->entityManager->unitList.size(); i++)
	{
		Unit* unit = &App->entityManager->unitList[i];
		if (!unit->dead && unit->active)
		{
			if (unit->GetState() != STATE_DIE && unit->stats.player != CINEMATIC)
			{
				//Unit interaction with environment ----------------------------------------------------------------------------------------
				//Path in non-walkable tile controller ----------------
				if (unit->GetMovementState() == MOVEMENT_MOVE)
				{
					if (mapChanged && unit->GetMovementType() == GROUND)
					{
						bool stop = false;
						//If the map has changed, check that all nodes are still walkable
						for (int n = unit->currentNode; n < unit->path.size() && stop == false; n++)
						{
							if (!App->pathFinding->IsWalkable(unit->path[n].x, unit->path[n].y))
							{
								stop = true;
								//If they aren't, find the furthest node that's still walkable
								for (int m = unit->path.size() - 1; m >= 0; m--)
								{
									if (App->pathFinding->IsWalkable(unit->path[m].x, unit->path[m].y))
									{
										unit->Move(unit->path[m], unit->GetAttackState(), PRIORITY_MEDIUM);
									}
								}
							}
						}
					}
				}
				//----------------------------------------------------
				else if (unit->GetMovementType() == GROUND)
				{
					iPoint unitPos = App->pathFinding->WorldToMap(unit->GetPosition().x, unit->GetPosition().y);

					//Unit in non-walkable tile controller ---------------
					if (!App->pathFinding->IsWalkable(unitPos.x, unitPos.y))
					{
						LOG("Unit in no-walkable tile");
						iPoint tile = FindClosestWalkable(unitPos.x, unitPos.y);
						iPoint dst = App->pathFinding->MapToWorld(tile.x, tile.y);
						unit->SetTarget(dst.x, dst.y);
						unit->path.clear();
					}
				}
				//------------------------------------------------------------------------------------------------------------------------

				//Interaction between units----------------------------------------------------
				if (unit->GetMovementState() != MOVEMENT_WAIT && unit->GetState() != STATE_GATHER && unit->GetState() != STATE_GATHER_RETURN)
				{
					bool stop = false;
					for (int j = 0; j < App->entityManager->unitList.size(); j++ && !stop)
					{
						Unit* unit2 = &App->entityManager->unitList[j];
						if (unit2->active && !unit2->dead)
						{
							if (unit != unit2 && unit->GetAttackState() == ATTACK_ATTACK && unit2->GetState() != STATE_DIE && unit2->stats.player != CINEMATIC)
							{
								if (unit->stats.player != unit2->stats.player && unit->stats.attackDmg != 0 && unit->stats.type != KERRIGAN && unit->GetAttackState() != ATTACK_STAND)
								{
									if (unit->HasVision(unit2))
									{
										unit->SetAttack(unit2);
										stop = true;
									}
								}
								if (unit->GetMovementState() == MOVEMENT_IDLE && unit2->GetMovementState() == MOVEMENT_IDLE &&
									unit->GetMovementType() == GROUND && unit->GetMovementType() == GROUND)
								{
									if (DoUnitsIntersect(unit, unit2))
									{
										if (unit->waitingForPath)
										{
											if (!unit2->waitingForPath)
												SplitUnits(unit, unit2);
										}
										else if (unit2->waitingForPath)
										{
											SplitUnits(unit2, unit);
											stop = true;
										}

										else
										{
											if (unit->priority < unit2->priority)
												SplitUnits(unit, unit2);
											else
											{
												SplitUnits(unit2, unit);
												stop = true;
											}
										}
									}
								}
							}
						}

					}
				
					//Checking for buildings to attack
					if (unit->GetAttackState() == ATTACK_ATTACK && unit->GetMovementState() != MOVEMENT_ATTACK_IDLE && unit->GetMovementState() != MOVEMENT_ATTACK_ATTACK)
					{
						for (int i = 0; i < App->entityManager->buildingList.size(); i++)
						{
							Building* building = &App->entityManager->buildingList[i];
							if ((unit->stats.player != building->stats.player || unit->stats.type == GODMODE) && unit->stats.attackDmg != 0 && building->state != BS_DEAD && building->stats.player != CINEMATIC && building->state != BS_SPAWNING)
							{
								if (unit->GetType() != KERRIGAN || (unit->movement_state != MOVEMENT_BOSS_EXPLODING && unit->movement_state != MOVEMENT_BOSS_STUNNED))
								{
									if (unit->HasVision(building))
									{
										LOG("Set attack");
										unit->SetAttack(building);
									}
								}
							}
						}
					}
				}
			}
		}

	}

	if (mapChanged)
		mapChanged = false;
}

void M_CollisionController::DoBuildingLoop()
{
	for (int i = 0; i < App->entityManager->buildingList.size(); i++)
	{
		Building* building = &App->entityManager->buildingList[i];
		if (building->GetType() == PHOTON_CANNON)
		{
			if (building->state != BS_ATTACKING && building->state != BS_DEAD && building->state != BS_SPAWNING)
			{
				for (int j = 0; j < App->entityManager->unitList.size(); j++)
				{
					Unit* unit = &App->entityManager->unitList[i];
					if (unit->GetState() != STATE_DIE)
					{
						if ((building->stats.player != unit->stats.player) && building->HasVision(unit))
						{
							building->SetAttack(unit);
							break;
						}
					};
				}
			}
		}
		else if (building->GetType() == SUNKEN_COLONY)
		{
			if (building->state != BS_ATTACKING && building->state != BS_DEAD && building->state != BS_SPAWNING)
			{
				for (int i = 0; i < App->entityManager->unitList.size(); i++)
				{
					Unit* unit = &App->entityManager->unitList[i];
					if (unit->GetState() != STATE_DIE && unit->GetMovementType() == GROUND)
					{
						if ((building->stats.player != (unit)->stats.player) && building->HasVision(unit))
						{
							building->SetAttack(unit);
							break;
						}
					}
				}
			}
		}
		else if (building->GetType() == SPORE_COLONY)
		{
			if (building->state != BS_ATTACKING && building->state != BS_DEAD && building->state != BS_SPAWNING)
			{
				for (int i = 0; i < App->entityManager->unitList.size(); i++)
				{
					Unit* unit = &App->entityManager->unitList[i];
					if (unit->GetState() != STATE_DIE && unit->GetMovementType() != GROUND)
					{
						if ((unit->stats.player != unit->stats.player) && building->HasVision(unit))
						{
							building->SetAttack(unit);
							break;
						}
					}
				}
			}
		}
	}
}

iPoint M_CollisionController::FindClosestWalkable(int x, int y)
{
	bool found = false;
	int lenght = 3;

	iPoint tile = { x + lenght / 2, y - lenght / 2 - 1 };

	while (!found && lenght < 20)
	{
		while (tile.y < y + lenght / 2 && !found)
		{
			tile.y++;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;
		}

		while (tile.x > x - lenght / 2 && !found)
		{
			tile.x--;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;
		}

		while (tile.y > y - lenght / 2 && !found)
		{
			tile.y--;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;

		}

		while (tile.x < x + lenght / 2 && !found)
		{
				tile.x++;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;
		}
		lenght+=2;
	}
	return tile;
}

bool M_CollisionController::DoUnitsIntersect(Unit* unit1, Unit* unit2)
{
	C_Vec2<float> distance = { unit1->GetPosition().x - unit2->GetPosition().x, unit1->GetPosition().y - unit2->GetPosition().y };
	return (distance.GetModule() < unit1->colRadius + unit2->colRadius);
}

//Higher priority unit is unit1, we will move unit2
void M_CollisionController::SplitUnits(Unit* unit1, Unit* unit2)
{
	C_Vec2<float> vec = { unit2->GetPosition().x - unit1->GetPosition().x, unit2->GetPosition().y - unit1->GetPosition().y };
	vec.position = unit1->GetPosition();
	vec.Normalize();
	vec *= unit1->colRadius + unit2->colRadius + 1;
	if (vec.GetModule() == 0)
	{
		vec.x = unit1->colRadius + unit2->colRadius + 1;
		vec.SetAngle(rand() % 360);
	}

	fPoint pos = vec.position + fPoint{ vec.x, vec.y };
	iPoint tile = App->pathFinding->WorldToMap(pos.x, pos.y);
	iPoint dst = App->pathFinding->MapToWorld(tile.x, tile.y);

	int loops = 0;
	while (!App->pathFinding->IsWalkable(tile.x, tile.y) && loops < 24)
	{
		vec.SetAngle(vec.GetAngle() + 15);
		pos = vec.position + fPoint{ vec.x, vec.y };
		tile = App->pathFinding->WorldToMap(pos.x, pos.y);
		dst = App->pathFinding->MapToWorld(tile.x, tile.y);
		loops++;
	}

	unit2->SetTarget(pos.x, pos.y);
	unit2->path.clear();
}