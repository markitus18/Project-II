#ifndef __S_SCENE_MENU_H__
#define __S_SCENE_MENU_H__

#include "j1Module.h"
#include "C_Player.h"

class UI_Image;
class UI_Label;

class S_SceneMenu : public j1Module
{
public:

	S_SceneMenu(bool);

	//Destructor
	virtual ~S_SceneMenu();

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void ManageInput(float dt);
	void LoadMenu1();

	Player player;


private:

	//List of all grids
	//Ask Marc about this :D
	//std::vector<Grid_Type> types;


	//The image at the bottom
	UI_Image* title_image;
	/*
	//Resources
	UI_Image* mineral_image;
	UI_Image* gas_image;
	UI_Image* psi_image;

	UI_Label* mineral_label;
	UI_Label* gas_label;
	UI_Label* psi_label;
	*/
	//Textures
	SDL_Texture* title_tex;
};
#endif // __SCENE_MENU_H__
