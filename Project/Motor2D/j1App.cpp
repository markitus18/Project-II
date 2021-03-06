#include <iostream> 
#include <sstream> 
#include<time.h>

#include "Defs.h"
#include "Log.h"

#include "j1App.h"
#include "M_Window.h"
#include "M_Input.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_Map.h"
#include "S_SceneMap.h"
#include "M_FileSystem.h"
#include "M_PathFinding.h"
#include "M_Fonts.h"
#include "M_GUI.h"
#include "M_Console.h"
#include "M_EntityManager.h"
#include "M_CollisionController.h"
#include "S_SceneMenu.h"
#include "M_Missil.h"
#include "M_IA.h"
#include "M_FogOfWar.h"
#include "M_Explosion.h"
#include "M_Particles.h"
#include "M_InputManager.h"
#include "M_Player.h"
#include "M_Minimap.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);

	input = new M_Input(true);
	events = new M_InputManager(true);
	win = new M_Window(true);
	render = new M_Render(true);
	tex = new M_Textures(true);
	font = new M_Fonts(true);
	gui = new M_GUI(true);
	audio = new M_Audio(true);
	entityManager = new M_EntityManager(false);
	map = new M_Map(false);
	missiles = new M_Missil(false);
	IA = new M_IA(false);
	fogOfWar = new M_FogOfWar(false);
	explosion = new M_Explosion(false);
	particles = new M_Particles(false);
	player = new M_Player(false);
	minimap = new M_Minimap(false);

	//Scenes-------------------------false
	sceneMap = new S_SceneMap(false);
	sceneMenu = new S_SceneMenu(true);
	//-------------------------------

	fs = new M_FileSystem(true);
	pathFinding = new M_PathFinding(false);
	console = new M_Console(true);
	collisionController = new M_CollisionController(false);

	
	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(fs);
	AddModule(input);
	AddModule(events);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(font);
	AddModule(map);
	AddModule(gui);
	AddModule(console);
	AddModule(pathFinding);
	AddModule(collisionController);
	AddModule(entityManager);
	AddModule(explosion);
	AddModule(missiles);
	AddModule(particles);
	AddModule(IA);
	AddModule(fogOfWar);
	AddModule(minimap);
	AddModule(player);

	AddScene(sceneMap);
	AddScene(sceneMenu);

	// render last to swap buffer
	AddModule(render);
	

	PERF_PEEK(ptimer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	std::list<j1Module*>::reverse_iterator item = modules.rbegin();

	while(item != modules.rend())
	{
		RELEASE((*item));
		item++;
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	modules.push_back(module);
}

void j1App::AddScene(j1Module* module)
{
	modules.push_back(module);
	scenes.push_back(module);
}
// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);

	App->console->AddCommand(&c_LoadScene);
	App->console->AddCommand(&c_DisplayScenes);
	App->console->AddCommand(&c_save);
	App->console->AddCommand(&c_load);

	pugi::xml_node		app_config;

	bool ret = false;
		
	config = LoadConfig(config_file);

	if(config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title.create(app_config.child("title").child_value());
		organization.create(app_config.child("organization").child_value());

		int cap = app_config.attribute("framerate_cap").as_int(-1);

		if(cap > 0)
		{
			capped_ms = 1000 / cap;
		}
	}

	if(ret == true)
	{
		std::list<j1Module*>::iterator item;
		item = modules.begin();

		while(item != modules.end() && ret == true)
		{
			ret = (*item)->Awake(config.child((*item)->name.GetString()));
			item++;
		}
	}

	PERF_PEEK(ptimer);

	if (ret)
	{
		ret = LoadCVars();
	}
	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	PERF_START(ptimer);
	bool ret = true;
	std::list<j1Module*>::iterator item;
	item = modules.begin();

	item = modules.begin();
	while(item != modules.end() && ret == true)
	{
		if ((*item)->IsEnabled())
		{
			ret = (*item)->Start();
		}
		item++;
	}
	startup_time.Start();

	PERF_PEEK(ptimer);

	init = true;

	player_name = "Player";
	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	if (want_to_exit == true)
		ret = false;

	FinishUpdate();
	if (ret == false)
	{
		init = false;
	}

	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	char* buf;
	int size = App->fs->Load("config.xml", &buf);
	pugi::xml_parse_result result = config_file.load_buffer(buf, size);
	RELEASE_ARRAY(buf);

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = frame_time.ReadSec();
	frame_time.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	if (want_change_scene == true)
		changeSceneNow();

	if(want_to_save == true)
		SavegameNow();

	if(want_to_load == true)
		LoadGameNow();
	// Framerate calculations --

	if(last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float seconds_since_startup = startup_time.ReadSec();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	static char title[256];
	sprintf_s(title, 256, "FPS: %i Last Frame Ms: %u Time since startup: %u",
		frames_on_last_update, (uint)(dt*1000), (uint)seconds_since_startup);
	App->win->SetTitle(title);
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;
	std::list<j1Module*>::iterator item;
	item = modules.begin();

	while(item != modules.end() && ret == true)
	{
		if ((*item)->IsEnabled() == true)
		{
			ret = (*item)->PreUpdate();
		}
		item++;
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	std::list<j1Module*>::iterator item;
	item = modules.begin();

	CAP(dt, 0, 0.1f);
	while(item != modules.end() && ret == true)
	{
		if ((*item)->IsEnabled())
		{
			ret = (*item)->Update(dt);
		}
		item++;
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	std::list<j1Module*>::iterator item;
	item = modules.begin();

	while(item != modules.end() && ret == true)
	{
		if ((*item)->IsEnabled())
		{
			ret = (*item)->PostUpdate(dt);
		}

		item++;
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	SaveCVars();
	PERF_START(ptimer);
	bool ret = true;
	std::list<j1Module*>::reverse_iterator item;
	item = modules.rbegin();

	while(item != modules.rend() && ret == true)
	{
		ret = (*item)->Disable();
		item++;
	}
	PERF_PEEK(ptimer);
	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
float j1App::GetDT() const
{
	return dt;
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.GetString();
}

pugi::xml_node& j1App::GetConfig(const char* node)
{
	return config.child(node);
}
// Load / Save
void j1App::LoadGame(const char* file)
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	C_String tmp = file;
	tmp += ".xml";

	want_to_load = true;
	load_game.create("%s%s", fs->GetSaveDirectory(),tmp.GetString());
}

// ---------------------------------------
void j1App::SaveGame(const char* file) const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?
	C_String saveName;
	std::vector<std::string> saves;
	GetSaveGames(saves);

#pragma region //Erasing a save file if the max is reached

	while (saves.size() >= MAX_SAVE_GAMES)
	{
		std::string tmp = saves[0];
		tmp += ".xml";
		if (fs->EraseFile(tmp.c_str()))
		{
			saves.erase(saves.begin());
		}
	}

#pragma endregion
	
#pragma region //Adding date

	char* toAdd = new char[50];
	time_t     now = time(0);
	struct tm  tstruct;
	localtime_s(&tstruct, &now);

	sprintf_s(toAdd, CHAR_BIT * 4, "%02i-%02i  %02i-%02i   -   ", tstruct.tm_mday, tstruct.tm_mon + 1, tstruct.tm_hour, tstruct.tm_min);
	saveName += toAdd;

	saveName += file;

	delete[] toAdd;

#pragma endregion

	bool repeated = true;
	bool alreadyAdded = false;
	int n = 1;
	if (saves.empty() == false)
	{
		while (repeated)
		{
			for (std::vector<std::string>::const_iterator it = saves.cbegin(); it != saves.cend();)
			{

				if (*it == saveName.GetString())
				{
					char* toAdd = new char[5];

					sprintf_s(toAdd, CHAR_BIT * 4, "_%i", n);

					if (alreadyAdded)
					{
						saveName.Cut(saveName.Length() - 2);
					}
					saveName += toAdd;
					delete[] toAdd;
					n++;
					alreadyAdded = true;
					break;
				}
				it++;
				if (it == saves.cend())
				{
					repeated = false;
				}
			}
		}
	}

	saveName += ".xml";

	want_to_save = true;
	save_game.create(saveName.GetString());

}

// ---------------------------------------
void j1App::GetSaveGames(std::vector<std::string> &output) const
{
	std::vector<std::string> tmp = fs->GetSaveFiles();
	std::vector<std::string>::const_iterator it = tmp.cbegin();

	while (it != tmp.cend())
	{
		output.push_back((*it));
		it++;
	}
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	char* buffer;
	uint size = fs->Load(load_game.GetString(), &buffer);

	if(size > 0)
	{
		pugi::xml_document data;
		pugi::xml_node root;

		pugi::xml_parse_result result = data.load_buffer(buffer, size);
		RELEASE(buffer);

		if(result != NULL)
		{

			root = data.child("game_state");

			int version = root.child("version").attribute("value").as_int();

			if (version == 1)
			{
				LOG("Loading new Game State from %s...", load_game.GetString());

				if (sceneMenu->enabled == true)
				{
					changeScene(App->sceneMap, sceneMenu);
				}
				else if (sceneMap->enabled == true)
				{
					changeScene(App->sceneMap, App->sceneMap);
				}
				changeSceneNow();



				char* tmp = (char*)root.child("PlayerName").attribute("value").as_string();
				player_name = tmp;



				std::list<j1Module*>::iterator item = modules.begin();
				ret = true;

				while (item != modules.end() && ret == true)
				{
					ret = (*item)->Load(root.child((*item)->name.GetString()));
					item++;
				}

				data.reset();
				if (ret == true)
					LOG("...finished loading");
				else
					LOG("...loading process interrupted with error on module %s", (item != modules.end()) ? (*item)->name.GetString() : "unknown");
			}
			else
			{
				LOG("Tried to load an outdated save file");
				//Removing the "save/" part, to keep only the file name
				load_game.Cut(0, 4);
				App->fs->EraseFile(load_game.GetString());
				want_to_exit = true;
			}
		}
		else
			LOG("Could not parse game state xml file %s. pugi error: %s", load_game.GetString(), result.description());
	}
	else
		LOG("Could not load game state xml file %s", load_game.GetString());

	want_to_load = false;
	return ret;
}

bool j1App::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.GetString());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;
	
	root = data.append_child("game_state");

	std::list<j1Module*>::const_iterator item;
	item = modules.begin();

	root.append_child("PlayerName").append_attribute("value") = player_name.c_str();
	root.append_child("version").append_attribute("value") = 01;
	while(item != modules.end() && ret == true)
	{
		ret = (*item)->Save(root.append_child((*item)->name.GetString()));
		item++;
	}

	if(ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		// we are done, so write data to disk
		fs->Save(save_game.GetString(), stream.str().c_str(), stream.str().length());
		LOG("... finished saving", save_game.GetString());
	}
	else
		LOG("Save process halted from an error in module %s", (item != modules.end()) ? (*item)->name.GetString() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}

float j1App::GetTimeSinceStart()
{
	return startup_time.ReadSec();
}

bool j1App::isInit() const
{
	return init;
}

bool j1App::SaveCVars() const
{
	bool ret = true;

	LOG("Saving Config File");

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("cVars");
	ret = console->SaveCVars(root);

	if (ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		// we are done, so write data to disk
		fs->Save("CVars.xml", stream.str().c_str(), stream.str().length());
		LOG("Finished saving cVars");
	}
	else
		LOG("Error while saving cVars");

	data.reset();
	return ret;
}

bool j1App::LoadCVars()
{
	bool ret = true;

	char* buffer;
	uint size = fs->Load("save/CVars.xml", &buffer);

	if (size > 0)
	{
		pugi::xml_document data;
		pugi::xml_node root;

		pugi::xml_parse_result result = data.load_buffer(buffer, size);
		RELEASE_ARRAY(buffer);

		if (result != NULL)
		{
			LOG("Loading cVars");

			root = data.child("cVars");

			ret = console->LoadCVars(root);

			data.reset();
			if (ret == true)
				LOG("finished loading cVars");
			else
				LOG("Error while loading CVars");
		}
		else
			LOG("Could not load cVars.xml. pugi error: %s", result.description());
	}
	else
		LOG("Could not load cVars.xml");

	return ret;
}

j1Module* j1App::FindScene(const char* name) const
{
	j1Module* scene = NULL;
	std::list<j1Module*>::const_iterator item;
	item = modules.begin();
	bool found = false;

	while (item != modules.end() && !found)
	{
		if ((*item)->name == name)
		{
			scene = (*item);
			found = true;
		}
		item++;
	}
	return scene;
}

void j1App::SetCurrentScene(j1Module* newScene)
{
	if (newScene)
	{
		currentScene = newScene;
	}	
}

void j1App::changeScene(j1Module* toEnable, j1Module* toDisable)
{
	want_change_scene = true;
	sceneToDisable = toDisable;
	sceneToEnable = toEnable;
}

void j1App::changeSceneNow()
{
	if (sceneToDisable)
	{
		sceneToDisable->Disable();
	}
	audio->ClearLoadedFX();
	if (sceneToEnable)
	{
		sceneToEnable->Enable();
	}
	currentScene = sceneToEnable;

	sceneToDisable = NULL;
	sceneToEnable = NULL;

	want_change_scene = false;

}
j1Module* j1App::GetCurrentScene() const
{
	return currentScene;
}

uint j1App::GetFrameCount()
{
	return frame_count;
}

void j1App::C_LoadScene::function(const C_DynArray<C_String>* arg)
{
	if (arg->Count() > 1)
	{
		j1Module* sceneToLoad = App->FindScene(arg->At(1)->GetString());
		if (sceneToLoad)
		{
			App->currentScene->Disable();
			App->input->UnFreezeInput();
			sceneToLoad->Enable();
		}
		else
		{
			LOG("'%s': '%s' scene not found", arg->At(0)->GetString(), arg->At(1)->GetString());
		}
	}
	else
		LOG("'%s': not enough arguments, expecting scene name");
}

void j1App::C_DisplayScenes::function(const C_DynArray<C_String>* arg)
{
	LOG("Scene List:");
	std::list<j1Module*>::iterator item = App->scenes.begin();
	while (item != App->scenes.end())
	{
		LOG("    %s", (*item)->name.GetString());
		item++;
	}
}

void j1App::C_Save::function(const C_DynArray<C_String>* arg)
{
	LOG("Saving %s", arg->At(1)->GetString());
	App->SaveGame(arg->At(1)->GetString());
}

void j1App::C_Load::function(const C_DynArray<C_String>* arg)
{
	LOG("Loading %s", arg->At(1)->GetString());
	App->LoadGame(arg->At(1)->GetString());
}