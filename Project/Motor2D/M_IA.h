#ifndef __IA_H__
#define __IA_H__

#include "j1Module.h"
#include "j1Timer.h"

#define BASE_UPDATE_DELAY 2.0f
#define N_OF_RANDOM_BASES 3

enum Unit_Type;
enum Building_Type;
class Unit;
class Building;
struct MapLayer;
class UI_Image;

class Base
{
public:
	Base(char* _name) { name = _name; generationTimer.Start(); updateDelay.Start(); }

	bool BaseUpdate(float dt);
	void Spawn();
	void Kill();

private:
	virtual bool PersonalUpdate();

	bool IsBaseAlive();

	void CheckBaseUnits();
	virtual void UpdateOutOfBaseUnits();
	void ClearDeadUnits();

	bool changingCreepOpacity = false;

public:
	C_String name;
//private:
	MapLayer* creep = NULL;
	UI_Image* creepOnMap = NULL;

	std::list<Unit*>		unitsInBase;
	std::list<Unit*>		unitsOutOfBase;
	std::list<Building*>	buildings;
	std::list<Building*>	turrets;

	bool defeated = false;
	bool  spawning = true;
	bool  sentUnits = false;
	j1Timer generationTimer;
	float generationDelay = 60.0f;
	j1Timer updateDelay;
	int nOfSpawningPoints = 5;
	int baseN = 0;

	std::vector<iPoint> spawningPoints;
	uint whereToSpawn = 0;
	Unit_Type typeOfBase;
	Building_Type personalBuilding ;
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

class Base_Terran : public Base
{
public:
	Base_Terran();
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

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void StartBossPhase();

public:
	std::vector<Base*> basesList;

	fPoint lastDeath;
	bool aZergDied = false;

	float baseUpdateSpacing = 1.0f;
	int baseToInicialize = 0;
	j1Timer timer;

	bool bossDefeated = false;
	bool bossPhase = false;
	bool createBoss = false;

	Unit* boss = NULL;

	int nBases = 4;

};

#endif 