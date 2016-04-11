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
	virtual bool PersonalUpdate(float dt);

	bool IsBaseAlive();

	void CheckBaseUnits();
	virtual void UpdateOutOfBaseUnits();

public:
	C_String name;
//private:
	std::list<Unit*>		unitsInBase;
	std::list<Unit*>		unitsOutOfBase;
	std::list<Building*>	buildings;

	bool  spawning = true;
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
	bool PersonalUpdate(float dt);
	void UpdateOutOfBaseUnits();
};

class Base_Hydralisk : public Base
{
public:
	Base_Hydralisk();
protected:
	bool PersonalUpdate(float dt);
	void UpdateOutOfBaseUnits();
};

class Base_Mutalisk : public Base
{
public:
	Base_Mutalisk();
protected:
	bool PersonalUpdate(float dt);
	void UpdateOutOfBaseUnits();
};

class Base_Ultralisk : public Base
{
public:
	Base_Ultralisk();
protected:
	bool PersonalUpdate(float dt);
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

private:

	std::vector<Base*> basesList;

};

#endif 