#include "S_SceneGUI.h"

#include "j1App.h"
#include "M_Textures.h"
#include "M_GUI.h"
#include "M_Orders.h"

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
	icons = App->tex->Load("ui/cmdicons.png");
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
	//Testing buttons
	Grid_Coords coords;
	SDL_Texture* back_b = App->tex->Load("graphics/pcmdbtns.png");

	Grid3x3 nexus(coords);

	UI_Button2* button = nexus.setOrder(App->orders->o_genProbe_toss, { 1, 0, 33, 34 }, { 74, 1, 33, 34 }, 0, 0, *back_b, true);

	UI_Image* image = App->gui->CreateUI_Image(SDL_Rect{ 1, 1, 31, 31 }, "graphics/cmdicons.png", SDL_Rect{ 468, 102, 32, 32 }, SDL_Rect{ 0, 0, 0, 0 });

	image->SetParent(button);
	button->AddListener((j1Module*)App->orders);
}

bool S_SceneGUI::Update(float dt)
{
	return true;
}



bool S_SceneGUI::CleanUp()
{
	RELEASE(icons);
	RELEASE(atlas);
	return true;
}