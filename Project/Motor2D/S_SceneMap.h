#ifndef __S_SceneMap_H__
#define __S_SceneMap_H__

#include "j1Module.h"
#include "M_Console.h"
#include "C_Player.h"

class Unit;
class Building;

//UI WEIRD STUFF------------------
	struct Grid_Coords;
	class Grid3x3;
	class UI_Image;
	class UI_Label;
//--------------------------------

#define CAMERA_SPEED 400.0f

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

	//GUI WEIRD STUFF ----------------------------------
	bool changeCurrentGrid(Grid3x3 * newCurrent);

		bool loaded = false;

		int min = 0, gas = 0, pep = 0, max_pep = 50;

		char it_res_c[9];
	//--------------------------------------------------

public:

	//UI WEIRD STUFF-----------------------------------
		//List of all grids
		std::vector<Grid3x3*> grids;

		Grid_Coords* coords;

		Grid3x3* currentGrid;

		//The image at the bottom
		UI_Image* controlPanel;

		//Resources
		UI_Image* res_img[2];
		UI_Label* res_lab[2];
		//Textures
		SDL_Texture* uiIconsT;
		SDL_Texture* orderIconsT;
		SDL_Texture* atlasT;
		SDL_Texture* controlPT;
	//-------------------------------------------------

	Player player;

	//Debug Labels
	UI_Label* screenMouse;
	UI_Label* globalMouse;
	float labelUpdateTimer;


	C_DynArray<iPoint> path;
	int currentTile_x;
	int currentTile_y;

	bool startTileExists;
	bool endTileExists;
	iPoint startTile;
	iPoint endTile;

	Building* building;
	Unit* unit;

	C_Sprite currentTileSprite;
	SDL_Texture* debug_tex = NULL;

private:

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
