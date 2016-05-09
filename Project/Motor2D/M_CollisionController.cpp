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
		DoUnitLoop();
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
	std::list<Unit*>::iterator it = App->entityManager->unitList.begin();
	while (it != App->entityManager->unitList.end())
	{
		if ((*it)->GetState() != STATE_DIE && (*it)->stats.player != CINEMATIC)
		{
			//Unit interaction with environment ----------------------------------------------------------------------------------------
			//Path in non-walkable tile controller ----------------
			if ((*it)->GetMovementState() == MOVEMENT_MOVE)
			{
				if (mapChanged && (*it)->GetMovementType() == GROUND)
				{
					bool stop = false;
					//If the map has changed, check that all nodes are still walkable
					for (int n = (*it)->currentNode; n < (*it)->path.size() && stop == false; n++)
					{
						if (!App->pathFinding->IsWalkable((*it)->path[n].x, (*it)->path[n].y))
						{
							stop = true;
							//If they aren't, find the furthest node that's still walkable
							for (int m = (*it)->path.size() - 1; m >= 0; m--)
							{
								if (App->pathFinding->IsWalkable((*it)->path[m].x, (*it)->path[m].y))
								{
									(*it)->Move((*it)->path[m], (*it)->GetAttackState(), PRIORITY_MEDIUM);
								}
							}
						}
					}
				}
			}
			//----------------------------------------------------
			else if ((*it)->GetMovementType() == GROUND)
			{
				iPoint unitPos = App->pathFinding->WorldToMap((*it)->GetPosition().x, (*it)->GetPosition().y);

				//Unit in non-walkable tile controller ---------------
				if (!App->pathFinding->IsWalkable(unitPos.x, unitPos.y))
				{
					LOG("Unit in no-walkable tile");
					iPoint tile = FindClosestWalkable(unitPos.x, unitPos.y);
					iPoint dst = App->pathFinding->MapToWorld(tile.x, tile.y);
					(*it)->SetTarget(dst.x, dst.y);
					(*it)->path.clear();
				}
			}
			//------------------------------------------------------------------------------------------------------------------------

			//Interaction between units----------------------------------------------------
			if ((*it)->GetMovementState() != MOVEMENT_WAIT && (*it)->GetState() != STATE_GATHER && (*it)->GetState() != STATE_GATHER_RETURN)
			{
				bool stop = false;
				std::list<Unit*>::iterator it2 = App->entityManager->unitList.begin();
				while (it2 != App->entityManager->unitList.end() && !stop)
				{
					if (*it != *it2 && (*it)->GetAttackState() == ATTACK_ATTACK && (*it2)->GetState() != STATE_DIE && (*it2)->stats.player != CINEMATIC)
					{
						if ((*it)->stats.player != (*it2)->stats.player && (*it)->stats.attackDmg != 0 && (*it)->stats.type != KERRIGAN && (*it)->GetAttackState() != ATTACK_STAND)
						{
							if ((*it)->HasVision(*it2))
							{
								(*it)->SetAttack(*it2);
								stop = true;
							}
						}
						if ((*it)->GetMovementState() == MOVEMENT_IDLE && (*it2)->GetMovementState() == MOVEMENT_IDLE &&
							(*it)->GetMovementType() == GROUND && (*it)->GetMovementType() == GROUND)
						{
							if (DoUnitsIntersect(*it, *it2))
							{
								if ((*it)->waitingForPath)
								{
									if (!(*it2)->waitingForPath)
										SplitUnits(*it, *it2);
								}
								else if ((*it2)->waitingForPath)
								{
									SplitUnits(*it2, *it);
									stop = true;
								}

								else
								{
									if ((*it)->priority < (*it2)->priority)
										SplitUnits(*it, *it2);
									else
									{
										SplitUnits(*it2, *it);
										stop = true;
									}
								}
							}
						}
					}
					it2++;
				}
				
				//Checking for buildings to attack
				if ((*it)->GetAttackState() == ATTACK_ATTACK && (*it)->GetMovementState() != MOVEMENT_ATTACK_IDLE && (*it)->GetMovementState() != MOVEMENT_ATTACK_ATTACK)
				{
					std::list<Building*>::iterator it_building = App->entityManager->buildingList.begin();
					while (it_building != App->entityManager->buildingList.end())
					{
						if (((*it)->stats.player != (*it_building)->stats.player || (*it)->stats.type == GODMODE) && (*it)->stats.attackDmg != 0 && (*it_building)->state != BS_DEAD && (*it_building)->stats.player != CINEMATIC && (*it_building)->state != BS_SPAWNING)
						{
							if ((*it)->GetType() != KERRIGAN || ((*it)->movement_state != MOVEMENT_BOSS_EXPLODING && (*it)->movement_state != MOVEMENT_BOSS_STUNNED))
							{
								if ((*it)->HasVision(*it_building))
								{
									LOG("Set attack");
									(*it)->SetAttack(*it_building);
								}
							}
						}
						it_building++;
					}
				}
			}
		}
	it++;
	}

	if (mapChanged)
		mapChanged = false;
}

void M_CollisionController::DoBuildingLoop()
{
	std::list<Building*>::iterator it = App->entityManager->buildingList.begin();
	while (it != App->entityManager->buildingList.end())
	{
		if ((*it)->GetType() == PHOTON_CANNON)
		{
			if ((*it)->state != BS_ATTACKING && (*it)->state != BS_DEAD && (*it)->state != BS_SPAWNING)
			{
				std::list<Unit*>::iterator unit_it = App->entityManager->unitList.begin();
				while (unit_it != App->entityManager->unitList.end())
				{
					if ((*unit_it)->GetState() != STATE_DIE)
					{
						if (((*it)->stats.player != (*unit_it)->stats.player) && (*it)->HasVision(*unit_it))
						{
							(*it)->SetAttack(*unit_it);
							break;
						}
					}
					unit_it++;
				}
			}
		}
		else if ((*it)->GetType() == SUNKEN_COLONY)
		{
			if ((*it)->state != BS_ATTACKING && (*it)->state != BS_DEAD && (*it)->state != BS_SPAWNING)
			{
				std::list<Unit*>::iterator unit_it = App->entityManager->unitList.begin();
				while (unit_it != App->entityManager->unitList.end())
				{
					if ((*unit_it)->GetState() != STATE_DIE && (*unit_it)->GetMovementType() == GROUND)
					{
						if (((*it)->stats.player != (*unit_it)->stats.player) && (*it)->HasVision(*unit_it))
						{
							(*it)->SetAttack(*unit_it);
							break;
						}
					}
					unit_it++;
				}
			}
		}
		else if ((*it)->GetType() == SPORE_COLONY)
		{
			if ((*it)->state != BS_ATTACKING && (*it)->state != BS_DEAD && (*it)->state != BS_SPAWNING)
			{
				std::list<Unit*>::iterator unit_it = App->entityManager->unitList.begin();
				while (unit_it != App->entityManager->unitList.end())
				{
					if ((*unit_it)->GetState() != STATE_DIE && (*unit_it)->GetMovementType() != GROUND)
					{
						if (((*it)->stats.player != (*unit_it)->stats.player) && (*it)->HasVision(*unit_it))
						{
							(*it)->SetAttack(*unit_it);
							break;
						}
					}
					unit_it++;
				}
			}
		}
		it++;
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