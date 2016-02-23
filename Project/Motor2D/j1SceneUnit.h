#ifndef __j1SceneUnit_H__
#define __j1SceneUnit_H__

#include "j1Module.h"

class UIBar;
class Unit;

class UIImage;
class UILabel;
class UIButton;

class j1SceneUnit : public j1Module
{
public:

	j1SceneUnit(bool);

	// Destructor
	virtual ~j1SceneUnit();

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

	void OnGUI(UI_Event _event, UIElement* _element);
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

	UIImage* cont_image = NULL;
	UILabel* cont_label = NULL;
	UIButton* cont_button = NULL;

	UIImage* smooth_image = NULL;
	UILabel* smooth_label = NULL;
	UIButton* smooth_button = NULL;

	SDL_Texture* instructions_title;
	SDL_Texture* instructions;
	SDL_Texture* entity_tex;
	SDL_Texture* target_tex;
	SDL_Texture* grid_tex;

	int entityTargetIndex;
	int entityType = 0;
};

#endif // __j1SceneUnit_H__