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
	BOSS_EXPLOSIVE,
	BOSS_DIE,
	BOSS_DEAD,
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

	bool Update(float dt);

	void UpdateAttack(float dt);

	j1Timer stunnedTimer;

	void SetAttack(Unit* unit);
	void SetAttack(Building* unit);

	void Stop();
	
	void Stun();
	void UpdateExplosive();

	Boss_State GetState() const;
	Boss_Attack_State GetAttackState() const;

	void StartDeath();

private:

	Boss_State bossState = BOSS_STAND;
	Boss_Attack_State bossAtkState = BOSS_ATK_ATTACK;
	
};

#endif