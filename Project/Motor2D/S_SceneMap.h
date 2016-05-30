#ifndef __S_SceneMap_H__
#define __S_SceneMap_H__

#include "j1Module.h"
#include "j1Timer.h"

#include <map>

enum Grid_Type;
enum Unit_Type;
class Unit;
class Building;

struct _TTF_Font;

//UI WEIRD STUFF------------------
struct Grid_Coords;
class Grid3x3;
class UI_Image;
class UI_Label;
class Stats_Panel_Single;
class Stats_Panel_Mult;
struct UI_Panel_Queue;
class UI_ProgressBar;
//--------------------------------

#define CAMERA_SPEED 1600.0f

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
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();


	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;


	void ManageInput(float dt);
	void UnitCreationInput();

	void LoadTextures();
	void LoadGUI();
	void OnGUI(GUI_EVENTS event, UI_Element* element);

	void SpawnResources();
	void SpawnStartingUnits();

	void FirstEventScript();
	void SecondEventScript();
	void VictoryEventScript();

	void DisplayMineralFeedback();
	void DisplayGasFeedback();
	void DisplayPsiFeedback();

	void AddBossBar();
	

	// Event Manager
	j1Timer scriptTimer;
	
	bool action, action_aux;
	bool onEvent;
	bool kerriganSpawn;
	bool onEventVictory;
	bool interruptEvent;

	Unit* scripted_unit1 = NULL;
	Unit* scripted_unit2 = NULL;
	Unit* scripted_unit3 = NULL;
	Unit* scripted_unit4 = NULL;
	Unit* scripted_unit5 = NULL;
	Unit* scripted_zergling = NULL;
	Unit* scripted_shuttle1 = NULL;
	Unit* scripted_shuttle2 = NULL;
	Unit* startingUnits[10];

	uint sfx_shuttle_drop;
	uint sfx_script_adquire;
	uint sfx_script_beep;
	uint brief_leave_planet;
	uint brief_no_fear;
	uint brief_reinforcement;
	uint boss_kill_you;

	C_Sprite spawnSplash;
	C_Sprite bloodSplash;

	// Briefing Labels and Stuff
	j1Timer auxBriefTimer;

	SDL_Texture* loading_tex = NULL;
	UI_Image* loading_image = NULL;

	SDL_Texture* inactiveProbe_tex = NULL;
	UI_Image* inactiveProbe = NULL;

	UI_Label* intro_text_name = NULL;
	UI_Label* intro_text_1 = NULL;
	UI_Label* intro_text_2 = NULL;
	UI_Label* intro_text_3 = NULL;
	UI_Label* intro_text_4 = NULL;
	UI_Label* intro_text_5 = NULL;
	UI_Label* intro_text_6 = NULL;

	UI_Label* spawn_text_name_1 = NULL;
	UI_Label* spawn_text_name_2 = NULL;
	UI_Label* spawn_text_name_3 = NULL;
	UI_Label* spawn_text_1 = NULL;
	UI_Label* spawn_text_2 = NULL;
	UI_Label* spawn_text_3 = NULL;

	UI_Label* win_text_name = NULL;
	UI_Label* win_text_2 = NULL;
	UI_Label* win_text_3 = NULL;
	int br_x;
	int br_y;

	// Resources Display
	int displayed_mineral;
	int displayed_gas;
	
	void UpdateDisplayedResources(char* tmp);

	int psi_reached_timer;

	void UpdateDisplayedPsiReached(float dt, char* tmp);

public:

//UI WEIRD STUFF-----------------------------------

	//List of all grids
	std::vector<Grid3x3*> grids;
	std::vector<Grid_Type> gridTypes;

	Grid_Coords* coords = NULL;

	//The image at the bottom
	UI_Image* controlPanel = NULL;
	UI_Image* finalScreen = NULL;
	//Resources
	UI_Image* res_img_0 = NULL;
	UI_Image* res_img_1 = NULL;
	UI_Image* res_img_2 = NULL;

	UI_Label* res_lab_0 = NULL;
	UI_Label* res_lab_1 = NULL;
	UI_Label* res_lab_2 = NULL;

	UI_Image* bossBase = NULL;
	UI_ProgressBar* bossLife = NULL;
	UI_ProgressBar* bossShield = NULL;

	UI_Image* bossBase2 = NULL;
	UI_ProgressBar* bossLife2 = NULL;
	UI_ProgressBar* bossShield2 = NULL;

	//UI_Image* bossBlood;
	//Textures
	SDL_Texture* uiIconsT = NULL;
	SDL_Texture* orderIconsT = NULL;
	SDL_Texture* atlasT = NULL;
	SDL_Texture* controlPT = NULL;
	SDL_Texture* uiWireframesT = NULL;
	SDL_Texture* victoryT = NULL;
	SDL_Texture* defeatT = NULL;
	SDL_Texture* queue_backgroundT = NULL;
	SDL_Texture* boss_life_barT = NULL;
	SDL_Texture* boss_shield_barT = NULL;
	SDL_Texture* boss_base_barT = NULL;
	//Orders hover textures
	SDL_Texture* buildings_hover = NULL;
	SDL_Texture* units_hover = NULL;
	SDL_Texture* orderAttack_hover = NULL;
	SDL_Texture* orderCancel_hover = NULL;
	SDL_Texture* orderMove_hover = NULL;
	SDL_Texture* orderRallypoint_hover = NULL;
	SDL_Texture* orderStop_hover = NULL;
	SDL_Texture* orderStructure_hover = NULL;

	SDL_Texture* progressBar_back = NULL;
	SDL_Texture* progressBar_bar = NULL;
	//Stats_Panel_Single* statsPanel_s;
	Stats_Panel_Mult* statsPanel_m = NULL;
	UI_Panel_Queue* panel_queue = NULL;

	std::map<Unit_Type, SDL_Rect> ui_unit_sections;
//-------------------------------------------------

	std::vector<iPoint> cameraPositions;

//-------------------------------------------------

	//Debug Labels
	UI_Label* screenMouse = NULL;
	UI_Label* globalMouse = NULL;
	UI_Label* tileMouse = NULL;
	float labelUpdateTimer;

	C_DynArray<iPoint> path;
	int currentTile_x;
	int currentTile_y;

	bool startTileExists;
	bool endTileExists;
	iPoint startTile;
	iPoint endTile;

	Building* building = NULL;
	Unit* unit = NULL;

	C_Sprite currentTileSprite;
	SDL_Texture* debug_tex = NULL;

	int numUnit;

	Building* zergSample = NULL;

	void useConditions();

	bool gameFinished = false;
	bool victory = false;
	bool defeat = false;

	//Quit menu
	SDL_Texture* quit_tex = NULL;
	SDL_Texture* border_tex = NULL;
	UI_Image* quit_image = NULL;
	UI_Label* cancel_label = NULL;
	UI_Label* save_label = NULL;
	UI_Image* save_border = NULL;
	UI_Label* quit_label = NULL;
	UI_Image* quit_border = NULL;
	UI_Label* save_quit_label = NULL;
	_TTF_Font* quit_info_font = NULL;

	

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
