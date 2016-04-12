#include "M_IA.h"
#include "j1App.h"
#include "M_FileSystem.h"
#include "M_Map.h"
#include "M_PathFinding.h"
#include <ctime>

//General Base ---------------------------------------------------------------------------------------------------------------

bool Base::BaseUpdate(float dt)
{
	if (updateDelay.ReadSec() > 1.0f)
	{
		if (spawning)
		{
			if (generationTimer.ReadSec() >= generationDelay)
			{
				generationTimer.Start();
				Spawn();
			}
		}
		ClearDeadUnits();
		CheckBaseUnits();
		UpdateOutOfBaseUnits();
	}
	PersonalUpdate(dt);
	return IsBaseAlive();
}

bool Base::PersonalUpdate(float dt)
{
	return true;
}

void Base::Spawn()
{
	Unit* tmp = App->entityManager->CreateUnit(spawningPoints[whereToSpawn].x, spawningPoints[whereToSpawn].y, typeOfBase, COMPUTER);
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
	//	spawning = false;
	}

	if (!ret && !unitsInBase.empty())
	{
		ret = true;
	}
	else
	{
		spawning = false;
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
	if (unitsInBase.size() >= baseUnitsReactN)
	{
		std::list<Unit*>::iterator it = unitsInBase.begin();
		std::list<Unit*>::iterator it2 = it;
		for (int n = 0; n < unitsToSend; n++)
		{
			it2 = it;
			it2++;

			unitsOutOfBase.push_back((*it));
			unitsInBase.erase(it);

			it = it2;
		}
		LOG("%s has %i units, sending %i out. There are %i units out of base now.", name.GetString(), unitsInBase.size() + unitsToSend, unitsToSend, unitsOutOfBase.size());
		sentUnits = true;
	}
}

void Base::UpdateOutOfBaseUnits()
{

}

void Base::ClearDeadUnits()
{
	std::list<Unit*>::iterator itOut = unitsOutOfBase.begin();
	while (itOut != unitsOutOfBase.end())
	{
		if ((*itOut)->GetState() == STATE_DIE)
		{
			unitsOutOfBase.erase(itOut);
		}
		else
		{
			itOut++;
		}
	}
	std::list<Unit*>::iterator itIn = unitsInBase.begin();
	while (itIn != unitsInBase.end())
	{
		if ((*itIn)->GetState() == STATE_DIE)
		{
			unitsOutOfBase.erase(itIn);
		}
		else
		{
			itIn++;
		}
	}
}

//Zergling Base -------------------------------------------------------------------------------------------------------------
Base_Zergling::Base_Zergling() : Base("Zergling base")
{
	typeOfBase = ZERGLING;
}

bool Base_Zergling::PersonalUpdate(float dt)
{
	if (sentUnits)
	{
		generationDelay -= generationDelay/50.0f;
		int toIncrease = unitsToSend + unitsToSend / 4;
		baseUnitsReactN += toIncrease;
		unitsToSend += toIncrease;
	}
	return true;
}

void Base_Zergling::UpdateOutOfBaseUnits()
{
	std::list<Unit*>::iterator it = unitsOutOfBase.begin();
	while (it != unitsOutOfBase.end())
	{
		if ((*it)->GetState() == STATE_STAND)
		{
			(*it)->Move(iPoint(34, 167), ATTACK_ATTACK);
		}
		it++;
	}
}


//Hydralisk Base -------------------------------------------------------------------------------------------------------------
Base_Hydralisk::Base_Hydralisk() : Base("Hydralisk base")
{
	typeOfBase = HYDRALISK;
}

bool Base_Hydralisk::PersonalUpdate(float dt)
{
	return true;
}

void Base_Hydralisk::UpdateOutOfBaseUnits()
{

}


//Mutalisk Base -------------------------------------------------------------------------------------------------------------
Base_Mutalisk::Base_Mutalisk() : Base("Mutalisk base")
{
	typeOfBase = MUTALISK;
}

bool Base_Mutalisk::PersonalUpdate(float dt)
{
	return true;
}

void Base_Mutalisk::UpdateOutOfBaseUnits()
{
	srand(time(NULL));
	std::list<Unit*>::iterator it = unitsOutOfBase.begin();
	while (it != unitsOutOfBase.end())
	{
		if ((*it)->GetState() == STATE_STAND)
		{
			iPoint toMove;
			toMove.x = (rand() % (App->pathFinding->width - 10)) + 5;
			toMove.y = ((rand() + rand()) % (App->pathFinding->height - 10)) + 5;
			(*it)->Move(toMove, ATTACK_ATTACK);
		}
		it++;
	}
}


//Ultralisk Base -------------------------------------------------------------------------------------------------------------
Base_Ultralisk::Base_Ultralisk() : Base("Ultralisk base")
{
	typeOfBase = ULTRALISK;
}

bool Base_Ultralisk::PersonalUpdate(float dt)
{
	if (sentUnits)
	{
		generationDelay -= generationDelay / 70.0f;
		int toIncrease = 2;
		baseUnitsReactN += toIncrease;
		unitsToSend += toIncrease;
	}
	return true;
}

void Base_Ultralisk::UpdateOutOfBaseUnits()
{
	std::list<Unit*>::iterator it = unitsOutOfBase.begin();
	while (it != unitsOutOfBase.end())
	{
		if ((*it)->GetState() == STATE_STAND)
		{
			(*it)->Move(iPoint(34, 167), ATTACK_ATTACK);
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

	return ret;
}

bool M_IA::Update(float dt)
{
	std::vector<Base*>::iterator it = basesList.begin();
	while (it != basesList.end())
	{
		//Debug Draw
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

		if ((*it)->BaseUpdate(dt) == false)
		{
			LOG("%s was erradicated :D", (*it)->name.GetString());
			if (*it)
			{
				delete (*it);
			}
			basesList.erase(it);
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
		if (*it)
		{
			delete (*it);
		}
		basesList.erase(it);
	}

	return true;
}