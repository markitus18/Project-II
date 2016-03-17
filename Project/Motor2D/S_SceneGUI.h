#ifndef __S_SCENE_GUI_H__
#define __S_SCENE_GUI_H__

#include "j1Module.h"

class UIButton2;
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

	void LoadGUI();

private:
	SDL_Texture* icons;
	SDL_Texture* atlas;
	UIButton2 * debug;
};
#endif // !__S_SCENE_GUI_H__
