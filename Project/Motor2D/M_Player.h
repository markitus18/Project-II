#ifndef __M_PLAYER_H__
#define __M_PLAYER_H__

#include "j1Module.h"
#include "j1Timer.h"

class UI_Label;
struct _TTF_Font;

enum Player_Type
{
	PLAYER,
	COMPUTER,
	CINEMATIC,
};

struct Player_Stats
{
	int mineral = 0;
	int gas = 0;
	int psi = 0;
	int realMaxPsi = 0;
	int maxPsi = 0; //200 max
};

class M_Player : public j1Module
{
public:
	Player_Stats stats;

private:
	j1Timer feedbackText_timer;
	uint sfx_script_adquire;
	_TTF_Font* not_enough_res_font;

	UI_Label* not_enough_minerals;
	UI_Label* not_enough_gas;
	UI_Label* need_more_pylons;

public:
	M_Player(bool);
	~M_Player();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void AddMineral(int amount);
	void AddGas(int amount);
	void AddPsi(int amount);
	void AddMaxPsi(int amount);

	void SubstractMineral(int amount);
	void SubstractGas(int amount);
	void SubstractPsi(int amount);
	void SubstractMaxPsi(int amount);

	bool CanBeCreated(int mineral, int gas, int psi, bool warning = true);

	void DisplayMineralFeedback();
	void DisplayGasFeedback();
	void DisplayPsiFeedback();
};

#endif