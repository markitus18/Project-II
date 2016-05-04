#ifndef _MINIMAP__
#define _MINIMAP__

#include "j1Module.h"

class UI_Image;

class M_Minimap : public j1Module
{
public:
	M_Minimap(bool);
	~M_Minimap();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnGUI(GUI_EVENTS event, UI_Element* element);

	iPoint WorldToMinimap(int x, int y);
	iPoint MinimapToWorld(int x, int y);


	SDL_Texture* minimap;
	UI_Image* map;

	bool movingMap = false;

};

#endif //_EXPLOSION__