#ifndef __S_SCENE_MENU_H__
#define __S_SCENE_MENU_H__

#include "j1Module.h"
#include "C_Player.h"

class UI_Image;
class UI_Label;
class UI_Button;
class UI_AnimatedImage;
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


	void OnGUI(GUI_EVENTS event, UI_Element* element);

	void ManageInput(float dt);
	void LoadMenu1();


private:

	//Bool in order to create the menu just 1 time after a few seconds
	bool create = false;

	//Seconds the title image lasts in screen since start
	int seconds = 8;

	//Images
	//The image at the bottom of title
	UI_Image* title_image;

	//The image at the bottom of Menu 1
	UI_Image* background_menu_1_image;

	//The image at the bottom of Menu 2
	UI_Image* background_menu_2_image;

	//Single Player image
	//UI_Image* single_player_image;

	//Info image
	UI_Image* info_image;

	//Map image
	UI_Image* map_image;

	//Map Info image
	UI_Image* map_info_image;

	//OK image
	UI_Image* ok_image;

	//Cancel image
	UI_Image* cancel_image;

	//Single Player image animation
	//UI_AnimatedImage* single_player_image_animation;

	//Buttons
	//Start Button
	UI_Button* start_button;

	//Labels
	//UI_Label* single_player;
	UI_Label* computer;
	UI_Label* player;
	UI_Label* protoss;
	UI_Label* zerg;
	UI_Label * vs_1;
	UI_Label* ok;
	UI_Label* cancel;
	UI_Label* map_name;
	
	//Info font
	_TTF_Font* info_font;

	//Textures
	SDL_Texture* title_tex;
	SDL_Texture* background_menu_tex;
	SDL_Texture* info_tex;
	SDL_Texture* map_tex;
	SDL_Texture* map_info_tex;
	SDL_Texture* ok_tex;
	SDL_Texture* cancel_tex;

	//Music
	uint background_music;
};
#endif // __SCENE_MENU_H__
