#ifndef __S_SCENE_MENU_H__
#define __S_SCENE_MENU_H__

#include "j1Module.h"
#include "j1Timer.h"

class UI_Image;
class UI_Label;
class UI_Button;
class UI_InputText;
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

	bool CleanUp();

	void OnGUI(GUI_EVENTS event, UI_Element* element);

	void ManageInput(float dt);
	void LoadMenu1();


private:

	j1Timer startTimerDelay;

	bool wantToQuit = false;
	//Bool in order to create the menu just 1 time after a few seconds
	bool create = false;
	bool create2 = false;
	bool create3 = false;
	bool controls = false;

	//Seconds the title image lasts in screen since start
	int seconds = 8;

	//Images
	//Cursor
	UI_Image* cursor;
	SDL_Texture* cursorTexture;
	float cursorTimer;

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

	//Map border
	UI_Image* map_border;

	//Map size
	UI_Label* map_size;

	//Map Info image
	UI_Image* map_info_image;

	//OK image
	UI_Image* ok_image;

	//Cancel image
	UI_Image* cancel_image;

	//Description panel
	UI_Image* description_panel;

	//Single Player image animation
	//UI_AnimatedImage* single_player_image_animation;

	//Add your name 
	//Image
	UI_Image* enter_name_image;
	//Input
	UI_InputText* enter_name_text;
	//Label done
	UI_Label* done;

	//Menu 3 load and new game
	//background
	UI_Image* background_menu_3_image;
	//Load & New game image
	UI_Image* load_new_game_image;
	//Load game label
	UI_Label* load_label;
	//Load game border
	UI_Image* load_game_image;
	//New game label
	UI_Label* new_game_label;
	//New game border
	UI_Image* new_game_image;
	//Back image
	UI_Image* back_image;
	//Back label
	UI_Label* back_label;
	//Open input manager image
	UI_Image* input_manager_image;
	//Open input manager label
	UI_Label* input_manager_label;


	//Menu 4 select your game
	//background
	UI_Image* background_menu_4_image;
	//Selectgame image
	UI_Image* select_game_image;
	//names of your game
	UI_Label* name_label_1;
	UI_Label* name_label_2;
	UI_Label* name_label_3;
	UI_Label* name_label_4;

	//Back image
	UI_Image* back_image_4;
	//Back label
	UI_Label* back_label_4;
	


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

	//Change controls menu
	SDL_Texture* controls_tex;
	SDL_Texture* border_tex;
	UI_Image* controls_image;
	UI_Label* open_menu;
	UI_Label* save_game;
	UI_Label* load_game;
	UI_Label* open_console;
	UI_Label* activate_debug;
	UI_Label* camera_up;
	UI_Label* camera_down;
	UI_Label* camera_right;
	UI_Label* camera_left;
	UI_Label* ok_label;
	UI_Label* cancel_label;
	UI_Label* open_menu_event;
	UI_Label* save_game_event;
	UI_Label* load_game_event;
	UI_Label* open_console_event;
	UI_Label* activate_debug_event;
	UI_Label* camera_up_event;
	UI_Label* camera_down_event;
	UI_Label* camera_right_event;
	UI_Label* camera_left_event;
	UI_Image* open_menu_event_border;
	UI_Image* save_game_event_border;
	UI_Image* load_game_event_border;
	UI_Image* open_console_event_border;
	UI_Image* activate_debug_event_border;
	UI_Image* camera_up_event_border;
	UI_Image* camera_down_event_border;
	UI_Image* camera_right_event_border;
	UI_Image* camera_left_event_border;

	//Textures
	SDL_Texture* title_tex;
	SDL_Texture* background_menu_tex;
	SDL_Texture* info_tex;
	SDL_Texture* map_tex;
	SDL_Texture* map_info_tex;
	SDL_Texture* ok_tex;
	SDL_Texture* cancel_tex;
	SDL_Texture* frame;
	SDL_Texture* description;
	SDL_Texture* enter_name_tex;
	SDL_Texture* border_tex2;

	//Music
	bool m_play = false;
};
#endif // __SCENE_MENU_H__
