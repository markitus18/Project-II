#ifndef __BOSS_H__
#define __BOSS_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"
#include "M_Missil.h"

#include "Unit.h"

#include "j1Timer.h"

enum Boss_State
{
	BOSS_STAND,
	BOSS_MOVE,
	BOSS_ATTACK,
	BOSS_SPELL,
	BOSS_STUN,
	BOSS_DIE,
};

enum Boss_Attack_State
{
	BOSS_ATK_STAND,
	BOSS_ATK_DEFEND,
	BOSS_ATK_ATTACK,
	BOSS_ATK_COOLDOWN,
};

class Boss : public Unit
{
public:
	Boss();
	Boss(float x, float y, Unit_Type _type, Player_Type);
	Boss(fPoint);
	~Boss();

	bool Start();
	bool Update(float dt);

private:

	Boss_State bossState = BOSS_STAND;
	Boss_Attack_State bossAtkState = BOSS_ATK_ATTACK;

	//Movement variables--------------------------
	iPoint target;
	//Velocities
	C_Vec2<float> currentVelocity = { 0, 0 };
	C_Vec2<float> desiredVelocity = { 0, 0 };

	float rotationSpeed = 500.0f; //Used as angles / seconds
	float targetRadius = 3.0f;
	//--------------------------------------------
};

#endif