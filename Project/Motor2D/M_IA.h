#ifndef __IA_H__
#define __IA_H__

#include "j1Module.h"
#include "M_EntityManager.h"
#include "Unit.h"
#include "Building.h"

class Base
{
public:
	Base(char* _name) { name = _name; }

	bool BaseUpdate(float dt);

private:
	virtual bool PersonalUpdate(float dt);

	void Spawn();
	bool IsBaseAlive();

	void CheckBaseUnits();
	virtual void UpdateOutOfBaseUnits();

public:
	char* name;
private:
	std::list<Unit*>		unitsInBase;
	std::list<Unit*>		unitsOutOfBase;
	std::list<Building*>	buildings;

	bool  spawning = true;
	float generationTimer = 0.0f;
	float generationDelay = 60.0f;

	iPoint spawningPoint;
	Unit_Type typeOfBase = ZERGLING;
	int BaseUnitsReactN = 5;
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

	bool Awake(pugi::xml_node&);

	bool Start();

	bool Update(float dt);

	bool CleanUp();

private:

	std::vector<Base*> basesList;

};

#endif 