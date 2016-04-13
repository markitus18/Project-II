#ifndef __MISSIL_H__
#define __MISSIL_H__

#include "j1Module.h"

#include "C_Point.h"

#include "Controlled.h"
#include "Entity.h"

struct Num_Missil
{
	fPoint pos;
	Controlled* target;
	float vel;
	uint dmg;
};

enum MissileTypes
{
	DRAGOON_MISSILE = 0,
	HYDRALISK_MISSILE,
	MUTALISK_MISSILE,
};

class M_Missil : public j1Module
{
public:
	M_Missil(bool);
	~M_Missil(){};

	bool Start();

	void AddMissil(fPoint start, Controlled* target, MissileTypes typeOfMissile = HYDRALISK_MISSILE);

	bool Update(float dt);

private:
	std::list <Num_Missil> missilList;
	C_Sprite missile;
	float timer = 0.0f;
	uint nFrames = 4;


};

#endif //__MISSIL_H__