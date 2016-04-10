#ifndef __MISSIL_H__
#define __MISSIL_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"
#include "j1Module.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "j1App.h"
#include "Controlled.h"
#include "Entity.h"

#include "j1Timer.h"

struct Num_Missil
{
	fPoint pos;
	Controlled* target;
	float vel;
	uint dmg;
};

class M_Missil : public j1Module
{
public:
	M_Missil(bool start_enabled) : j1Module(start_enabled)
	{
		name.create("Missil");
	}
	~M_Missil(){};

	bool Start()
	{
		missile.texture = App->tex->Load("graphics/neutral/missiles/dragbull.png");
		missile.section = { 0, 0, 32, 32 };
		missile.position = { 0, 0, 32, 32 };
		return true;
	}

	void AddMissil(fPoint start, Controlled* target)
	{
		Num_Missil missil;
		missil.pos = start;
		missil.target = target;
		missil.vel = 250.0f;
		missil.dmg = 10;

		missilList.push_back(missil);
	}

	bool Update(float dt)
	{
		std::list <Num_Missil>::iterator missilIterator = missilList.begin();
		while (missilIterator != missilList.end())
		{
			C_Vec2 <float> vector;
			vector.position.x = missilIterator->pos.x;
			vector.position.y = missilIterator->pos.y;
			vector.x = missilIterator->target->GetPosition().x - vector.position.x;
			vector.y = missilIterator->target->GetPosition().y - vector.position.y;

			vector.Normalize();
	
			vector *= (float)missilIterator->vel * dt;
			missilIterator->pos.x += vector.x;
			missilIterator->pos.y += vector.y;

			missile.position.x = missilIterator->pos.x;
			missile.position.y = missilIterator->pos.y;

			App->render->AddSprite(&missile, SCENE);

			if (missilIterator->pos.DistanceManhattan(missilIterator->target->GetPosition()) < missilIterator->vel * dt)
			{
				std::list <Num_Missil>::iterator it = missilIterator;
				it++;
				//missilIterator->target->Hit(missilIterator->dmg);
				missilList.erase(missilIterator);
				missilIterator = it;
			}
			else
			{
				missilIterator++;
			}
			
		}
		return true;
	}

private:
	std::list <Num_Missil> missilList;
	C_Sprite missile;


};

#endif //__MISSIL_H__