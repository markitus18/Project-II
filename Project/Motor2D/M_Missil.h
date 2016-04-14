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
	C_Sprite missilSprite;
	float timer = 0.0f;
	uint nFrames = 1;
	bool directional = false;
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

	void AddMissil(fPoint start, Controlled* target, int damage, MissileTypes typeOfMissile = HYDRALISK_MISSILE);

	bool Update(float dt);

private:
	std::list <Num_Missil> missilList;
	SDL_Texture* dragoonTexture = NULL;
	SDL_Texture* hydraliskTexture = NULL;
	SDL_Texture* mutaliskTexture = NULL;

};

#endif //__MISSIL_H__