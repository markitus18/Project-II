#ifndef __M_COLLISIONCONTROLLER_H__
#define __M_COLLISIONCONTROLLER_H__

#include "j1Module.h"

class Unit;

class M_CollisionController : public j1Module
{
	struct collisionTile
	{
		bool free;
		Unit* unit = NULL;
	};

	struct collisionMap
	{
		int width;
		int height;
		collisionTile* data;

		bool isWalkable(int x, int y) const;
		bool IsFree(int x, int y, Unit* unit) const;		
	};

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

	bool IsFree(int x, int y, Unit* unit) const;

	void UpdateMap(Unit* unit, bool hasStopped);

private:
	void CreateMap();

public:

private:
	collisionMap mapData;
};

#endif // __M_COLLISIONCONTROLLER_H__