#ifndef __M_PLAYER_H__
#define __M_PLAYER_H__

#include "j1Module.h"

struct Player_Stats
{
	int mineral = 0;
	int gas = 0;
	int realMaxPsi = 0;
	int maxPsi = 0;
	int psi = 0;
};

class M_Player : public j1Module
{
public:
	Player_Stats stats;

public:
	M_Player(bool);
	~M_Player();

	bool Awake();
	bool Start();
	bool Uppdate();
	bool CleanUp();
};

#endif