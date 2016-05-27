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

	Unit* scripted_unit1;
	Unit* scripted_unit2;
	Unit* scripted_unit3;
	Unit* scripted_unit4;
	Unit* scripted_unit5;
	Unit* scripted_zergling;
	Unit* scripted_shuttle1;
	Unit* scripted_shuttle2;
	Unit* startingUnits[4];

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

	SDL_Texture* inactiveProbe_tex;
	UI_Image* inactiveProbe;

	UI_Label* intro_text_name;
	UI_Label* intro_text_1;
	UI_Label* intro_text_2;
	UI_Label* intro_text_3;
	UI_Label* intro_text_4;
	UI_Label* intro_text_5;
	UI_Label* intro_text_6;

	UI_Label* spawn_text_name_1;
	UI_Label* spawn_text_name_2;
	UI_Label* spawn_text_name_3;
	UI_Label* spawn_text_1;
	UI_Label* spawn_text_2;
	UI_Label* spawn_text_3;

	UI_Label* win_text_name;
	UI_Label* win_text_2;
	UI_Label* win_text_3;
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

	Grid_Coords* coords;

	//The image at the bottom
	UI_Image* controlPanel;
	UI_Image* finalScreen;
	//Resources
	UI_Image* res_img[3];
	UI_Label* res_lab[3];

	UI_Image* bossBase;
	UI_ProgressBar* bossLife;
	UI_ProgressBar* bossShield;
	//UI_Image* bossBlood;
	//Textures
	SDL_Texture* uiIconsT;
	SDL_Texture* orderIconsT;
	SDL_Texture* atlasT;
	SDL_Texture* controlPT;
	SDL_Texture* uiWireframesT;
	SDL_Texture* victoryT;
	SDL_Texture* defeatT;
	SDL_Texture* queue_backgroundT;
	SDL_Texture* boss_life_barT;
	SDL_Texture* boss_shield_barT;
	SDL_Texture* boss_base_barT;
	SDL_Texture* boss_bloodT;
	//Orders hover textures
	SDL_Texture* buildings_hover;
	SDL_Texture* units_hover;
	SDL_Texture* orderAttack_hover;
	SDL_Texture* orderCancel_hover;
	SDL_Texture* orderMove_hover;
	SDL_Texture* orderRallypoint_hover;
	SDL_Texture* orderStop_hover;
	SDL_Texture* orderStructure_hover;

	SDL_Texture* progressBar_back;
	SDL_Texture* progressBar_bar;
	//Stats_Panel_Single* statsPanel_s;
	Stats_Panel_Mult* statsPanel_m;
	UI_Panel_Queue* panel_queue;

	std::map<Unit_Type, SDL_Rect> ui_unit_sections;
//-------------------------------------------------

	std::vector<iPoint> cameraPositions;

//-------------------------------------------------

	//Debug Labels
	UI_Label* screenMouse;
	UI_Label* globalMouse;
	UI_Label* tileMouse;
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

	int numUnit;

	Building* zergSample;

	void useConditions();

	bool gameFinished = false;
	bool victory = false;
	bool defeat = false;

	//Quit menu
	SDL_Texture* quit_tex;
	SDL_Texture* border_tex;
	UI_Image* quit_image;
	UI_Label* cancel_label;
	UI_Label* save_label;
	UI_Image* save_border;
	UI_Label* quit_label;
	UI_Image* quit_border;
	UI_Label* save_quit_label;
	_TTF_Font* quit_info_font;

	

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
