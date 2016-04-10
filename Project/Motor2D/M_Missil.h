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
		if (!missilList.empty())
		{
			timer += dt;
			if (timer > 0.05f)
			{
				timer = 0.0f;
				missile.section.y += missile.section.h;
				if (missile.section.y > nFrames*missile.section.h)
				{
					missile.section.y = 0;
				}
			}

			std::list <Num_Missil>::iterator it = missilList.begin();
			while (it != missilList.end())
			{
				C_Vec2 <float> vector;
				vector.position.x = it->pos.x;
				vector.position.y = it->pos.y;
				vector.x = it->target->GetPosition().x - vector.position.x;
				vector.y = it->target->GetPosition().y - vector.position.y;

				vector.Normalize();

				vector *= (float)it->vel * dt;
				it->pos.x += vector.x;
				it->pos.y += vector.y;

				missile.position.x = it->pos.x;
				missile.position.y = it->pos.y;

				App->render->AddSprite(&missile, SCENE);

				if (it->pos.DistanceManhattan(it->target->GetPosition()) < it->vel * dt)
				{
					std::list <Num_Missil>::iterator it2 = it;
					it2++;
					//it->target->Hit(it->dmg);
					missilList.erase(it);
					it = it2;
				}
				else
				{
					it++;
				}

			}
		}
		return true;
	}

private:
	std::list <Num_Missil> missilList;
	C_Sprite missile;
	float timer = 0.0f;
	uint nFrames = 4;


};

#endif //__MISSIL_H__