#ifndef __INTERSECTIONS_H__
#define __INTERSECTIONS_H__

#include "C_Point.h"

struct SDL_Rect;

bool I_Rect_Circle(const SDL_Rect& rect, const float& x, const float& y, const float& radius);

bool I_Point_Cicle(const iPoint& point, const float& x, const float& y, const float& radius);

bool I_Rect_Rect(const SDL_Rect& rectA, const SDL_Rect& rectB);


#endif //__INTERSECTIONS_H__