
#include "j1App.h"

#include "M_Explosion.h"

#include "M_Render.h"
#include "M_Textures.h"


M_Explosion::M_Explosion(bool start_enabled) : j1Module(start_enabled)
{
	name.create("Explosion");
}

bool M_Explosion::Start()
{
			
	return true;
}



void M_Explosion::AddExplosion(fPoint position, int radius, int damage)
{

}

bool M_Explosion::Update(float dt)
{
	return true;
}