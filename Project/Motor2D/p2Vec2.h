// ----------------------------------------------------
// Vector2D class    -----------
// ----------------------------------------------------

#ifndef __P2VEC2_H__
#define __P2VEC2_H__

#include "p2Defs.h"
#include "p2Point.h"
#include <math.h>
#include <cmath>

template<class TYPE>
class p2Vec2
{
public:

	p2Point<TYPE> position;
	TYPE x, y;

	p2Vec2()
	{}

	p2Vec2(const p2Vec2<TYPE>& v)
	{
		this->position.x = v.position.x;
		this->position.y = v.position.y;
		this->x = v.x;
		this->y = v.y;
	}

	p2Vec2(const TYPE& _distanceX, const TYPE& _distanceY, const TYPE& _x = 0, const TYPE& _y = 0)
	{
		this->position.x = _x;
		this->position.y = _y;
		this->x = _distanceX;
		this->y = _distanceY;
	}

	p2Vec2& create(const TYPE& _distanceX, const TYPE& _distanceY, const TYPE& _x = 0, const TYPE& _y = 0)
	{
		this->position.x = _x;
		this->position.y = _y;
		this->x = _distanceX;
		this->y = _distanceY

			return(*this);
	}

	// Math ------------------------------------------------
	p2Vec2 operator -(const p2Vec2 &v) const
	{
		p2Vec2 r;

		r.x = x - v.x;
		r.y = y - v.y;

		return(r);
	}

	p2Vec2 operator -(const TYPE &v) const
	{
		p2Vec2 r;

		r.x = x - v;
		r.y = y - v;

		return(r);
	}

	p2Vec2 operator + (const p2Vec2 &v) const
	{
		p2Vec2 r;

		r.x = x + v.x;
		r.y = y + v.y;

		return(r);
	}

	const p2Vec2& operator -=(const p2Vec2 &v)
	{
		x -= v.x;
		y -= v.y;

		return(*this);
	}

	const p2Vec2& operator -=(const int &v)
	{
		x -= v;
		y -= v;

		return(*this);
	}

	const p2Vec2<TYPE>& operator +=(const p2Vec2<TYPE> &v)
	{
		x += v.x;
		y += v.y;

		return(*this);
	}
	const p2Vec2<TYPE>& operator *=(const float mult)
	{
		x *= mult;
		y *= mult;
		return (*this);
	}

	p2Vec2<TYPE> operator * (const float mult)
	{
		p2Vec2<TYPE> newVec;
		newVec.x = x * mult;
		newVec.y = y * mult;
		return newVec;
	}

	const p2Vec2<TYPE>& operator /=(const float mult)
	{
		x /= mult;
		y /= mult;
		return (*this);
	}

	p2Vec2<TYPE> operator / (const float mult)
	{
		p2Vec2<TYPE> newVec;
		newVec.x = x / mult;
		newVec.y = y / mult;
		return newVec;
	}
	/*
	bool operator ==(const p2Vec2& v) const
	{
	return (x == v.x && y == v.y);
	}

	bool operator !=(const p2Vec2& v) const
	{
	return (x != v.x || y != v.y);
	}

	// Utils ------------------------------------------------
	bool IsZero() const
	{
	return (x == 0 && y == 0);
	}
	*/
	p2Vec2& SetToZero()
	{
		x = y = 0;
		return(*this);
	}

	//Set an angle in degrees
	void SetAngle(float angle)
	{
		float mod = GetModule();
		float i = DEGTORAD(angle);
		y = -mod * sin(i);
		x = mod * cos(i);

	}

	p2Vec2& Negate()
	{
		x = -x;
		y = -y;

		return(*this);
	}

	bool IsOpposite(const p2Vec2 v)
	{
		float a1 = GetAngle();
		float a2 = v.GetAngle();

		if (abs(a1 - a2) > 178 && abs(a1 - a2) < 182)
			return true;
		return false;
		/*
		if (this->x / -v.x == this->y / -v.y && !(x == 0 && y == 0))
			return true;
		return false;
		*/
	}
	/*
	// Distances ---------------------------------------------
	TYPE DistanceTo(const p2Vec2& v) const
	{
	TYPE fx = x - v.x;
	TYPE fy = y - v.y;

	return sqrtf((fx*fx) + (fy*fy));
	}

	TYPE DistanceNoSqrt(const p2Vec2& v) const
	{
	TYPE fx = x - v.x;
	TYPE fy = y - v.y;

	return (fx*fx) + (fy*fy);
	}

	TYPE DistanceManhattan(const p2Vec2& v) const
	{
	return abs(v.x - x) + abs(v.y - y);
	}
	*/

	float GetModule() const
	{
		return sqrt(x*x + y*y);
	}

	float GetAngle() const
	{
		int y2 = (int)round(y);
		int x2 = (int)round(x);
		double angle = atan2(y2, x2);
		angle = RADTODEG(angle);
		if (angle > 0)
			angle -= 360;
		if (angle != 0)
			angle = -angle;
		return (float)angle;
	}

	p2Vec2 GetNormal() const
	{
		float module = GetModule();
		p2Vec2 vect;
		if (module != 0)
		{
			vect.x = x / module;
			vect.y = y / module;
		}
		return vect;
	}

	bool Normalize()
	{
		float module = GetModule();
		if (module != 0)
		{
			x /= module;
			y /= module;
			return true;
		}
		return false;
	}
};

typedef p2Vec2<int> iVec2;
typedef p2Vec2<float> fVec2;

#endif // __P2VEC2_H__