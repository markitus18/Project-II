#ifndef __S_SceneUnit_H__
#define __S_SceneUnit_H__

#include "j1Module.h"

class Unit;

class S_SceneUnit : public j1Module
{
public:

	S_SceneUnit(bool);

	// Destructor
	virtual ~S_SceneUnit();

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

	void OnGUI(GUI_EVENTS event, UI_Element* element);
	void LoadGUI();

	int currentTile_x;
	int currentTile_y;

	bool startTileExists;
	bool endTileExists;
	iPoint startTile;
	iPoint endTile;

	bool renderGrid = false;
	bool renderForces = true;
	bool renderUnits = true;
	SDL_Texture* debug_tex;
	Unit* unit;

private:

	SDL_Texture* instructions_title;
	SDL_Texture* instructions;
	SDL_Texture* entity_tex;
	SDL_Texture* target_tex;
	SDL_Texture* grid_tex;

	int entityTargetIndex;
	int entityType = 0;
};

#endif // __S_SceneUnit_H__