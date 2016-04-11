#include "M_IA.h"


M_IA::M_IA(bool start_enabled) : j1Module(start_enabled)
{
	name.create("Missil");
}

bool M_IA::Start()
{
	return true;
}

bool M_IA::Update(float dt)
{
	return true;
}

bool M_IA::CleanUp()
{
	return true;
}