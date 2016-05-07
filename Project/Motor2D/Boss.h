#ifndef __BOSS_H__
#define __BOSS_H__

#include "Unit.h"

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
	j1Timer explosionSpaceTimer;
	j1Timer basicAttackTimer;

	int explosion_time = 5;
	int explosion_space = 10;

	int stun_time = 20;

	void SetAttack(Building* unit);

	void Stop();
	
	void Stun();
	void UpdateStun();
	void ExplosiveMutation();

	void Explode();
	void UpdateExplosion();

	void MoveToSample();

	void StartDeath();

	C_Sprite consumption;
	
};

#endif