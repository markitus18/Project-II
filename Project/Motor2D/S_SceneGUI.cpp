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
	//icons = App->tex->Load("ui/cmdicons.png");
	atlas = App->tex->Load("gui/pcmdbtns.png");
	if (icons == NULL)
	{
		LOG("Error at loading icons texture");
	}
	if (atlas == NULL)
	{
		LOG("Error at loading icons texture");
	}
	LoadGUI();
	return true;
}

void S_SceneGUI::LoadGUI()
{
	//test = App->tex->Load("gui/pcmdbtns.png");
			//debug = App->gui->CreateUIButton2({ 50, 50,36,35 }, "gui/pcmdbtns.png", { 0, 0, 36, 35 }, { 72, 0, 36, 35 }, true);
	App->gui->CreateUI_Button({ 50, 50 }, "gui/pcmdbtns.png",{ 0, 0, 36, 35 }, { 0, 0, 36, 35 }, { 0, 0, 36, 35 });

	App->gui->CreateUIButton2({ 50, 100 }, "gui/pcmdbtns.png", { 0, 0, 36, 35 }, { 0, 0, 36, 35 });
}

bool S_SceneGUI::Update(float dt)
{
	SDL_Rect pos1 = { 100, 50 };
	App->render->Blit(atlas, &pos1, true);

	return true;
}



bool S_SceneGUI::CleanUp()
{
	RELEASE(icons);
	RELEASE(atlas);
	return true;
}