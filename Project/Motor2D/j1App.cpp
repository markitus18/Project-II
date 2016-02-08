#include <iostream> 
#include <sstream> 

#include "p2Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1SceneGUI.h"
#include "j1SceneMap.h"
#include "j1SceneUnit.h"
#include "j1FileSystem.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Fonts.h"
#include "j1Gui.h"
#include "j1Console.h"
#include "Entity.h"
#include "Unit.h"
#include "EntityManager.h"


// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);

	input = new j1Input(true);
	win = new j1Window(true);
	render = new j1Render(true);
	tex = new j1Textures(true);
	font = new j1Fonts(true);
	gui = new j1Gui(true);
	audio = new j1Audio(true);
	entityManager = new EntityManager(true);

	//Scenes-------------------------
	sceneGUI = new j1SceneGUI(false);
	sceneMap = new j1SceneMap(false);
	sceneUnit = new j1SceneUnit(true);
	//-------------------------------

	fs = new j1FileSystem(true);
	map = new j1Map(true);
	pathFinding = new j1PathFinding(true);
	console = new j1Console(true);


	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(fs);
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(font);
	AddModule(gui);
	AddModule(console);
	AddScene(sceneGUI);
	AddScene(sceneMap);
	AddScene(sceneUnit);
	AddModule(entityManager);
	AddModule(pathFinding);

	// render last to swap buffer
	AddModule(render);

	PERF_PEEK(ptimer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	p2List_item<j1Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	modules.add(module);
}

void j1App::AddScene(j1Module* module)
{
	modules.add(module);
	scenes.add(module);
}
// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);

	App->console->AddCommand(&c_LoadScene);
	App->console->AddCommand(&c_DisplayScenes);

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
		p2List_item<j1Module*>* item;
		item = modules.start;

		while(item != NULL && ret == true)
		{
			if (item->data->IsEnabled())
				ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
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
	p2List_item<j1Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		if (item->data->IsEnabled())
			ret = item->data->Start();
		item = item->next;
	}
	startup_time.Start();

	PERF_PEEK(ptimer);

	init = true;

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
	RELEASE(buf);

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

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	static char title[256];
	sprintf_s(title, 256, "Av.FPS: %.2f Last Frame Ms: %u Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %lu ",
			  avg_fps, last_frame_ms, frames_on_last_update, dt, seconds_since_startup, frame_count);
	App->win->SetTitle(title);

	if(capped_ms > 0 && (int)last_frame_ms < capped_ms)
	{
		j1PerfTimer t;
		SDL_Delay(capped_ms - last_frame_ms);
		LOG("We waited for %d milliseconds and got back in %f", capped_ms - last_frame_ms, t.ReadMs());
	}
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->IsEnabled() == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->IsEnabled() == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->IsEnabled() == false) {
			continue;
		}

		ret = item->data->PostUpdate(dt);
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	SaveCVars();
	SaveGUI();
	PERF_START(ptimer);
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->Disable();
		item = item->prev;
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

pugi::xml_node& j1App::GetConfig(char* node)
{
	return config.child(node);
}
// Load / Save
void j1App::LoadGame(const char* file)
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
	load_game.create("%s%s", fs->GetSaveDirectory(), file);
}

// ---------------------------------------
void j1App::SaveGame(const char* file) const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
	save_game.create(file);
}

// ---------------------------------------
void j1App::GetSaveGames(p2List<p2SString>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
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
			LOG("Loading new Game State from %s...", load_game.GetString());

			root = data.child("game_state");

			p2List_item<j1Module*>* item = modules.start;
			ret = true;

			while(item != NULL && ret == true)
			{
				ret = item->data->Load(root.child(item->data->name.GetString()));
				item = item->next;
			}

			data.reset();
			if(ret == true)
				LOG("...finished loading");
			else
				LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown");
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

	p2List_item<j1Module*>* item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Save(root.append_child(item->data->name.GetString()));
		item = item->next;
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
		LOG("Save process halted from an error in module %s", (item != NULL) ? item->data->name.GetString() : "unknown");

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
		RELEASE(buffer);

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

bool j1App::SaveGUI() const
{
	bool ret = true;

	LOG("Saving GUI File");

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("Labels");
	ret = sceneGUI->SaveDrag(root);

	if (ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		// we are done, so write data to disk
		fs->Save("gui.xml", stream.str().c_str(), stream.str().length());
		LOG("Finished savin GUI");
	}
	else
		LOG("Error while saving GUI");

	data.reset();
	return ret;
}

bool j1App::LoadGUI()
{
	bool ret = true;

	char* buffer;
	uint size = fs->Load("save/gui.xml", &buffer);

	if (size > 0)
	{
		pugi::xml_document data;
		pugi::xml_node root;

		pugi::xml_parse_result result = data.load_buffer(buffer, size);
		RELEASE(buffer);

		if (result != NULL)
		{
			LOG("Loading GUI");

			root = data.child("Labels");

			ret = sceneGUI->LoadDrag(root);

			data.reset();
			if (ret == true)
				LOG("finished loading GUI");
			else
				LOG("Error while loading GUI");
		}
		else
			LOG("Could not load gui.xml. pugi error: %s", result.description());
	}
	else
		LOG("Could not load gui.xml");

	return ret;
}

j1Module* j1App::FindScene(const char* name) const
{
	j1Module* scene = NULL;
	p2List_item<j1Module*>* item;
	bool found = false;

	for (item = scenes.start; item && !found; item = item->next)
	{
		if (item->data->name == name)
		{
			scene = item->data;
			found = true;
		}
	}
	return scene;
}

void j1App::SetCurrentScene(j1Module* newScene)
{
	currentScene = newScene;
}

j1Module* j1App::GetCurrentScene() const
{
	return currentScene;
}
void j1App::C_LoadScene::function(const p2DynArray<p2SString>* arg)
{
	if (arg->Count() > 1)
	{
		j1Module* sceneToLoad = App->FindScene(arg->At(1)->GetString());
		if (sceneToLoad)
		{
			App->currentScene->Disable();
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

void j1App::C_DisplayScenes::function(const p2DynArray<p2SString>* arg)
{
	LOG("Scene List:");
	p2List_item<j1Module*>* item = NULL;
	for (item = App->scenes.start; item; item = item->next)
	{
		LOG("    %s", item->data->name.GetString());
	}
}