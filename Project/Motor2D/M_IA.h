#ifndef __IA_H__
#define __IA_H__

#include "j1Module.h"
#include "M_EntityManager.h"
#include "Unit.h"
#include "Building.h"
#include "j1Timer.h"

class Base
{
public:
	Base(char* _name) { name = _name; generationTimer.Start(); updateDelay.Start(); }

	bool BaseUpdate(float dt);
	void Spawn();

private:
	virtual bool PersonalUpdate();

	bool IsBaseAlive();

	void CheckBaseUnits();
	virtual void UpdateOutOfBaseUnits();
	void ClearDeadUnits();

public:
	C_String name;
//private:
	std::list<Unit*>		unitsInBase;
	std::list<Unit*>		unitsOutOfBase;
	std::list<Building*>	buildings;

	bool defeated = false;
	bool  spawning = true;
	bool  sentUnits = false;
	j1Timer generationTimer;
	float generationDelay = 60.0f;
	j1Timer updateDelay;

	std::vector<iPoint> spawningPoints;
	uint whereToSpawn = 0;
	Unit_Type typeOfBase = ZERGLING;
	//N of units when the base will send some to the "out of base" list
	int baseUnitsReactN = 20;
	//N of units sent to the "out of base" list each time
	int unitsToSend = 10;
};

class Base_Zergling : public Base
{
public:
	Base_Zergling();
protected:
	bool PersonalUpdate();
	void UpdateOutOfBaseUnits();
};

class Base_Hydralisk : public Base
{
public:
	Base_Hydralisk();
protected:
	std::list<Unit*>		attackingUnits;
	bool PersonalUpdate();
	void UpdateOutOfBaseUnits();
};

class Base_Mutalisk : public Base
{
public:
	Base_Mutalisk();
protected:
	bool PersonalUpdate();
	void UpdateOutOfBaseUnits();
};

class Base_Ultralisk : public Base
{
public:
	Base_Ultralisk();
protected:
	bool PersonalUpdate();
	void UpdateOutOfBaseUnits();
};

class M_IA : public j1Module
{
public:
	M_IA(bool);
	~M_IA(){};

	bool Start();

	bool Update(float dt);

	bool CleanUp();


public:
	std::vector<Base*> basesList;

	fPoint lastDeath;
	bool aZergDied = false;

};

#endif 