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

enum Missil_Type
{
	DRAGOON,
};

struct Num_Missil
{
	iPoint pos;
	Controlled* target;
	float vel;
	uint dmg;
};

class Missil : j1Module
{
public:
	Missil(bool activate) : j1Module(activate){ name.create("Missil"); };
	~Missil();

	bool Start()
	{
		missile.texture = App->tex->Load("graphics\neutral\missiles\dragbull.png");
		missile.section = { 0, 0, 32, 32 };
		return true;
	}

	void AddMissil(iPoint start, Controlled* target)
	{
		Num_Missil missil;
		missil.pos = start;
		missil.target = target;
		missil.vel = 10.0f;
		missil.dmg = 10;

		missilList.push_back(missil);
	}

	bool Update(float dt)
	{
		std::list <Num_Missil>::iterator missilIterator = missilList.begin();
		while (missilIterator != missilList.end())
		{
			C_Vec2 <float> vector;
			vector.x = missilIterator->target->GetPosition().x - missilIterator->pos.x;
			vector.y = missilIterator->target->GetPosition().y - missilIterator->pos.y;

			vector.Normalize();
	
			vector*missilIterator->vel;
			missilIterator->pos.x += vector.x;
			missilIterator->pos.y += vector.y;

			missile.position.x = missilIterator->pos.x;
			missile.position.y = missilIterator->pos.y;

			App->render->AddSprite(&missile, SCENE);

			C_Point<int> point;
			point.x = missilIterator->target->GetPosition().x;
			point.x = missilIterator->target->GetPosition().y;

			if (missilIterator->pos.DistanceManhattan(point) < missilIterator->vel)
			{
				//missilIterator->target->Hit(missilIterator->dmg);
				missilList.erase(missilIterator);
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