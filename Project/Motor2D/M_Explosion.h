#ifndef _EXPLOSION__
#define _EXPLOSION__

#include "j1Module.h"

#include "C_Point.h"
#include "M_Textures.h"
#include "M_Player.h"

#include <map>

enum Unit_Type;

enum e_Explosion_Types
{
	EXPLOSION_DEFAULT = 0,
	EXPLOSION_TERRAN,
	EXPLOSION_GAS,
	EXPLOSION_BLOOD,
	EXPLOSION_CLOUD,
	EXPLOSION_ACID,
	EXPLOSION_POISON,
	EXPLOSION_NONE,
	EXPLOSION_PSIONIC_STORM,
};

class Explosion
{
public:
	bool Fuse(float time);
	bool ToErase();

	iPoint position;
	int radius;
	float innerRadius;
	int damage;
	int nTicks;
	float tickDelay;
	/////
	float timer = 0.0f;
	int currentTick = 0;
	Player_Type objective = PLAYER;
	bool showStencil;

	e_Explosion_Types graphic;
};

struct StoredExplosion
{
	float timeUntilExplosion = 0.0f;

	iPoint position;
	int radius;
	float innerRadius;
	int damage;
	int nTicks;
	float tickDelay;
	Player_Type objective = PLAYER;
	e_Explosion_Types graphic = EXPLOSION_DEFAULT;
	bool showStencil = true;
	bool blown = false;
};

class ExplosionSystem
{
public:
	ExplosionSystem();
	ExplosionSystem(Unit_Type _toSpawn);
	void PushExplosion(float delay, iPoint relativePos, int radius, int damage, int nTicks = 1, float tickDelay = 4.0f, Player_Type objective = PLAYER, bool showStencil = true, e_Explosion_Types graphic = EXPLOSION_DEFAULT, float innerRadius = 0.0f);

	bool Update(float dt);
	void SetSpawningUnit(Unit_Type _toSpawn);
private:
	std::multimap<float, StoredExplosion> explosions;
	Unit_Type toSpawn;
	float timer = 0.0f;
public:
	iPoint position;
	float duration = 1.0f;

};

class M_Explosion : public j1Module
{
public:
	M_Explosion(bool);
	~M_Explosion(){};

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void AddExplosion(iPoint position, int radius, int damage, float delay = 4.0f, int nTicks = 1, Player_Type objective = PLAYER, e_Explosion_Types graphic = EXPLOSION_DEFAULT, bool showStencil = true, float innerRadius = 0.0f);

	void AddSystem(ExplosionSystem toPush, iPoint pos);

	bool debug = false;

	ExplosionSystem testingSystem;
	ExplosionSystem testingSystem2;
	ExplosionSystem spinSystem;
	ExplosionSystem crossSystem;
	ExplosionSystem spawnSystem;
	ExplosionSystem spawnKerrigan;
private:
	std::list<Explosion> explosions;

	std::list<ExplosionSystem> explosionSystems;

	C_Sprite stencil;
	SDL_Texture* green;
	SDL_Texture* yellow;
	SDL_Texture* red;

	C_Sprite hugeExplosion;
	C_Sprite terranExplosion;
	C_Sprite swarmExplosion;
	C_Sprite psiStorm;
	C_Sprite bossBlood;
	C_Sprite bossCloud;
	C_Sprite bossAcid;
	C_Sprite bossPoison;
};

#endif //_EXPLOSION__