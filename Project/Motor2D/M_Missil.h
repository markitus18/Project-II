#ifndef __MISSIL_H__
#define __MISSIL_H__

#include "j1Module.h"

enum MissileTypes
{
	DRAGOON_MISSILE = 0,
	HYDRALISK_MISSILE,
	MUTALISK_MISSILE,
	SUNKEN_MISSILE,
	SCOUT_AIR_MISSILE,
	SCOUT_MISSILE,
	SPORE_MISSILE,
};
class Controlled;

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
	bool attackingBuilding = false;
	MissileTypes type;
};

class M_Missil : public j1Module
{
public:
	M_Missil(bool);
	~M_Missil(){};

	bool Start();

	void AddMissil(fPoint start, Controlled* target, int damage, MissileTypes typeOfMissile = HYDRALISK_MISSILE, bool attackingABuilding = false);

	bool Update(float dt);

private:

	void UpdateMissiles(float dt);

	void AssignByType(Num_Missil* output, MissileTypes typeOfMissile);
	void CreateExplosion(fPoint position, MissileTypes typeOfMissile);

	std::list <Num_Missil>	missilList;

	SDL_Texture*	dragoonTexture = NULL;
	SDL_Texture*	hydraliskTexture = NULL;
	SDL_Texture*	mutaliskTexture = NULL;
	SDL_Texture*	scoutTexture = NULL;

	C_Sprite		dragoonExplosion;
	C_Sprite		hydraliskExplosion;
	C_Sprite		mutaliskExplosion;
	C_Sprite		scoutExplosion;
	C_Sprite		sunkenExplosion;

};

#endif //__MISSIL_H__