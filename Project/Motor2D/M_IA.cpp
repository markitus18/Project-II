#include "M_IA.h"
#include "j1App.h"
#include "M_FileSystem.h"

//General Base ---------------------------------------------------------------------------------------------------------------

bool Base::BaseUpdate(float dt)
{
	if (spawning)
	{
		generationTimer += dt;
		if (generationTimer >= generationDelay)
		{
			generationTimer = 00.f;
			Spawn();
		}
	}
	PersonalUpdate(dt);
	CheckBaseUnits();
	UpdateOutOfBaseUnits();
	return IsBaseAlive();
}

bool Base::PersonalUpdate(float dt)
{
	return true;
}

void Base::Spawn()
{
//	Unit* tmp = App->entityManager->CreateUnit(spawningPoint.x, spawningPoint.y, typeOfBase, COMPUTER);
//	unitsInBase.push_back(tmp);
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
	if (!ret && !unitsOutOfBase.empty())
	{
		ret = true;
	}
	return ret;
}

void Base::CheckBaseUnits()
{
	if (unitsInBase.size() > BaseUnitsReactN)
	{
		std::list<Unit*>::iterator it = unitsInBase.begin();
		std::list<Unit*>::iterator it2 = it;
		for (int n = 0; n < BaseUnitsReactN; n++)
		{
			it2 = it;
			it2++;

			unitsOutOfBase.push_back((*it));
			unitsInBase.erase(it);

			it = it2;
		}
	}
}

void Base::UpdateOutOfBaseUnits()
{

}


//Zergling Base -------------------------------------------------------------------------------------------------------------
Base_Zergling::Base_Zergling() : Base("Zergling base")
{

}
bool Base_Zergling::PersonalUpdate(float dt)
{
	return true;
}
void Base_Zergling::UpdateOutOfBaseUnits()
{

}


//Hydralisk Base -------------------------------------------------------------------------------------------------------------
Base_Hydralisk::Base_Hydralisk() : Base("Hydralisk base")
{

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

}
bool Base_Mutalisk::PersonalUpdate(float dt)
{
	return true;
}
void Base_Mutalisk::UpdateOutOfBaseUnits()
{

}


//Ultralisk Base -------------------------------------------------------------------------------------------------------------
Base_Ultralisk::Base_Ultralisk() : Base("Ultralisk base")
{

}
bool Base_Ultralisk::PersonalUpdate(float dt)
{
	return true;
}
void Base_Ultralisk::UpdateOutOfBaseUnits()
{

}



//Module IA ------------------------------------------------------------------------------------------------------------------

M_IA::M_IA(bool start_enabled) : j1Module(start_enabled)
{
	name.create("IA");
}

bool M_IA::Awake(pugi::xml_node&)
{
	return true;
}

bool M_IA::Start()
{
/*	bool ret = true;
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
	for (node = file.child("stats").child("unit"); node && ret; node = node.next_sibling("unit"))
	{
		C_String tmp = node.child("name").attribute("value").as_string();
	//else if (tmp == "Mutalisk")
//		unitsLibrary.types.push_back(MUTALISK);
		//stats.type = node.child("type").attribute("value").as_int();
	}
	*/


	Base_Zergling* Zerg = new Base_Zergling();
	Base_Hydralisk* Hydra = new Base_Hydralisk();
	Base_Mutalisk* Muta = new Base_Mutalisk();
	Base_Ultralisk* Ultra = new Base_Ultralisk();

	basesList.push_back(Zerg);
	basesList.push_back(Hydra);
	basesList.push_back(Muta);
	basesList.push_back(Ultra);
	return true;
}

bool M_IA::Update(float dt)
{
	std::vector<Base*>::iterator it = basesList.begin();
	while (it != basesList.end())
	{
		if ((*it)->BaseUpdate(dt) == false)
		{
			LOG("%s was erradicated :D", (*it)->name);
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
		std::vector<Base*>::iterator it2 = it;
		it2++;
		RELEASE(*it);
		basesList.erase(it);
		it = it2;
	}

	return true;
}