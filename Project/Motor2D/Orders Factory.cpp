#include "Orders Factory.h"

#include "Log.h"
#include "j1App.h"
#include "M_EntityManager.h"

void Set_rallyPoint::Function()
{
	LOG("Setting Rally Point!");
}
void Gen_probe::Function()
{
	/*
	Maybe
	ordersMod->GenerateUnit(Zergling);
	*/
	//ordersMod->Generate_Zergling();
	LOG("Generate Probe");
}

void Move::Function()
{
	LOG("Executing Move");
}

void Stop::Function()
{
	LOG("Executing Stop");
}

void Attack::Function()
{
	LOG("Executing Attack");
}

void Hold_pos::Function()
{
	LOG("Executing Hold_pos");
}

void Gather::Function()
{
	LOG("Executing Gather");
}


void Patrol::Function()
{
	LOG("Executing Patrol");
}