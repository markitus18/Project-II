#ifndef __j1APP_H__
#define __j1APP_H__

#include "j1Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "M_Console.h"

// Modules
class M_Window;
class M_Input;
class M_Render;
class M_Textures;
class M_Fonts;
class j1Gui;
class M_GUI;
class M_Audio;
class M_FileSystem;
class j1SceneGUI;
class S_SceneMap;
class S_SceneUnit;
class S_SceneGUI;
class M_Map;
class M_PathFinding;
class M_Console;
class M_EntityManager;
class M_CollisionController;

class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	virtual ~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(j1Module* module);
	void AddScene(j1Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;
	pugi::xml_node& GetConfig(char* node);
	float GetDT() const;

	void LoadGame(const char* file);
	void SaveGame(const char* file) const;
	void GetSaveGames(C_List<C_String>& list_to_fill) const;

	float GetTimeSinceStart();

	bool isInit() const;
	
	j1Module* FindScene(const char* name) const;
	void SetCurrentScene(j1Module*);
	j1Module* GetCurrentScene() const;
private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;
	bool LoadCVars();
	bool SaveCVars() const;

public:

	// Modules
	M_Window*				win = NULL;
	M_Input*				input = NULL;
	M_Render*				render = NULL;
	M_Textures*				tex = NULL;
	M_Audio*				audio = NULL;
	j1SceneGUI*				sceneGUI = NULL;
	S_SceneMap*				sceneMap = NULL;
	S_SceneUnit*			sceneUnit = NULL;
	S_SceneGUI*				sceneGui = NULL;
	M_FileSystem*			fs = NULL;
	M_Map*					map = NULL;
	M_PathFinding*			pathFinding = NULL;
	M_Fonts*				font = NULL;
	M_GUI*					gui = NULL;
	M_Console*				console = NULL;
	M_EntityManager*		entityManager = NULL;
	M_CollisionController*	collisionController = NULL;

private:

	C_List<j1Module*>	modules;
	C_List<j1Module*>	scenes;
	j1Module*			currentScene = NULL;

	int					argc;
	char**				args;

	C_String			title;
	C_String			organization;

	bool				want_to_exit = false;

	mutable bool		want_to_save = false;
	bool				want_to_load = false;
	C_String			load_game;
	mutable C_String	save_game;

	bool				change_scene = false;
	int					next_scene = 0;

	bool				save_gui;
	bool				load_gui;

	j1PerfTimer			ptimer;
	uint64				frame_count = 0;
	j1Timer				startup_time;
	j1Timer				frame_time;
	j1Timer				last_sec_frame_time;
	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;
	float				dt = 0.0f;
	int					capped_ms = -1;

	pugi::xml_document	config_file;
	pugi::xml_node		config;

	bool				init = false;

	#pragma region Commands
	struct C_LoadScene : public Command
	{
		C_LoadScene() : Command("load_scene", "Load new scene", 1, NULL, "App"){};
		void function(const C_DynArray<C_String>* arg);
	};
	C_LoadScene c_LoadScene;

	struct C_DisplayScenes : public Command
	{
		C_DisplayScenes() : Command("scenes", "Display scenes list", 0, NULL, "App"){};
		void function(const C_DynArray<C_String>* arg);
	};
	C_DisplayScenes c_DisplayScenes;

	#pragma endregion

};
extern j1App* App;

#endif