#ifndef __MISSIL_H__
#define __MISSIL_H__

#include "j1Module.h"

#include "C_Point.h"

#include "Controlled.h"
#include "Entity.h"

enum MissileTypes
{
	DRAGOON_MISSILE = 0,
	HYDRALISK_MISSILE,
	MUTALISK_MISSILE,
};

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

struct MissileExplosion
{
	C_Sprite explosionSprite;
	float animSpeed = 0.25f;
	float timer = 0.0f;
	uint nFrames = 1;
	bool toErase = false;
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
	void UpdateExplosions(float dt);

	void AssignByType(Num_Missil* output, MissileTypes typeOfMissile);
	void CreateExplosion(fPoint position, MissileTypes typeOfMissile);

	std::list <Num_Missil>	missilList;
	std::list <MissileExplosion>	explosionList;

	SDL_Texture*	dragoonTexture = NULL;
	SDL_Texture*	hydraliskTexture = NULL;
	SDL_Texture*	mutaliskTexture = NULL;
	SDL_Texture*	dragoonExplosion = NULL;
	SDL_Texture*	hydraliskExplosion = NULL;
	SDL_Texture*	mutaliskExplosion = NULL;

};

#endif //__MISSIL_H__