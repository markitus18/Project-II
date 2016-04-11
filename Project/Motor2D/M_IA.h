#ifndef __IA_H__
#define __IA_H__

#include "j1Module.h"
#include "M_EntityManager.h"
#include "Unit.h"
#include "Building.h"

class Base
{
public:
	bool BaseUpdate(float dt);

private:
	virtual bool PersonalUpdate(float dt);

	void Spawn();
	bool IsBaseAlive();

	void CheckBaseUnits();
	virtual void UpdateOutOfBaseUnits();

	std::list<Unit*>		unitsInBase;
	std::list<Unit*>		unitsOutOfBase;
	std::list<Building*>	buildings;

	bool  spawning;
	float generationTimer;
	float generationDelay;

	iPoint spawningPoint;
	Unit_Type typeOfBase = ZERGLING;
	int BaseUnitsReactN = 5;
};

class M_IA : public j1Module
{
public:
	M_IA(bool);
	~M_IA(){};

	bool Start();

	bool Update(float dt);

	bool CleanUp();

private:

	std::vector<Base> basesList;

};

#endif 