#ifndef __MISSIL_H__
#define __MISSIL_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"
#include "j1Module.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "j1App.h"
#include "Controlled.h"
#include "Entity.h"

#include "j1Timer.h"

struct Num_Missil
{
	fPoint pos;
	Controlled* target;
	float vel;
	uint dmg;
};

class M_Missil : public j1Module
{
public:
	M_Missil(bool);
	~M_Missil(){};

	bool Start();

	void AddMissil(fPoint start, Controlled* target);

	bool Update(float dt);

private:
	std::list <Num_Missil> missilList;
	C_Sprite missile;
	float timer = 0.0f;
	uint nFrames = 4;


};

#endif //__MISSIL_H__