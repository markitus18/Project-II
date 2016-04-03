#ifndef __S_SCENE_MENU_H__
#define __S_SCENE_MENU_H__

#include "j1Module.h"
#include "C_Player.h"

class UI_Image;
class UI_Label;
class UI_Button;
struct _TTF_Font;

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


private:

	//List of all grids
	//Ask Marc about this :D
	//std::vector<Grid_Type> types;


	//The image at the bottom
	UI_Image* title_image;
	UI_Image* background_image;
	UI_Image* single_player_image;
	
	UI_Button* single_player_button;

	UI_Label* single_player;
	
	_TTF_Font* single_player_font;

	//Textures
	SDL_Texture* title_tex;
	SDL_Texture* background_menu_tex;
	SDL_Texture* single_player_tex;
};
#endif // __SCENE_MENU_H__
