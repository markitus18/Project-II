#ifndef __S_SceneMap_H__
#define __S_SceneMap_H__

#include "j1Module.h"
#include "M_Console.h"

class Unit;
class Building;
class Grid3x3;
class UI_Image;

class S_SceneMap : public j1Module
{

public:

	S_SceneMap(bool);

	// Destructor
	virtual ~S_SceneMap();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void ManageInput(float dt);
	void UnitCreationInput();

	void LoadGUI();
	void OnGUI(GUI_EVENTS event, UI_Element* element);

public:
	C_DynArray<iPoint> path;
	int currentTile_x;
	int currentTile_y;

	bool startTileExists;
	bool endTileExists;
	iPoint startTile;
	iPoint endTile;

	bool renderUnits = true;
	bool renderForces = false;
	bool renderMap = false;
	bool renderBuildings = true;

	Building* building;
	Unit* unit;
	SDL_Texture* debug_tex = NULL;

	SDL_Texture* mapTexture = NULL;
	SDL_Texture* mapTexture_wall = NULL;

private:
	std::list<Grid3x3*> grids;

	Grid3x3* currentGrid;
	UI_Image* console;

	//Textures
	SDL_Texture* iconsT;
	SDL_Texture* atlasT;
	SDL_Texture* consoleT;
#pragma region Commands

	struct C_SaveGame : public Command
	{
		C_SaveGame() : Command("save_game", "Save current game", 0, NULL, "Scene"){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_SaveGame c_SaveGame;

	struct C_LoadGame : public Command
	{
		C_LoadGame() : Command("load_game", "Load current game", 0, NULL, "Scene"){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_LoadGame c_LoadGame;

#pragma endregion
};

#endif // __S_SceneMap_H__
