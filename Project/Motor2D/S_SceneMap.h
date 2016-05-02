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

	void ManageInput(float dt);
	void UnitCreationInput();

	void LoadTextures();
	void LoadGUI();
	void OnGUI(GUI_EVENTS event, UI_Element* element);

	void SpawnResources();
	void SpawnStartingUnits();
	void FirstEventScript();

	void DisplayMineralFeedback();
	void DisplayGasFeedback();
	void DisplayPsiFeedback();

	

	// Event Manager
	bool onEvent;
	j1Timer script1Timer;
	bool action, action_aux;
	Unit* scripted_unit1;
	Unit* scripted_unit2;
	Unit* scripted_unit3;
	Unit* scripted_unit4;
	Unit* scripted_unit5;
	Unit* scripted_shuttle1;
	Unit* scripted_shuttle2;
	uint sfx_shuttle_drop;
	uint sfx_script_adquire;

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

	//Textures
	SDL_Texture* uiIconsT;
	SDL_Texture* orderIconsT;
	SDL_Texture* atlasT;
	SDL_Texture* controlPT;
	SDL_Texture* uiWireframesT;
	SDL_Texture* victoryT;
	SDL_Texture* defeatT;
	SDL_Texture* queue_backgroundT;

	//Orders hover textures
	SDL_Texture* orderAssimilator_hover;
	SDL_Texture* orderDragoon_hover;
	SDL_Texture* orderForge_hover;
	SDL_Texture* orderGateway_hover;
	SDL_Texture* orderNexus_hover;
	SDL_Texture* orderAdvancedStructure_hover;
	SDL_Texture* orderAttack_hover;
	SDL_Texture* orderCancel_hover;
	SDL_Texture* orderGather_hover;
	SDL_Texture* orderMove_hover;
	SDL_Texture* orderRallypoint_hover;
	SDL_Texture* orderReturnCargo_hover;
	SDL_Texture* orderStop_hover;
	SDL_Texture* orderStructure_hover;
	SDL_Texture* orderHold_hover;
	SDL_Texture* orderPatrol_hover;
	SDL_Texture* orderProbe_hover;
	SDL_Texture* orderPylon_hover;
	SDL_Texture* orderZealot_hover;

	//Orders hover requirments
	SDL_Texture* orderCybernetics_requirement;
	SDL_Texture* orderDragoon_requirement;
	SDL_Texture* orderPhotonCannon_requirement;
	SDL_Texture* orderShieldBattery_requirement;
	SDL_Texture* orderTemplar_requirement;



	//Stats_Panel_Single* statsPanel_s;
	Stats_Panel_Mult* statsPanel_m;
	UI_Panel_Queue* panel_queue;

	std::map<Unit_Type, SDL_Rect> ui_unit_sections;
//-------------------------------------------------



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
	UI_Image* quit_image;
	UI_Label* yes_label;
	UI_Label* no_label;
	UI_Label* quit_label;
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
