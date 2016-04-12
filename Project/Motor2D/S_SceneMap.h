#ifndef __S_SceneMap_H__
#define __S_SceneMap_H__

#include "j1Module.h"
#include "M_Console.h"
#include "C_Player.h"

enum Grid_Type;

class Unit;
class Building;

//UI WEIRD STUFF------------------
struct Grid_Coords;
class Grid3x3;
class UI_Image;
class UI_Label;
class Stats_Panel_Single;
class Stats_Panel_Mult;
//--------------------------------

#define CAMERA_SPEED 800.0f

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

	void SpawnResources();
	void SpawnStartingUnits();

	iPoint WorldToMinimap(int x, int y);
	iPoint MinimapToWorld(int x, int y);

public:

//UI WEIRD STUFF-----------------------------------
	//List of all grids
	std::vector<Grid3x3*> grids;
	std::vector<Grid_Type> gridTypes;

	Grid_Coords* coords;

	//The image at the bottom
	UI_Image* controlPanel;
	UI_Image* map;

	//Resources
	UI_Image* res_img[2];
	UI_Label* res_lab[2];

	//Textures
	SDL_Texture* uiIconsT;
	SDL_Texture* orderIconsT;
	SDL_Texture* atlasT;
	SDL_Texture* controlPT;
	SDL_Texture* uiWireframesT;
	SDL_Texture* minimap;

	//Stats_Panel_Single* statsPanel_s;
	//Stats_Panel_Mult* statsPanel_m;
//-------------------------------------------------

	/*Mix_Music *protoss1;
	Mix_Music *protoss3;*/

//-------------------------------------------------

	PlayerData player;

	//Debug Labels
	UI_Label* screenMouse;
	UI_Label* globalMouse;
	UI_Label* tileMouse;
	float labelUpdateTimer;

	bool movingMap = false;

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

	int numUnit;

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
