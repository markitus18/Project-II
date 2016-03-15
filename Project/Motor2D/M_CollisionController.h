#ifndef __M_COLLISIONCONTROLLER_H__
#define __M_COLLISIONCONTROLLER_H__

#include "j1Module.h"

class M_CollisionController : public j1Module
{
	struct collisionMap
	{
		int width;
		int height;
		uint* data;

		bool isWalkable(int x, int y) const;
		bool isOcupied(int x, int y) const;
		
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

private:
	void CreateMap();

public:

private:
	collisionMap mapData;
};

#endif // __M_COLLISIONCONTROLLER_H__