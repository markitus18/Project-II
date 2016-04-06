#include "Intersections.h"
#include "SDL\include\SDL.h"
#include <math.h>
#include <xtgmath.h>

bool I_Rect_Circle(const SDL_Rect& rect, const float& x, const float& y, const float& radius)
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

	return dst < radius;
}

bool I_Point_Cicle(const iPoint& point, const float& x, const float& y, const float& radius)
{
	float dst = sqrt((x - point.x)*(x - point.x) + (y - point.y)*(y - point.y));

	return dst < radius;
}

bool I_Rect_Rect(const SDL_Rect& rectA, const SDL_Rect& rectB)
{
	return (rectA.x <= rectB.x + rectB.w && rectB.x <= rectA.x + rectA.w &&
		rectA.y <= rectB.y + rectB.h && rectB.y <= rectA.y + rectA.h);
}