
#include "j1App.h"

#include "M_Missil.h"

#include "M_Render.h"
#include "M_Textures.h"


M_Missil::M_Missil(bool start_enabled) : j1Module(start_enabled)
{
	name.create("Missil");
}

bool M_Missil::Start()
{
	dragoonTexture = App->tex->Load("graphics/neutral/missiles/dragbull.png");
	hydraliskTexture = App->tex->Load("graphics/neutral/missiles/spores.png");
	mutaliskTexture = App->tex->Load("graphics/neutral/missiles/parasite.png");
	return true;
}



void M_Missil::AddMissil(fPoint start, Controlled* target, int damage, MissileTypes typeOfMissile)
{
	Num_Missil missil;
	missil.pos = start;
	missil.target = target;
	missil.dmg = damage;
	AssignByType(&missil, typeOfMissile);
	missil.missilSprite.y_ref = 1;
	missilList.push_back(missil);
}

bool M_Missil::Update(float dt)
{
	if (!missilList.empty())
	{
		std::list <Num_Missil>::iterator it = missilList.begin();
		while (it != missilList.end())
		{
			//If it has an animation, keep it moving
			if ((*it).nFrames > 1)
			{
				(*it).timer += dt;
				if ((*it).timer > 0.05f)
				{
					(*it).timer = 0.0f;
					(*it).missilSprite.section.y += (*it).missilSprite.section.h;
					if ((*it).missilSprite.section.y > (*it).nFrames*(*it).missilSprite.section.h)
					{
						(*it).missilSprite.section.y = 0;
					}
				}
			}

			//Check if the target is still alive
			if (it->target->active && it->target->GetHP() > 0)
			{
				//If it is, move it
				C_Vec2 <float> vector;
				vector.position.x = it->pos.x;
				vector.position.y = it->pos.y;
				vector.x = it->target->GetPosition().x - vector.position.x;
				vector.y = it->target->GetPosition().y - vector.position.y;

				vector.Normalize();

				vector *= (float)it->vel * dt;
				it->pos.x += vector.x;
				it->pos.y += vector.y;

				(*it).missilSprite.position.x = it->pos.x - (*it).missilSprite.position.w/2;
				(*it).missilSprite.position.y = it->pos.y - (*it).missilSprite.position.h / 2;

				//If it has to turn, get the angle and update the sprite
				if ((*it).directional)
				{
					int direction = round(((vector.GetAngle() - 90.f) * 32.0f) / 360.0f);
					while (direction >= 32)
					{
						direction -= 32;
					}
					while (direction < 0)
					{
						direction += 32;
					}
					if (direction > 16)
					{
						(*it).missilSprite.flip = SDL_FLIP_HORIZONTAL;
						(*it).missilSprite.section.x = (32 - direction) * (*it).missilSprite.section.w;
					}
					else
					{
						(*it).missilSprite.flip = SDL_FLIP_HORIZONTAL;
						(*it).missilSprite.section.x = (16 - direction) * (*it).missilSprite.section.w;
					}
				}



				App->render->AddSprite(&(*it).missilSprite, FX);

				if (it->pos.DistanceManhattan(it->target->GetPosition()) < it->vel * dt)
				{
					std::list <Num_Missil>::iterator it2 = it;
					it2++;
					it->target->Hit(it->dmg);
					missilList.erase(it);
					it = it2;
				}
				else
				{
					it++;
				}
			}
			else
			{
				//If it isn't, erase the missile
				std::list <Num_Missil>::iterator it2 = it;
				it2++;
				missilList.erase(it);
				it = it2;
			}

		}
	}
	return true;
}

void M_Missil::AssignByType(Num_Missil* output, MissileTypes typeOfMissile)
{
	switch (typeOfMissile)
	{
	case DRAGOON_MISSILE:
	{
		output->missilSprite.texture = dragoonTexture;
		output->missilSprite.position = { 0, 0, 32, 32 };
		output->missilSprite.section = { 0, 0, 32, 32 };
		output->nFrames = 4;
		output->vel = 250.0f;
		break;
	}
	case HYDRALISK_MISSILE:
	{
		output->missilSprite.texture = hydraliskTexture;
		output->missilSprite.position = { 0, 0, 36, 36 };
		output->missilSprite.section = { 0, 0, 36, 36 };
		output->nFrames = 10;
		output->vel = 200.0f;
		break;
	}
	case MUTALISK_MISSILE:
	{
		output->missilSprite.texture = mutaliskTexture;
		output->missilSprite.position = { 0, 0, 20, 20 };
		output->missilSprite.section = { 0, 0, 20, 20 };
		output->directional = true;
		output->vel = 150.0f;
	}
	}
}