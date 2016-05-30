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

	std::vector<C_String> vector;

	bool wantToQuit = false;
	//Bool in order to create the menu just 1 time after a few seconds
	bool create = false;
	bool create2 = false;
	bool create3 = false;
	bool controls = false;

	std::vector<UI_Element*> UI_Elements;

	//Seconds the title image lasts in screen since start
	int seconds = 8;

	//Images
	//Cursor
	UI_Image* cursor = NULL;
	SDL_Texture* cursorTexture = NULL;
	float cursorTimer;

	//The image at the bottom of title
	UI_Image* title_image = NULL;

	//The image at the bottom of Menu 1
	UI_Image* background_menu_1_image = NULL;

	//The image at the bottom of Menu 2
	UI_Image* background_menu_2_image = NULL;

	//Single Player image
	//UI_Image* single_player_image;

	//Info image
	UI_Image* info_image = NULL;

	//Map image
	UI_Image* map_image = NULL;

	//Map border
	UI_Image* map_border = NULL;

	//Map size
	UI_Label* map_size = NULL;

	//Map Info image
	UI_Image* map_info_image = NULL;

	//OK image
	UI_Image* ok_image = NULL;

	//Cancel image
	UI_Image* cancel_image = NULL;

	//Description panel
	UI_Image* description_panel = NULL;

	//Single Player image animation
	//UI_AnimatedImage* single_player_image_animation;

	//Add your name 
	//Image
	UI_Image* enter_name_image = NULL;
	//Input
	UI_InputText* enter_name_text = NULL;
	//Label done
	UI_Label* done = NULL;

	//Menu 3 load and new game
	//background
	UI_Image* background_menu_3_image = NULL;
	//Load & New game image
	UI_Image* load_new_game_image = NULL;
	//Load game label
	UI_Label* load_label = NULL;
	//Load game border
	UI_Image* load_game_image = NULL;
	//Soon label
	UI_Label* soon_label = NULL;
	//New game label
	UI_Label* new_game_label = NULL;
	//New game border
	UI_Image* new_game_image = NULL;
	//Back image
	UI_Image* back_image = NULL;
	//Back label
	UI_Label* back_label = NULL;
	//Open input manager image
	UI_Image* input_manager_image = NULL;
	//Open input manager label
	UI_Label* input_manager_label = NULL;

	//Loading map image
	UI_Image* loading_image = NULL;
	//Menu 4 select your game
	//background
	UI_Image* background_menu_4_image = NULL;
	//Selectgame image
	UI_Image* select_game_image = NULL;
	//names of your game
	UI_Label** save_games = NULL;
	uint nOfSaveGames = 0;


	//Back image
	UI_Image* back_image_4 = NULL;
	//Back label
	UI_Label* back_label_4 = NULL;
	


	//Labels
	//UI_Label* single_player;
	UI_Label* computer = NULL;
	UI_Label* player = NULL;
	UI_Label* protoss = NULL;
	UI_Label* zerg = NULL;
	UI_Label* ok = NULL;
	UI_Label* cancel = NULL;
	UI_Label* map_name = NULL;

	//dificculty selection
	UI_Image* dificulty_selector = NULL;
	UI_Element* dif2 = NULL;
	UI_Element* dif3 = NULL;
	UI_Element* dif4 = NULL;
	
	//Info font
	_TTF_Font* info_font = NULL;

	//Change controls menu
	SDL_Texture* controls_tex = NULL;
	SDL_Texture* border_tex = NULL;
	UI_Image* controls_image = NULL;
	UI_Label* open_menu = NULL;
	UI_Label* open_console = NULL;
	UI_Label* activate_debug = NULL;
	UI_Label* camera_up = NULL;
	UI_Label* camera_down = NULL;
	UI_Label* camera_right = NULL;
	UI_Label* camera_left = NULL;
	UI_Label* ok_label = NULL;
	UI_Label* open_menu_event = NULL;
	UI_Label* open_console_event = NULL;
	UI_Label* activate_debug_event = NULL;
	UI_Label* camera_up_event = NULL;
	UI_Label* camera_down_event = NULL;
	UI_Label* camera_right_event = NULL;
	UI_Label* camera_left_event = NULL;
	UI_Image* open_menu_event_border = NULL;
	UI_Image* open_console_event_border = NULL;
	UI_Image* activate_debug_event_border = NULL;
	UI_Image* camera_up_event_border = NULL;
	UI_Image* camera_down_event_border = NULL;
	UI_Image* camera_right_event_border = NULL;
	UI_Image* camera_left_event_border = NULL;

	//Textures
	SDL_Texture* title_tex = NULL;
	SDL_Texture* background_menu_tex = NULL;
	SDL_Texture* info_tex = NULL;
	SDL_Texture* map_tex = NULL;
	SDL_Texture* map_info_tex = NULL;
	SDL_Texture* ok_tex = NULL;
	SDL_Texture* cancel_tex = NULL;
	SDL_Texture* frame = NULL;
	SDL_Texture* description = NULL;
	SDL_Texture* enter_name_tex = NULL;
	SDL_Texture* border_tex2 = NULL;
	SDL_Texture* loading_tex = NULL;
	SDL_Texture* selector_tex = NULL;

	//Music
	bool m_play = false;
};
#endif // __SCENE_MENU_H__
