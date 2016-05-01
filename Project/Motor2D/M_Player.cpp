#include "j1Module.h"
#include "M_Player.h"

M_Player::M_Player(bool _active) : j1Module(_active)
{
	name.create("pathFinding");
}

M_Player::~M_Player()
{

}


bool M_Player::Awake()
{
	return true;
}

bool M_Player::Start()
{
	return true;
}

bool M_Player::Uppdate()
{
	return true;
}

bool M_Player::CleanUp()
{
	return true;
}