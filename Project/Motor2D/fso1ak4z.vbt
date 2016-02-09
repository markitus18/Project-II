#ifndef __j1APP_H__
#define __j1APP_H__

#include "j1Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "j1Console.h"

// Modules
class j1Window;
class j1Input;
class j1Render;
class j1Textures;
class j1Fonts;
class j1Gui;
class j1Audio;
class j1FileSystem;
class j1SceneGUI;
class j1SceneMap;
class j1SceneUnit;
class j1Map;
class j1PathFinding;
class j1Console;
class EntityManager;

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
	void GetSaveGames(p2List<p2SString>& list_to_fill) const;

	float GetTimeSinceStart();

	bool isInit() const;
	
	bool SaveGUI() const;
	bool LoadGUI();

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
	j1Window*			win = NULL;
	j1Input*			input = NULL;
	j1Render*			render = NULL;
	j1Textures*			tex = NULL;
	j1Audio*			audio = NULL;
	j1SceneGUI*			sceneGUI = NULL;
	j1SceneMap*			sceneMap = NULL;
	j1SceneUnit*		sceneUnit = NULL;
	j1FileSystem*		fs = NULL;
	j1Map*				map = NULL;
	j1PathFinding*		pathFinding = NULL;
	j1Fonts*			font = NULL;
	j1Gui*				gui = NULL;
	j1Console*			console = NULL;
	EntityManager*		entityManager = NULL;

private:

	p2List<j1Module*>	modules;
	p2List<j1Module*>	scenes;
	j1Module*			currentScene = NULL;

	int					argc;
	char**				args;

	p2SString			title;
	p2SString			organization;

	bool				want_to_exit = false;

	mutable bool		want_to_save = false;
	bool				want_to_load = false;
	p2SString			load_game;
	mutable p2SString	save_game;

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
		void function(const p2DynArray<p2SString>* arg);
	};
	C_LoadScene c_LoadScene;

	struct C_DisplayScenes : public Command
	{
		C_DisplayScenes() : Command("scenes", "Display scenes list", 0, NULL, "App"){};
		void function(const p2DynArray<p2SString>* arg);
	};
	C_DisplayScenes c_DisplayScenes;

	#pragma endregion

};


extern j1App* App; // No student is asking me about that ... odd :-S

#endif