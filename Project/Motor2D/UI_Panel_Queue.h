#ifndef _PRODUCTION_QUEUE_H_
#define _PRODUCTION_QUEUE_H_
#include <map>
class UI_Image;
class SDL_Rect;
enum Unit_Type;

struct UI_Panel_Queue
{
	UI_Panel_Queue(){}

	std::map<Unit_Type, SDL_Rect> * icon_rects;

	UI_Image* icons[5];
	UI_Image* background; //100 38 154 75
};
#endif // !_PRODUCTION_QUEUE_H_
