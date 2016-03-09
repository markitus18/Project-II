// ----------------------------------------------------
// Vector2D class    -----------
// ----------------------------------------------------

#ifndef __P2VEC2_H__
#define __P2VEC2_H__

#include "Defs.h"
#include "C_Point.h"
#include <math.h>
#include <cmath>

template<class TYPE>
class C_Vec2
{
public:

	C_Point<TYPE> position;
	TYPE x, y;

	C_Vec2()
	{}

	C_Vec2(const C_Vec2<TYPE>& v)
	{
		this->position.x = v.position.x;
		this->position.y = v.position.y;
		this->x = v.x;
		this->y = v.y;
	}

	C_Vec2(const TYPE& _distanceX, const TYPE& _distanceY, const TYPE& _x = 0, const TYPE& _y = 0)
	{
		this->position.x = _x;
		this->position.y = _y;
		this->x = _distanceX;
		this->y = _distanceY;
	}

	C_Vec2& create(const TYPE& _distanceX, const TYPE& _distanceY, const TYPE& _x = 0, const TYPE& _y = 0)
	{
		this->position.x = _x;
		this->position.y = _y;
		this->x = _distanceX;
		this->y = _distanceY

			return(*this);
	}

	// Math ------------------------------------------------
	C_Vec2 operator -(const C_Vec2 &v) const
	{
		C_Vec2 r;

		r.x = x - v.x;
		r.y = y - v.y;

		return(r);
	}

	C_Vec2 operator -(const TYPE &v) const
	{
		C_Vec2 r;

		r.x = x - v;
		r.y = y - v;

		return(r);
	}

	C_Vec2 operator + (const C_Vec2 &v) const
	{
		C_Vec2 r;

		r.x = x + v.x;
		r.y = y + v.y;

		return(r);
	}

	const C_Vec2& operator -=(const C_Vec2 &v)
	{
		x -= v.x;
		y -= v.y;

		return(*this);
	}

	const C_Vec2& operator -=(const int &v)
	{
		x -= v;
		y -= v;

		return(*this);
	}

	const C_Vec2<TYPE>& operator +=(const C_Vec2<TYPE> &v)
	{
		x += v.x;
		y += v.y;

		return(*this);
	}
	const C_Vec2<TYPE>& operator *=(const float mult)
	{
		x *= mult;
		y *= mult;
		return (*this);
	}

	C_Vec2<TYPE> operator * (const float mult)
	{
		C_Vec2<TYPE> newVec;
		newVec.x = x * mult;
		newVec.y = y * mult;
		return newVec;
	}

	const C_Vec2<TYPE>& operator /=(const float mult)
	{
		x /= mult;
		y /= mult;
		return (*this);
	}

	C_Vec2<TYPE> operator / (const float mult)
	{
		C_Vec2<TYPE> newVec;
		newVec.x = x / mult;
		newVec.y = y / mult;
		return newVec;
	}
	/*
	bool operator ==(const C_Vec2& v) const
	{
	return (x == v.x && y == v.y);
	}

	bool operator !=(const C_Vec2& v) const
	{
	return (x != v.x || y != v.y);
	}

	// Utils ------------------------------------------------
	bool IsZero() const
	{
	return (x == 0 && y == 0);
	}
	*/
	C_Vec2& SetToZero()
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

	C_Vec2& Negate()
	{
		x = -x;
		y = -y;

		return(*this);
	}

	bool IsOpposite(const C_Vec2 v)
	{
		float a1 = GetAngle();
		float a2 = v.GetAngle();

		if (abs(a1 - a2) > 178 && abs(a1 - a2) < 182)
			return true;
		return false;
	}

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

	C_Vec2 GetNormal() const
	{
		float module = GetModule();
		C_Vec2 vect;
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

typedef C_Vec2<int> iVec2;
typedef C_Vec2<float> fVec2;

#endif // __P2VEC2_H__