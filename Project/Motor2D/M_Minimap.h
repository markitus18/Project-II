#ifndef _MINIMAP__
#define _MINIMAP__

#include "j1Module.h"

#include "j1Timer.h"

class UI_Image;
class Unit;
class Building;
class Resource;

class M_Minimap : public j1Module
{
public:
	M_Minimap(bool);
	~M_Minimap();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnGUI(GUI_EVENTS event, UI_Element* element);

	iPoint WorldToMinimap(int x, int y) const;
	iPoint MinimapToWorld(int x, int y) const;

	void PingOnWorld(int x, int y, bool forced = false);
	void PingOnMinimap(int x, int y, bool forced = false);

	void DrawUnit(Unit* unit);
	void DrawBuilding(Building* building);
	void DrawResource(Resource* resource);

	SDL_Texture* creepTex[4];
	UI_Image* creep[4];

	SDL_Texture* minimap;
	UI_Image* map;

	bool movingMap = false;
	bool freezeMovement = false;

	C_Sprite ping;
	C_Sprite boss;
	C_Sprite boss_white;
	C_Sprite boss_purple;
	j1Timer pingTimer;
	iPoint pingPos;
	iPoint rectSizes;
	float pingRadius;
	int bossRadius;
	int roughTimer;
	bool pinging;
	SDL_Rect coords;
	uint underAttack1;
	uint underAttack2;

};

#endif //_EXPLOSION__