#include "M_IA.h"
#include "j1App.h"
#include "M_FileSystem.h"
#include "M_Map.h"
#include "M_PathFinding.h"
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
	return ret;
}

void Base::CheckBaseUnits()
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
	std::list<Unit*>::iterator it = unitsInBase.begin();
	std::list<Unit*>::iterator it2 = it;
	int n = 0;
	if (spawningPoints.size() > 0)
	{
		n = rand() % spawningPoints.size();
	}
	while (it != unitsInBase.end())
	{
		if ((*it)->GetState() == STATE_STAND)
		{
			iPoint ZergPos((*it)->GetPosition().x, (*it)->GetPosition().y);
			if (ZergPos.DistanceManhattan(spawningPoints[n % spawningPoints.size()]) > 800)
			{
				iPoint toSend = App->pathFinding->WorldToMap(spawningPoints[n % spawningPoints.size()].x, spawningPoints[n % spawningPoints.size()].y);
				(*it)->Move(toSend, ATTACK_ATTACK, PRIORITY_LOW);
				n++;
			}
		}
		it++;
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
}


//Zergling Base -------------------------------------------------------------------------------------------------------------
Base_Zergling::Base_Zergling() : Base("Zergling base")
{
	buildings.push_back(App->entityManager->CreateBuilding(27, 12, LAIR, COMPUTER));
	
	typeOfBase = ZERGLING;
}

bool Base_Zergling::PersonalUpdate()
{
	if (sentUnits)
	{
		generationDelay -= generationDelay/30.0f;
		CAP(generationDelay, 30, 500);
		int toIncrease = unitsToSend + unitsToSend / 4;
		baseUnitsReactN += toIncrease;
		CAP(baseUnitsReactN, 2, 28);
		unitsToSend += toIncrease;
		CAP(unitsToSend, 1, 15);
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
	buildings.push_back(App->entityManager->CreateBuilding(175, 135, LAIR, COMPUTER));
	typeOfBase = HYDRALISK;
}

bool Base_Hydralisk::PersonalUpdate()
{
	if (sentUnits)
	{
		generationDelay = unitsInBase.size() * 10;
		if (generationDelay < 40)
		{
			generationDelay = 40;
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
			if ((*it)->GetState() == STATE_STAND)
			{
				(*it)->Move(toSend, ATTACK_ATTACK, PRIORITY_LOW);
			}
			it++;
		}
		App->IA->aZergDied = false;
	}
}


//Mutalisk Base -------------------------------------------------------------------------------------------------------------
Base_Mutalisk::Base_Mutalisk() : Base("Mutalisk base")
{
	buildings.push_back(App->entityManager->CreateBuilding(102, 76, LAIR, COMPUTER));
	typeOfBase = MUTALISK;
}

bool Base_Mutalisk::PersonalUpdate()
{
	if (sentUnits && generationDelay > 70)
	{
		generationDelay -= 7;
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


//Ultralisk Base -------------------------------------------------------------------------------------------------------------
Base_Ultralisk::Base_Ultralisk() : Base("Ultralisk base")
{
	buildings.push_back(App->entityManager->CreateBuilding(140, 38, LAIR, COMPUTER));
	buildings.push_back(App->entityManager->CreateBuilding(153, 35, LAIR, COMPUTER));
	buildings.push_back(App->entityManager->CreateBuilding(151, 43, LAIR, COMPUTER));
	typeOfBase = ULTRALISK;
}

bool Base_Ultralisk::PersonalUpdate()
{
	if (sentUnits)
	{
		generationDelay -= generationDelay / 7.0f;
		CAP(generationDelay, 90, 500);
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

	Base_Zergling* Zerg = new Base_Zergling();
	Base_Hydralisk* Hydra = new Base_Hydralisk();
	Base_Mutalisk* Muta = new Base_Mutalisk();
	Base_Ultralisk* Ultra = new Base_Ultralisk();

	basesList.push_back(Zerg);
	basesList.push_back(Hydra);
	basesList.push_back(Muta);
	basesList.push_back(Ultra);

#pragma region Loading xml data
	bool ret = true;
	char* buf;
	int size = App->fs->Load("ZergBases.xml", &buf);
	pugi::xml_document file;
	pugi::xml_parse_result result = file.load_buffer(buf, size);

	RELEASE_ARRAY(buf);

	if (result == NULL)
	{
		LOG("Could not load Zerg Bases file %s. pugi error: %s", "entityManager / Sprite data.tmx", result.description());
		return false;
	}

	pugi::xml_node node;
	for (node = file.child("bases").child("base"); node && ret; node = node.next_sibling("base"))
	{
		bool found = true;
		C_String tmp = node.child("name").attribute("value").as_string();
		std::vector<Base*>::iterator it = basesList.begin();
		while ((*it)->name != tmp)
		{
			it++;
			if (it == basesList.end())
			{
				LOG("Tried to load an unexisting base");
				found = false;
				break;
			}
		}
		if (found)
		{
			int startingUnits = node.child("startingUnits").attribute("value").as_int();
			(*it)->generationDelay = node.child("generationTimer").attribute("value").as_float();
			(*it)->baseUnitsReactN = node.child("reactUnitsN").attribute("value").as_int();
			(*it)->unitsToSend = node.child("unitsToSend").attribute("value").as_int();

			pugi::xml_node spawningPoints;
			for (spawningPoints = node.child("spawningPoint"); spawningPoints && ret; spawningPoints = spawningPoints.next_sibling("spawningPoint"))
			{
				iPoint point;
				point.x = spawningPoints.attribute("x").as_int();
				point.y = spawningPoints.attribute("y").as_int();
				(*it)->spawningPoints.push_back(point);
			}
			
			for (int n = 0; n < startingUnits; n++)
			{
				(*it)->Spawn();
			}
		}
	}
#pragma endregion

	timer.Start();
	baseUpdateSpacing = BASE_UPDATE_DELAY / (float)basesList.size();
	baseToInicialize = 0;

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


	std::vector<Base*>::iterator it = basesList.begin();
	while (it != basesList.end())
	{

#pragma region Debug Draw
		if (App->entityManager->debug)
		{
			std::vector<iPoint>::iterator spawnPoints = (*it)->spawningPoints.begin();
			while (spawnPoints != (*it)->spawningPoints.end())
			{
				iPoint pos(*spawnPoints);
				App->render->AddLine(pos.x - 20, pos.y - 20, pos.x + 20, pos.y + 20, true, 255, 0, 0);
				App->render->AddLine(pos.x + 20, pos.y - 20, pos.x - 20, pos.y + 20, true, 255, 0, 0);
				spawnPoints++;
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