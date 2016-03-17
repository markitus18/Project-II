// ----------------------------------------------------
// Point class    -----------
// ----------------------------------------------------

#ifndef __P2POINT_H__
#define __P2POINT_H__

#include "Defs.h"
#include <math.h>

template<class TYPE>
class C_Point
{
public:

	TYPE x, y;

	C_Point()
	{}

	C_Point(const C_Point<TYPE>& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	C_Point(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;
	}

	C_Point& create(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;

		return(*this);
	}

	// Math ------------------------------------------------
	C_Point operator -(const C_Point &v) const
	{
		C_Point r;

		r.x = x - v.x;
		r.y = y - v.y;

		return(r);
	}

	C_Point operator + (const C_Point &v) const
	{
		C_Point r;

		r.x = x + v.x;
		r.y = y + v.y;

		return(r);
	}

	const C_Point& operator -=(const C_Point &v)
	{
		x -= v.x;
		y -= v.y;

		return(*this);
	}

	const C_Point& operator +=(const C_Point &v)
	{
		x += v.x;
		y += v.y;

		return(*this);
	}

	bool operator ==(const C_Point& v) const
	{
		return (x == v.x && y == v.y);
	}

	bool operator !=(const C_Point& v) const
	{
		return (x != v.x || y != v.y);
	}

	// Utils ------------------------------------------------
	bool IsZero() const
	{
		return (x == 0 && y == 0);
	}

	C_Point& SetToZero()
	{
		x = y = 0;
		return(*this);
	}

	C_Point& Negate()
	{
		x = -x;
		y = -y;

		return(*this);
	}

	// Distances ---------------------------------------------
	TYPE DistanceTo(const C_Point& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return (TYPE)(sqrtf((fx*fx) + (fy*fy)));
	}

	TYPE DistanceNoSqrt(const C_Point& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return (fx*fx) + (fy*fy);
	}

	TYPE DistanceManhattan(const C_Point& v) const
	{
		return abs(v.x - x) + abs(v.y - y);
	}
};

typedef C_Point<int> iPoint;
typedef C_Point<float> fPoint;

#endif // __P2POINT_H__