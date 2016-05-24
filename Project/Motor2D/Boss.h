#ifndef __BOSS_H__
#define __BOSS_H__

#include "Unit.h"
#include "M_Audio.h"

class Boss : public Unit
{
public:
	Boss();
	Boss(float x, float y, Unit_Type _type, Player_Type);
	Boss(fPoint);
	~Boss();

	bool Update(float dt);

	j1Timer stunnedTimer;
	j1Timer explosionTimer;
	j1Timer explosionSpaceTimer;

	int explosion_time = 5;
	int explosion_space = 10;

	int stun_time = 9;

	bool spawn_explosion = true;

	void Stop();
	
	void Stun();
	void UpdateStun();
	void ExplosiveMutation();
	void SpawningExplosion();

	void Explode();
	void UpdateExplosion();

	void Attack();

	void MoveToSample();

	void StartDeath();

	void PlayAttackSound();
	void PlayStunSound();

	C_Sprite consumption;
	
private:
	std::vector<uint> attackSounds;
	std::vector<uint> stunSounds;
	uint sfx_consumption;
};

#endif