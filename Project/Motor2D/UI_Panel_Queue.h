#ifndef _PRODUCTION_QUEUE_H_
#define _PRODUCTION_QUEUE_H_
#define QUEUE_SLOTS 5
#include <map>
#include "Defs.h"
class UI_Image;
class SDL_Rect;
class Building;
enum Unit_Type;

struct UI_Panel_Queue
{	
	~UI_Panel_Queue();
	void disableQueue();
	void removeSlot(uint);
	void addSlot(Unit_Type);
	void loadBuilding(const Building&);
public:
	int current_slots = -1;
	std::map<Unit_Type, SDL_Rect>* icon_rects;
	UI_Image* icons[QUEUE_SLOTS];
	UI_Image* background; //100 38 154 75
};
#endif // !_PRODUCTION_QUEUE_H_
