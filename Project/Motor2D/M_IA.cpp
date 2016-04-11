#include "M_IA.h"
#include "j1App.h"

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
	return IsBaseAlive();
}

bool Base::PersonalUpdate(float dt)
{
	return true;
}

void Base::Spawn()
{
	Unit* tmp = App->entityManager->CreateUnit(spawningPoint.x, spawningPoint.y, typeOfBase, COMPUTER);
	unitsInBase.push_back(tmp);
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
			it2 =
			it++;
		}
	}
}

void Base::UpdateOutOfBaseUnits()
{

}






//Module IA ------------------------------------------------------------------------------------------------------------------

M_IA::M_IA(bool start_enabled) : j1Module(start_enabled)
{
	name.create("IA");
}

bool M_IA::Start()
{
	return true;
}

bool M_IA::Update(float dt)
{
	std::vector<Base>::iterator it = basesList.begin();
	while (it != basesList.end())
	{
		if (it->BaseUpdate(dt) == false)
		{
			LOG("A base was erradicated :D");
			std::vector<Base>::iterator it2 = it;
			it2++;
			basesList.erase(it);
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
	return true;
}