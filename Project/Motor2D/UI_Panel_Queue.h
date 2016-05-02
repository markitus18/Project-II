#ifndef _PRODUCTION_QUEUE_H_
#define _PRODUCTION_QUEUE_H_
#define QUEUE_SLOTS 4
#include <map>
#include <list>
#include "Defs.h"
class UI_Image;
class SDL_Rect;
class Building;
class C_BuildingQueue;
class list;
enum Unit_Type;

struct UI_Panel_Queue
{	
	~UI_Panel_Queue();
	void disableQueue(bool deactivate = true);
	void removeSlot(uint );
	void addSlot(Unit_Type);
	void loadBuilding(const std::list<Unit_Type> &);
public:
	int current_slots = -1;
	std::map<Unit_Type, SDL_Rect>* icon_rects;
	UI_Image* icons[QUEUE_SLOTS + 1];
	UI_Image* background; //100 38 154 75
};
#endif // !_PRODUCTION_QUEUE_H_
