#ifndef __S_SCENE_GUI_H__
#define __S_SCENE_GUI_H__

#include "j1Module.h"

class Grid_Coords;
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
	// Called before quitting
	bool CleanUp();

	//void OnGUI(GUI_EVENTS event, UI_Element* element);
	bool changeCurrentGrid(Grid3x3 * newCurrent);

	void LoadGUI();

	bool loaded = false;

	int min = 0, gas = 0, pep = 0, max_pep = 50;

	char it_res_c[9];
private:
	//List of all grids
	std::list<Grid3x3*> grids;

	Grid_Coords* coords;

	Grid3x3* currentGrid;

	//The image at the bottom
	UI_Image* controlPanel;

	//Resources
	UI_Image* res_img[2];
	UI_Label* res_lab[2];
	//Textures
	SDL_Texture* uiIconsT;
	SDL_Texture* orderIconsT;
	SDL_Texture* atlasT;
	SDL_Texture* controlPT;
};
#endif // !__S_SCENE_GUI_H__
