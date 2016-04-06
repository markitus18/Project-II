#ifndef __INTERSECTIONS_H__
#define __INTERSECTIONS_H__

#include "SDL\include\SDL.h"
#include <math.h>
#include <xtgmath.h>

bool I_Rect_Circle(SDL_Rect rect, int x, int y, int radius)
{
	int closestX, closestY;
	//Finding unit position respect building
	//X point
	if (x < rect.x)
		closestX = rect.x;
	else if (x > rect.x + rect.w)
		closestX = rect.x + rect.w;
	else
		closestX = x;
	//Y point
	if (y < rect.y)
		closestY = rect.y;
	else if (y > rect.y + rect.h)
		closestY = rect.y + rect.h;
	else
		closestY = y;

	int dstX = abs(x - closestX);
	int dstY = abs(y - closestY);
	float dst = sqrt(dstX * dstX + dstY * dstY);

	if (dst < radius)
	{
		return true;
	}
	return false;
}

#endif //__INTERSECTIONS_H__