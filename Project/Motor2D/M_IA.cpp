#include "M_IA.h"
#include "j1App.h"
#include "M_FileSystem.h"
#include "M_Map.h"
#include "M_PathFinding.h"
#include "M_Render.h"
#include "Boss.h"
#include "M_InputManager.h"
#include "M_EntityManager.h"
#include "Unit.h"
#include "Building.h"
#include "M_GUI.h"
#include "M_Minimap.h"

#include "M_Player.h"

#include <ctime>

//General Base ---------------------------------------------------------------------------------------------------------------

bool Base::BaseUpdate(float dt)
{
	//Update base only once every second
	if (updateDelay.ReadSec() > BASE_UPDATE_DELAY)
	{
	//	LOG("Updated %s", name.GetString());
		//Only check timer if the base is still spawning units
		if (spawning)
		{
			if (generationTimer.ReadSec() >= generationDelay)
			{
				generationTimer.Start();
				Spawn();
			}
		}
		else
		{
			std::list<Building*>::iterator turret = turrets.begin();
			while (turret != turrets.end())
			{
				(*turret)->Hit(10);
				turret++;
			}
		}
		//Get out of the list dead units
		ClearDeadUnits();
		//Check if it must send units out
		CheckBaseUnits();
		//Give orders to wandering units
		UpdateOutOfBaseUnits();
		//Any changes the base should do
		PersonalUpdate();
		//Reset timer
		updateDelay.Start();
	}
	if (changingCreepOpacity)
	{
		creep->opacity--;
		if (creep->opacity == 0)
		{
			changingCreepOpacity = false;
			if (creepOnMap && App->minimap->enabled)
			{
				creepOnMap->SetActive(false);
				creepOnMap = NULL;
			}
		}
	}

	//Check if the base has been erradicated
	return IsBaseAlive();
}

bool Base::PersonalUpdate()
{
	return true;
}

void Base::Spawn()
{
	iPoint spawnPoint = spawningPoints[whereToSpawn];
	spawnPoint = App->pathFinding->WorldToMap(spawnPoint.x, spawnPoint.y);
	buildings.back()->hasWaypoint = true;
	buildings.back()->waypointTile = spawnPoint;
	Unit* tmp = buildings.back()->CreateUnit(typeOfBase, COMPUTER);
	unitsInBase.push_back(tmp);
	whereToSpawn++;
	if (whereToSpawn >= spawningPoints.size())
	{
		whereToSpawn = 0;
	}
}

void Base::Kill()
{
	std::list<Unit*>::iterator it = unitsInBase.begin();
	while (it != unitsInBase.end())
	{
		(*it)->Hit(100000);
		it++;
	}
	it = unitsOutOfBase.begin();
	while (it != unitsOutOfBase.end())
	{
		(*it)->Hit(100000);
		it++;
	}

	std::list<Building*>::iterator it2 = buildings.begin();
	while (it2 != buildings.end())
	{
		(*it2)->Hit(100000);
		it2++;
	}
}

bool Base::IsBaseAlive()
{
	bool ret = false;
	if (!buildings.empty())
	{
		ret = true;
	}
	else if (spawning)
	{
		spawning = false;
		changingCreepOpacity = true;
		if (typeOfBase == ULTRALISK)
		{
			App->IA->createBoss = true;
		}
	}

	if (!ret && !unitsInBase.empty())
	{
		ret = true;
	}
	else
	{
		if (spawning == false)
		{
			defeated = true;
		}
	}
	if (!ret && !unitsOutOfBase.empty())
	{
		ret = true;
	}
	if (!ret && !turrets.empty())
	{
		ret = true;
	}

	if (changingCreepOpacity)
	{
		ret = true;
	}
	return ret;
}

void Base::CheckBaseUnits()
{
	if (App->IA->bossPhase == false)
	{
		sentUnits = false;
		//If there are enough Zergs, assign some to the "Out of Base" list
		if (unitsInBase.size() >= baseUnitsReactN)
		{
			std::list<Unit*>::iterator itToSend = unitsInBase.begin();
			std::list<Unit*>::iterator itToSend2 = itToSend;
			for (int n = 0; n < unitsToSend; n++)
			{
				itToSend2 = itToSend;
				itToSend2++;

				unitsOutOfBase.push_back((*itToSend));
				unitsInBase.erase(itToSend);

				itToSend = itToSend2;
			}
			LOG("%s has %i units, sending %i out. There are %i units out of base now.", name.GetString(), unitsInBase.size() + unitsToSend, unitsToSend, unitsOutOfBase.size());
			sentUnits = true;
		}

		//If the ones in the base wandered too far away, send them back to the base
		if (!buildings.empty())
		{
			std::list<Unit*>::iterator it = unitsInBase.begin();
			std::list<Unit*>::iterator it2 = it;
			iPoint basePos = App->pathFinding->MapToWorld(buildings.front()->GetPosition().x, buildings.front()->GetPosition().y);
			while (it != unitsInBase.end())
			{
				if ((*it)->GetState() == STATE_STAND)
				{
					iPoint ZergPos((*it)->GetPosition().x, (*it)->GetPosition().y);
					if (ZergPos.DistanceManhattan(basePos) > 650)
					{
						int spawnToHead = rand() % spawningPoints.size();
						iPoint toSend = App->pathFinding->WorldToMap(spawningPoints[spawnToHead].x, spawningPoints[spawnToHead].y);
						(*it)->Move(toSend, ATTACK_ATTACK, PRIORITY_LOW);
					}
				}
				it++;
			}
		}
	}
	else
	{
		std::list<Unit*>::iterator it = unitsInBase.begin();
		while (it != unitsInBase.end())
		{
			if ((*it)->GetState() == STATE_STAND)
			{
				(*it)->Move(iPoint(28, 159), ATTACK_ATTACK, PRIORITY_LOW);
			}
			it++;
		}

		it = unitsOutOfBase.begin();
		while (it != unitsOutOfBase.end())
		{
			if ((*it)->GetState() == STATE_STAND)
			{
				(*it)->Move(iPoint(28, 159), ATTACK_ATTACK, PRIORITY_LOW);
			}
			it++;
		}
	}
}

void Base::UpdateOutOfBaseUnits()
{

}

void Base::ClearDeadUnits()
{
	std::list<Unit*>::iterator itOut = unitsOutOfBase.begin();
	std::list<Unit*>::iterator itOut2 = itOut;
	while (itOut != unitsOutOfBase.end())
	{
		if ((*itOut)->GetState() == STATE_DIE)
		{
			itOut2 = itOut;
			itOut2++;
			unitsOutOfBase.erase(itOut);
			itOut = itOut2;
		}
		else
		{
			itOut++;
		}
	}

	std::list<Unit*>::iterator itIn = unitsInBase.begin();
	std::list<Unit*>::iterator itIn2 = itIn;
	while (itIn != unitsInBase.end())
	{
		if ((*itIn)->GetState() == STATE_DIE)
		{
			App->IA->lastDeath = (*itIn)->GetPosition();
			App->IA->aZergDied = true;
			itIn2 = itIn;
			itIn2++;
			unitsInBase.erase(itIn);
			itIn = itIn2;
		}
		else
		{
			itIn++;
		}
	}

	std::list<Building*>::iterator itBuilding = buildings.begin();
	std::list<Building*>::iterator itBuilding2 = itBuilding;
	while (itBuilding != buildings.end())
	{
		if ((*itBuilding)->state == BS_DEAD)
		{
			itBuilding2 = itBuilding;
			itBuilding2++;
			buildings.erase(itBuilding);
			itBuilding = itBuilding2;
		}
		else
		{
			itBuilding++;
		}
	}

	itBuilding = turrets.begin();
	while (itBuilding != turrets.end())
	{
		if ((*itBuilding)->state == BS_DEAD)
		{
			itBuilding2 = itBuilding;
			itBuilding2++;
			turrets.erase(itBuilding);
			itBuilding = itBuilding2;
		}
		else
		{
			itBuilding++;
		}
	}
}


//Zergling Base -------------------------------------------------------------------------------------------------------------
Base_Zergling::Base_Zergling() : Base("Zergling base")
{	
	typeOfBase = ZERGLING;
	personalBuilding = SPAWNING_POOL;
}

bool Base_Zergling::PersonalUpdate()
{
	if (sentUnits)
	{
		generationDelay -= 15.0f;
		CAP(generationDelay, 20, 500);
		int toIncrease = unitsToSend * 2 - 1;
		baseUnitsReactN += toIncrease;
		unitsToSend += toIncrease;
		CAP(baseUnitsReactN, 2, 30);
		CAP(unitsToSend, 1, 12);
	}
	return true;
}

void Base_Zergling::UpdateOutOfBaseUnits()
{
	//Send them to attack if they're doing nothing
	std::list<Unit*>::iterator it = unitsOutOfBase.begin();
	while (it != unitsOutOfBase.end())
	{
		if ((*it)->GetState() == STATE_STAND)
		{
			(*it)->Move(iPoint(28, 159), ATTACK_ATTACK, PRIORITY_LOW);
		}
		it++;
	}
}


//Hydralisk Base -------------------------------------------------------------------------------------------------------------
Base_Hydralisk::Base_Hydralisk() : Base("Hydralisk base")
{
	typeOfBase = HYDRALISK;
	personalBuilding = HYDRALISK_DEN;
}

bool Base_Hydralisk::PersonalUpdate()
{
	if (unitsOutOfBase.empty() == false)
	{
		generationDelay = 160;
	}
	else
	{
		generationDelay = unitsInBase.size() * 10;
		if (generationDelay < 30)
		{
			generationDelay = 30;
		}
	}
	std::list<Unit*>::iterator it = unitsOutOfBase.begin();
	std::list<Unit*>::iterator it2 = it;
	int n = 0;
	if (spawningPoints.size() > 0)
	{
		n = rand() % spawningPoints.size();
	}
	while (it != unitsOutOfBase.end())
	{
		if ((*it)->GetState() == STATE_STAND)
		{
			iPoint ZergPos((*it)->GetPosition().x, (*it)->GetPosition().y);
			if (ZergPos.DistanceManhattan(spawningPoints[n % spawningPoints.size()]) > 1200)
			{
				iPoint toSend = App->pathFinding->WorldToMap(spawningPoints[n % spawningPoints.size()].x, spawningPoints[n % spawningPoints.size()].y);
				(*it)->Move(toSend, ATTACK_ATTACK, PRIORITY_LOW);
				n++;
			}
		}
		it++;
	}
	return true;
}

void Base_Hydralisk::UpdateOutOfBaseUnits()
{
	//If a zerg in a base died, send the "out of base hydralisks" to aid that base
	if (App->IA->aZergDied)
	{
		iPoint toSend =	App->pathFinding->WorldToMap(App->IA->lastDeath.x, App->IA->lastDeath.y);
		std::list<Unit*>::iterator it = unitsOutOfBase.begin();
		while (it != unitsOutOfBase.end())
		{
			(*it)->Move(toSend, ATTACK_ATTACK, PRIORITY_LOW);
			it++;
		}
		App->IA->aZergDied = false;
	}
}


//Mutalisk Base -------------------------------------------------------------------------------------------------------------
Base_Mutalisk::Base_Mutalisk() : Base("Mutalisk base")
{
	typeOfBase = MUTALISK;
	personalBuilding = SPIRE;
}

bool Base_Mutalisk::PersonalUpdate()
{
	if (sentUnits && generationDelay > 70)
	{
		generationDelay -= 5;
	}
	return true;
}

void Base_Mutalisk::UpdateOutOfBaseUnits()
{
	//Send them to a random location if they're doing nothing
	srand(time(NULL));
	std::list<Unit*>::iterator it = unitsOutOfBase.begin();
	while (it != unitsOutOfBase.end())
	{
		if ((*it)->GetState() == STATE_STAND)
		{
			iPoint toMove;
			toMove.x = (rand() % (App->pathFinding->width - 10)) + 5;
			toMove.y = ((rand() + rand()) % (App->pathFinding->height - 10)) + 5;
			(*it)->Move(toMove, ATTACK_ATTACK, PRIORITY_LOW);
		}
		it++;
	}
}


//Terran Base -------------------------------------------------------------------------------------------------------------
Base_Terran::Base_Terran() : Base("Terran base")
{
	typeOfBase = INFESTED_TERRAN;
	personalBuilding = INFESTED_COMMAND_CENTER;
}

bool Base_Terran::PersonalUpdate()
{
	if (unitsInBase.size() >= baseUnitsReactN - 2)
	{
		generationDelay = 200;
	}
	else
	{
		generationDelay = 100;
	}

	if (sentUnits)
	{
		while (true)
		{
			int x = rand() % App->pathFinding->width;
			int y = rand() % App->pathFinding->height;
			if (App->pathFinding->IsWalkable(x, y));
			{
				unitsOutOfBase.back()->Move({ x, y }, ATTACK_ATTACK, PRIORITY_LOW);
				break;
			}
		}

		
		sentUnits = false;
	}
	return true;
}

void Base_Terran::UpdateOutOfBaseUnits()
{
}


//Ultralisk Base -------------------------------------------------------------------------------------------------------------
Base_Ultralisk::Base_Ultralisk() : Base("Ultralisk base")
{
	typeOfBase = ULTRALISK;
	personalBuilding = ULTRALISK_CAVERN;
}

bool Base_Ultralisk::PersonalUpdate()
{
	if (sentUnits)
	{
		generationDelay -= 60;
		CAP(generationDelay, 90, 500);

		unitsToSend += 1;
		CAP(unitsToSend, 1, 2);

	}
	return true;
}

void Base_Ultralisk::UpdateOutOfBaseUnits()
{
	//Send them to attack if they're doing nothing
	std::list<Unit*>::iterator it = unitsOutOfBase.begin();
	while (it != unitsOutOfBase.end())
	{
		if ((*it)->GetState() == STATE_STAND)
		{
			(*it)->Move(iPoint(28, 159), ATTACK_ATTACK, PRIORITY_LOW);
		}
		it++;
	}
}



//Module IA ------------------------------------------------------------------------------------------------------------------

M_IA::M_IA(bool start_enabled) : j1Module(start_enabled)
{
	name.create("IA");
}

bool M_IA::Start()
{
	bool ret = true;
	Base* toPush = NULL;
	createBoss = false;

#pragma region //Random base selection

	int bases[4];
	std::vector<int> possibles = { 0, 3, 1, 1, 2, 2 };

	srand(time(NULL));
	for (int n = 0; n < 3; n++)
	{
		std::vector<int>::iterator it;
		int random = rand() % possibles.size();
		it = possibles.begin();
		for (int m = 0; m < random; m++)
		{
			it++;
		}
		bases[n] = possibles[random];
		possibles.erase(it);
	}
	bases[rand() % 3] = 0;
	bases[3] = 4;


#pragma endregion

#pragma region Loading xml data
	char* buf;
	int size = App->fs->Load("ZergBases.xml", &buf);
	pugi::xml_document file;
	pugi::xml_parse_result result = file.load_buffer(buf, size);

	RELEASE_ARRAY(buf);

	if (result == NULL)
	{
		LOG("Could not load Zerg Bases.xml. Pugi error: %s", result.description());
		return false;
	}

	srand(time(NULL));
	pugi::xml_node mainNode = file.child("bases");
	pugi::xml_node spawningPoints = file.child("bases").child("location");
	for (int n = 0; n < 4; n++)
	{
		//Generating a random number that will decide the types of base that will spawn
		C_String baseType;
		if (n < N_OF_RANDOM_BASES)
		{
			switch (bases[n])
			{
			case 0:
			{
				toPush = new Base_Zergling();
				baseType = "Zergling base"; break;
			}
			case 1:
			{
				toPush = new Base_Mutalisk();
				baseType = "Mutalisk base"; break;
			}
			case 2:
			{
				toPush = new Base_Hydralisk();
				baseType = "Hydralisk base"; break;
			}
			case 3:
			{
				toPush = new Base_Terran();
				baseType = "Terran base"; break;
			}
			}
		}
		else
		{
			toPush = new Base_Ultralisk();
			baseType = "Ultralisk base";
		}

		//Finding the correspondant xml section for that base
		pugi::xml_node node;
		for (node = mainNode.child("base"); baseType != node.child("name").attribute("value").as_string() && node; node = node.next_sibling("base"))
		{
		}

		//Loading base data
		int startingUnits = node.child("startingUnits").attribute("value").as_int();
		int nOfSpawningPoints = node.child("spawnPoints").attribute("value").as_int();
		toPush->generationDelay = node.child("generationTimer").attribute("value").as_float();
		toPush->baseUnitsReactN = node.child("reactUnitsN").attribute("value").as_int();
		toPush->unitsToSend = node.child("unitsToSend").attribute("value").as_int();

		//Setting as many spawpoints for that base as it should have
		pugi::xml_node thisBaseSpawningPoints = spawningPoints.child("spawningPoint");
		for (int n = 0; thisBaseSpawningPoints && n < nOfSpawningPoints; thisBaseSpawningPoints = thisBaseSpawningPoints.next_sibling("spawningPoint"))
		{
			iPoint point;
			point.x = thisBaseSpawningPoints.attribute("x").as_int();
			point.y = thisBaseSpawningPoints.attribute("y").as_int();
			toPush->spawningPoints.push_back(point);
			n++;
		}
		//Spawning as many spore colonies as needed
		for (pugi::xml_node building = spawningPoints.child("spore"); building; building = building.next_sibling("spore"))
		{
			int x, y;
			x = building.attribute("x").as_int();
			y = building.attribute("y").as_int();
			toPush->turrets.push_back(App->entityManager->CreateBuilding(x, y, SPORE_COLONY, COMPUTER));
		}
		//Spawning as many sunken colonies as needed
		for (pugi::xml_node building = spawningPoints.child("sunken"); building; building = building.next_sibling("sunken"))
		{
			int x, y;
			x = building.attribute("x").as_int();
			y = building.attribute("y").as_int();
			toPush->turrets.push_back(App->entityManager->CreateBuilding(x, y, SUNKEN_COLONY, COMPUTER));
		}
		//Building as many lairs as it should have
		for (pugi::xml_node lair = spawningPoints.child("lairLocation"); lair; lair = lair.next_sibling("lairLocation"))
		{
			int x, y;
			x = lair.attribute("x").as_int();
			y = lair.attribute("y").as_int();
			if (toPush->typeOfBase == ULTRALISK)
			{
				toPush->buildings.push_back(App->entityManager->CreateBuilding(x, y, HIVE, COMPUTER));
			}
			else
			{
				toPush->buildings.push_back(App->entityManager->CreateBuilding(x, y, LAIR, COMPUTER));
			}
		}
		//Spawning as many "personal" buildings as needed
		for (pugi::xml_node building = spawningPoints.child("personalBuilding"); building; building = building.next_sibling("personalBuilding"))
		{
			int x, y;
			x = building.attribute("x").as_int();
			y = building.attribute("y").as_int();
			toPush->buildings.push_back(App->entityManager->CreateBuilding(x, y, toPush->personalBuilding, COMPUTER));
		}
		//Spawning starting units
		for (int n = 0; n < startingUnits; n++)
		{
			toPush->Spawn();
		}

		//Assigning the creep layer it has
		toPush->creepOnMap = App->minimap->creep[n];

		std::vector<MapLayer*>::iterator layer = App->map->data.layers.begin();
		while (layer != App->map->data.layers.end())
		{
			if ((*layer)->properties.GetProperty("Base") == n + 1)
			{
				toPush->creep = (*layer);
				break;
			}

			layer++;
		}


		basesList.push_back(toPush);
		//Moving the "spawning points" node to the next base position in the xml
		spawningPoints = spawningPoints.next_sibling("location");

	}
#pragma endregion

	timer.Start();
	baseUpdateSpacing = BASE_UPDATE_DELAY / (float)basesList.size();
	baseToInicialize = 0;
	bossPhase = false;
	bossDefeated = false;

	return ret;
}

bool M_IA::Update(float dt)
{
	if (baseToInicialize < basesList.size())
	{
		if (timer.ReadSec() >= baseUpdateSpacing)
		{
			basesList.at(baseToInicialize)->updateDelay.Start();
			timer.Start();
			baseToInicialize++;
		}
	}

	if (App->events->GetEvent(E_DEBUG_KILL_ALL_ZERGS) == EVENT_DOWN)
	{
		for (int i = 0; i < App->entityManager->buildingList.size(); i++)
		{
			Building* building = &App->entityManager->buildingList[i];
			if (building->race == ZERG)
			{
				building->Hit(100000);
			}
		}

		for (int i = 0; i < App->entityManager->unitList.size(); i++)
		{
			if (App->entityManager->unitList[i].race == ZERG)
			{
				App->entityManager->unitList[i].Hit(100000);
			}
		}
		
	}

	std::vector<Base*>::iterator it = basesList.begin();
	while (it != basesList.end())
	{

#pragma region Debug Draw
		if (App->entityManager->debug)
		{
			int n = 1;
			std::vector<iPoint>::iterator spawnPoints = (*it)->spawningPoints.begin();
			while (spawnPoints != (*it)->spawningPoints.end())
			{
				iPoint pos(*spawnPoints);
				App->render->AddLine(pos.x - 20, pos.y - 20, pos.x + 20, pos.y + 20, true, 0, 0, 255);
				App->render->AddLine(pos.x + 20, pos.y - 20, pos.x - 20, pos.y + 20, true, 0, 0, 255);
				for (int m = 0; m < n; m++)
				{
					App->render->AddDebugRect({ pos.x + m%3 * 3, pos.y + 6 * (m/3), 1, 4 }, true, 0, 255, 0);
				}
				spawnPoints++;
				n++;
			}

		}
#pragma endregion
		//If the base return false, it was erradicated and can be removed from the list
		if ((*it)->BaseUpdate(dt) == false)
		{
			std::vector<Base*>::iterator it2 = it;
			it2++;
			LOG("%s was erradicated :D", (*it)->name.GetString());
			delete (*it);
			basesList.erase(it);
			if (it == basesList.end())
			{
				break;
			}
			it = it2;
		}
		else
		{
			it++;
		}
	}

	if (boss)
	{
		if (boss->GetMovementState() == MOVEMENT_IDLE)
		{
		//	boss->Move(iPoint(28, 159), ATTACK_ATTACK, PRIORITY_MEDIUM);
		}
		else if (boss->GetMovementState() == MOVEMENT_DEAD || boss->GetMovementState() == MOVEMENT_DIE || boss->GetState() == STATE_DIE)
		{
			boss = NULL;
			bossDefeated = true;
			std::vector<Base*>::iterator it = basesList.begin();
				while (it != basesList.end())
				{
					(*it)->Kill();
					it++;
				}
		}
	}

	return true;
}

bool M_IA::CleanUp()
{
	//Cleaning Bases
	std::vector<Base*>::iterator it = basesList.begin();
	LOG("Erasing bases");
	while (it != basesList.end())
	{
		delete (*it);
		basesList.erase(it);
	}

	return true;
}


void M_IA::StartBossPhase()
{
	boss = App->entityManager->CreateUnit(2681, 464, KERRIGAN, COMPUTER);
	boss->Move(iPoint(28, 159), ATTACK_ATTACK, PRIORITY_LOW);
	bossPhase = true;

	std::vector<Base*>::iterator it = basesList.begin();
	std::list<Unit*>::iterator unit;
	while (it != basesList.end())
	{
		unit = (*it)->unitsInBase.begin();
		while (unit != (*it)->unitsInBase.end() && (*unit)->GetState() != STATE_DIE)
		{
			(*unit)->Move(iPoint(28, 159), ATTACK_ATTACK, PRIORITY_LOW);
			unit++;
		}

		unit = (*it)->unitsOutOfBase.begin();
		while (unit != (*it)->unitsOutOfBase.end())
		{
			(*unit)->Move(iPoint(28, 159), ATTACK_ATTACK, PRIORITY_LOW);
			unit++;
		}

		it++;
	}
}