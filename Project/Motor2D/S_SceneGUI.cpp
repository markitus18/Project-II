#include "S_SceneGUI.h"

#include "j1App.h"
#include "M_Textures.h"
#include "M_GUI.h"

S_SceneGUI::S_SceneGUI(bool at_start) : j1Module(at_start)
{
	name.create("scene_gui");
}

// Called before render is available
bool S_SceneGUI::Awake(pugi::xml_node& node)
{
	LOG("Loading Scene");
	App->SetCurrentScene(this);

	return true;
}

bool S_SceneGUI::Start()
{
	icons = App->tex->Load("graphics/cmdicons.png");
	atlas = App->tex->Load("graphics/pcmdbtns");
	LoadGUI();
	return true;
}

void S_SceneGUI::LoadGUI()
{
	App->gui->CreateUIButton2({ 50, 50 }, atlas, { 1, 0, 33, 34 }, { 74, 1, 33, 34 }, true);
}

bool S_SceneGUI::CleanUp()
{
	RELEASE(icons);
	RELEASE(atlas);
	return true;
}