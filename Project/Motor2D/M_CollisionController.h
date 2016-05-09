#ifndef __M_COLLISIONCONTROLLER_H__
#define __M_COLLISIONCONTROLLER_H__

#include "j1Module.h"
#include "j1Timer.h"
#include "j1PerfTimer.h"

class Unit;

class M_CollisionController : public j1Module
{

public:

	M_CollisionController(bool);

	// Destructor
	virtual ~M_CollisionController();

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	void ManageInput(float dt);

	void DoUnitLoop();
	void DoBuildingLoop();

private:
	iPoint FindClosestWalkable(int x, int y);
	bool DoUnitsIntersect(Unit* unit1, Unit* unit2);
	void SplitUnits(Unit* unit1, Unit* unit2);
	j1Timer timer;
	j1PerfTimer performanceTimer;
public:
	bool mapChanged = false;

};

#endif // __M_COLLISIONCONTROLLER_H__