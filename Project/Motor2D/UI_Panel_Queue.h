#ifndef _PRODUCTION_QUEUE_H_
#define _PRODUCTION_QUEUE_H_
#define QUEUE_SLOTS 5
#include <map>
#include <list>
#include "Defs.h"
class UI_Image;
class SDL_Rect;
class Building;
class Unit;
class C_BuildingQueue;
class UI_ProgressBar_F;
enum Unit_Type;

struct UI_Panel_Queue
{	
	~UI_Panel_Queue();
	void disableQueue();
	void removeSlot(uint = 0);
	void addSlot(Unit_Type);
	void loadBuilding(Building*);
	void UpdateQueue();

public:
	Building* current_build;

	int current_slots = -1;

	std::map<Unit_Type, SDL_Rect>* icon_rects;

	UI_Image* icons[QUEUE_SLOTS];
	UI_Image* background = NULL; //100 38 154 75
	UI_Image* progress_background = NULL;
	UI_ProgressBar_F* progress_bar = NULL;

	float bar_current = 0.0f;
	float bar_max = 1.0f;

};
#endif // !_PRODUCTION_QUEUE_H_
