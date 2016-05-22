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
	if (whereToSpawn >= nOfSpawningPoints)
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
	boss = NULL;
	App->entityManager->muteUnitsSounds = true;
#pragma region //Random base selection

	CAP(nBases, 2, 4);
	int* bases = new int[nBases];
	std::vector<int> possibles = { 2, 3, 1, 0 };

	srand(time(NULL));
	for (int n = 1; n < nBases; n++)
	{
		std::vector<int>::iterator it;
		int random = rand() % possibles.size();
		it = possibles.begin();
		for (int m = 0; m < random; m++)
		{
			it++;
		}
		bases[n] = possibles[random];
		if (possibles[random] == 0 || possibles[random] == 3)
		{
			possibles.erase(it);
		}
	}
	bases[rand() % (nBases - 1) + 1] = 0;
	bases[0] = 4;


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

	std::vector<MapLayer*>::iterator l = App->map->data.layers.begin();
	while (l != App->map->data.layers.end())
	{
		if ((*l)->properties.GetProperty("Base") != -1)
		{
			(*l)->opacity = 0;
			break;
		}
		l++;
	}


	srand(time(NULL));
	pugi::xml_node mainNode = file.child("bases");
	pugi::xml_node spawningPoints = file.child("bases").child("location");
	for (int n = 0; n < nBases; n++)
	{
		//Generating a random number that will decide the types of base that will spawn
		C_String baseType;
		if (n < nBases && n > 0)
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
		toPush->nOfSpawningPoints= node.child("spawnPoints").attribute("value").as_int();
		toPush->generationDelay = node.child("generationTimer").attribute("value").as_float();
		toPush->baseUnitsReactN = node.child("reactUnitsN").attribute("value").as_int();
		toPush->unitsToSend = node.child("unitsToSend").attribute("value").as_int();
		toPush->baseN = n;

		//Setting as many spawpoints for that base as it should have
		pugi::xml_node thisBaseSpawningPoints = spawningPoints.child("spawningPoint");
		for (int n = 0; thisBaseSpawningPoints; thisBaseSpawningPoints = thisBaseSpawningPoints.next_sibling("spawningPoint"))
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
		toPush->creepOnMap->SetActive(true);

		std::vector<MapLayer*>::iterator layer = App->map->data.layers.begin();
		while (layer != App->map->data.layers.end())
		{
			if ((*layer)->properties.GetProperty("Base") == n + 1)
			{
				toPush->creep = (*layer);
				toPush->creep->opacity = 255;
				break;
			}

			layer++;
		}


		basesList.push_back(toPush);
		//Moving the "spawning points" node to the next base position in the xml
		spawningPoints = spawningPoints.next_sibling("location");

	}
#pragma endregion

	delete bases;

	timer.Start();
	baseUpdateSpacing = BASE_UPDATE_DELAY / (float)basesList.size();
	baseToInicialize = 0;
	bossPhase = false;
	bossDefeated = false;
	App->entityManager->muteUnitsSounds = false;
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
			if (App->entityManager->buildingList[i].dead == false && App->entityManager->buildingList[i].race == ZERG)
			{
				App->entityManager->buildingList[i].Hit(100000);
			}
		}

		for (int i = 0; i < App->entityManager->unitList.size(); i++)
		{
			if (App->entityManager->unitList[i].dead == false && App->entityManager->unitList[i].race == ZERG)
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

// Load Game State
bool M_IA::Load(pugi::xml_node& data)
{
	App->entityManager->muteUnitsSounds = true;

	bossPhase = false;
	boss = NULL;

	std::vector<Base*>::iterator base = basesList.begin();
	while (base != basesList.end())
	{
		(*base)->unitsInBase.clear();
		(*base)->unitsOutOfBase.clear();
		(*base)->buildings.clear();
		(*base)->turrets.clear();
		delete(*base);
		base++;
	}
	basesList.clear();

	//Ressetting the creep for non-existing bases
	for (int n = 0; n < 4; n++)
	{
		App->minimap->creep[n]->SetActive(false);

		std::vector<MapLayer*>::iterator layer = App->map->data.layers.begin();
		while (layer != App->map->data.layers.end())
		{
			if ((*layer)->properties.GetProperty("Base") == n + 1)
			{
				(*layer)->opacity = 0;
				break;
			}
			layer++;
		}
	}

	for (pugi::xml_node base = data.child("base"); base; base = base.next_sibling("base"))
	{
		int baseN = base.attribute("baseN").as_int();
		Unit_Type type = static_cast<Unit_Type>(base.attribute("type").as_int());
		Base* toPush = NULL;
		switch (type)
		{
		case (MUTALISK) :
		{
			toPush = new Base_Mutalisk;
			break;
		}
		case (HYDRALISK) :
		{
			toPush = new Base_Hydralisk;
			break;
		}
		case (INFESTED_TERRAN) :
		{
			toPush = new Base_Terran;
			break;
		}
		case (ULTRALISK) :
		{
			toPush = new Base_Ultralisk;
			break;
		}
		default:
		{
			toPush = new Base_Zergling;
		}
		}
		toPush->baseUnitsReactN = base.attribute("reactN").as_int();
		toPush->unitsToSend = base.attribute("toSendN").as_int();
		toPush->generationDelay = base.attribute("genDelay").as_int();
		toPush->nOfSpawningPoints = base.attribute("nOfSpawningPoints").as_int();
		for (pugi::xml_node point = base.child("spawningPoint"); point; point = point.next_sibling("spawningPoint"))
		{
			int x = point.attribute("x").as_int();
			int y = point.attribute("y").as_int();
			toPush->spawningPoints.push_back({ x, y });
		}
		for (pugi::xml_node unit = base.child("inBase"); unit; unit = unit.next_sibling("inBase"))
		{
			int x = unit.attribute("x").as_int();
			int y = unit.attribute("y").as_int();
			Unit_Type type = static_cast<Unit_Type>(unit.attribute("type").as_int());
			int hp = unit.attribute("HP").as_int();
			Unit* tmp = App->entityManager->CreateUnit(x, y, type, COMPUTER);
			tmp->currHP = hp;
			toPush->unitsInBase.push_back(tmp);
		}
		for (pugi::xml_node unit = base.child("outOfBase"); unit; unit = unit.next_sibling("outOfBase"))
		{
			int x = unit.attribute("x").as_int();
			int y = unit.attribute("y").as_int();
			Unit_Type type = static_cast<Unit_Type>(unit.attribute("type").as_int());
			int hp = unit.attribute("HP").as_int();
			Unit* tmp = App->entityManager->CreateUnit(x, y, type, COMPUTER);
			tmp->currHP = hp;
			toPush->unitsOutOfBase.push_back(tmp);
		}
		for (pugi::xml_node turret = base.child("turret"); turret; turret = turret.next_sibling("turret"))
		{
			int x = turret.attribute("x").as_int();
			int y = turret.attribute("y").as_int();
			Building_Type type = static_cast<Building_Type>(turret.attribute("type").as_int());
			int hp = turret.attribute("HP").as_int();
			Building* tmp = App->entityManager->CreateBuilding(x, y, type, COMPUTER, true);
			tmp->currHP = hp;
			toPush->turrets.push_back(tmp);
		}
		for (pugi::xml_node building = base.child("building"); building; building = building.next_sibling("building"))
		{
			int x = building.attribute("x").as_int();
			int y = building.attribute("y").as_int();
			Building_Type type = static_cast<Building_Type>(building.attribute("type").as_int());
			int hp = building.attribute("HP").as_int();
			Building* tmp = App->entityManager->CreateBuilding(x, y, type, COMPUTER, true);
			tmp->currHP = hp;
			toPush->buildings.push_back(tmp);
		}


		//Assigning the creep layer it has
		toPush->creepOnMap = App->minimap->creep[baseN];
		if (toPush->spawning)
		{
			toPush->creepOnMap->SetActive(true);
		}

		std::vector<MapLayer*>::iterator layer = App->map->data.layers.begin();
		while (layer != App->map->data.layers.end())
		{
			if ((*layer)->properties.GetProperty("Base") == baseN + 1)
			{
				toPush->creep = (*layer);
				if (toPush->spawning)
				{
					(*layer)->opacity = 255;
				}				
				break;
			}

			layer++;
		}


		basesList.push_back(toPush);
	}

	App->entityManager->muteUnitsSounds = false;
	return true;
}

// Save Game State
bool M_IA::Save(pugi::xml_node& data) const
{
	std::vector<Base*>::const_iterator base = basesList.cbegin();
	while (base != basesList.cend())
	{
		pugi::xml_node baseNode = data.append_child("base");
		baseNode.append_attribute("baseN") = (*base)->baseN;
		baseNode.append_attribute("type") = (*base)->typeOfBase;
		baseNode.append_attribute("reactN") = (*base)->baseUnitsReactN;
		baseNode.append_attribute("toSendN") = (*base)->unitsToSend;
		baseNode.append_attribute("genDelay") = (*base)->generationDelay;
		baseNode.append_attribute("nOfSpawningPoints") = (*base)->nOfSpawningPoints;

		std::vector<iPoint>::const_iterator spawnPoints = (*base)->spawningPoints.cbegin();
		while (spawnPoints != (*base)->spawningPoints.cend())
		{
			pugi::xml_node point = baseNode.append_child("spawningPoint");
			point.append_attribute("x") = (*spawnPoints).x;
			point.append_attribute("y") = (*spawnPoints).y;
			spawnPoints++;
		}

		std::list<Unit*>::const_iterator inBase = (*base)->unitsInBase.cbegin();
		while (inBase != (*base)->unitsInBase.cend())
		{
			pugi::xml_node tmp = baseNode.append_child("inBase");
			tmp.append_attribute("name") = (*inBase)->name.GetString();

			tmp.append_attribute("x") = (*inBase)->GetPosition().x;
			tmp.append_attribute("y") = (*inBase)->GetPosition().y;
			tmp.append_attribute("type") = (*inBase)->stats.type;

			tmp.append_attribute("HP") = (*inBase)->currHP;
			inBase++;
		}

		std::list<Unit*>::const_iterator outOfBase = (*base)->unitsOutOfBase.cbegin();
		while (outOfBase != (*base)->unitsOutOfBase.cend())
		{
			pugi::xml_node tmp = baseNode.append_child("outOfBase");
			tmp.append_attribute("name") = (*outOfBase)->name.GetString();

			tmp.append_attribute("x") = (*outOfBase)->GetPosition().x;
			tmp.append_attribute("y") = (*outOfBase)->GetPosition().y;
			tmp.append_attribute("type") = (*outOfBase)->stats.type;

			tmp.append_attribute("HP") = (*outOfBase)->currHP;
			outOfBase++;
		}

		std::list<Building*>::const_iterator turrets = (*base)->turrets.cbegin();
		while (turrets != (*base)->turrets.cend())
		{
			pugi::xml_node tmp = baseNode.append_child("turret");
			tmp.append_attribute("name") = (*turrets)->name.GetString();

			tmp.append_attribute("x") = (*turrets)->GetPosition().x;
			tmp.append_attribute("y") = (*turrets)->GetPosition().y;
			tmp.append_attribute("type") = (*turrets)->GetType();

			tmp.append_attribute("HP") = (*turrets)->currHP;
			turrets++;
		}

		std::list<Building*>::const_iterator buildings = (*base)->buildings.cbegin();
		while (buildings != (*base)->buildings.cend())
		{
			pugi::xml_node tmp = baseNode.append_child("building");
			tmp.append_attribute("name") = (*buildings)->name.GetString();

			tmp.append_attribute("x") = (*buildings)->GetPosition().x;
			tmp.append_attribute("y") = (*buildings)->GetPosition().y;
			tmp.append_attribute("type") = (*buildings)->GetType();

			tmp.append_attribute("HP") = (*buildings)->currHP;
			buildings++;
		}
		base++;
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