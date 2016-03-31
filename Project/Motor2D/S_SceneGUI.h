#ifndef __S_SCENE_GUI_H__
#define __S_SCENE_GUI_H__

#include "j1Module.h"
#include "C_Player.h"

struct Grid_Coords;
class Grid3x3;
class UI_Image;
class UI_Label;

class S_SceneGUI : public j1Module
{
public:

	S_SceneGUI(bool);

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	bool Update(float dt);

	bool PostUpdate();
	// Called before quitting
	bool CleanUp();

	//void OnGUI(GUI_EVENTS event, UI_Element* element);
	bool changeCurrentGrid(Grid3x3 * newCurrent);

	void ManageInput(float dt);
	void LoadGUI();

	bool wantToClose = false;

	Player player;

private:
	
	//List of all grids
	//Ask Marc about this :D
	//std::vector<Grid_Type> types;
	std::vector<Grid3x3*> grids;

	Grid_Coords* coords;

	Grid3x3* currentGrid;

	//The image at the bottom
	UI_Image* controlPanel;

	//Resources
	UI_Image* mineral_image;
	UI_Image* gas_image;
	UI_Image* psi_image;

	UI_Label* mineral_label;
	UI_Label* gas_label;
	UI_Label* psi_label;

	//Textures
	SDL_Texture* resource_icons_tex;
	SDL_Texture* order_icons_tex;
	SDL_Texture* buttons_back_tex;
	SDL_Texture* contorlPanel_tex;
};
#endif // __SCENE_GUI_H__
