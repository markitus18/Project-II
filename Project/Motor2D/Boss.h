#ifndef __BOSS_H__
#define __BOSS_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"
#include "M_Missil.h"

#include "Unit.h"

#include "j1Timer.h"

class Boss : public Unit
{
public:
	Boss();
	Boss(float x, float y, Unit_Type _type, Player_Type);
	Boss(fPoint);
	~Boss();

	bool Update(float dt);

	//void UpdateAttackState();
	void UpdateAttack(float dt);

	j1Timer stunnedTimer;
	j1Timer explosionTimer;
	j1Timer basicAttackTimer;

	void SetAttack(Building* unit);

	void Stop();
	
	void Stun();
	void UpdateStun();
	void UpdateExplosion();

	void MoveToSample();

	void StartDeath();
	
};

#endif