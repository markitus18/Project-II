
#include "j1App.h"

#include "M_Missil.h"

#include "M_Render.h"
#include "M_Textures.h"
#include "Building.h"


M_Missil::M_Missil(bool start_enabled) : j1Module(start_enabled)
{
	name.create("Missil");
}

bool M_Missil::Start()
{
	dragoonTexture = App->tex->Load("graphics/neutral/missiles/dragbull.png");
	hydraliskTexture = App->tex->Load("graphics/neutral/missiles/parasite.png");
	mutaliskTexture = App->tex->Load("graphics/neutral/missiles/spores.png");

	dragoonExplosion = App->tex->Load("graphics/neutral/missiles/explo1.png");
	hydraliskExplosion = App->tex->Load("graphics/neutral/missiles/zspark.png");
	mutaliskExplosion = App->tex->Load("graphics/neutral/missiles/epbbul.png");
		
	return true;
}



void M_Missil::AddMissil(fPoint start, Controlled* target, int damage, MissileTypes typeOfMissile, bool attackingABuilding)
{
	Num_Missil missil;
	missil.attackingBuilding = attackingABuilding;
	missil.pos = start;
	missil.target = target;
	missil.dmg = damage;
	AssignByType(&missil, typeOfMissile);
	missil.missilSprite.y_ref = 1;
	missilList.push_back(missil);
}

bool M_Missil::Update(float dt)
{
	UpdateMissiles(dt);
	UpdateExplosions(dt);
	return true;
}

void M_Missil::UpdateMissiles(float dt)
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
				
				fPoint target;

				if (it->attackingBuilding)
				{
					SDL_Rect collider = it->target->GetCollider();
					target.x = collider.x + collider.w / 2;
					target.y = collider.y + collider.h / 2;
				}
				else
				{
					target = it->target->GetPosition();
				}

				vector.x = target.x - vector.position.x;
				vector.y = target.y - vector.position.y;

				vector.Normalize();

				vector *= (float)it->vel * dt;
				it->pos.x += vector.x;
				it->pos.y += vector.y;

				(*it).missilSprite.position.x = it->pos.x - (*it).missilSprite.position.w / 2;
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

				if (it->pos.DistanceManhattan(target) < it->vel * dt)
				{
					std::list <Num_Missil>::iterator it2 = it;
					it2++;
					it->target->Hit(it->dmg);
					CreateExplosion(it->pos, it->type);
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
}

void M_Missil::UpdateExplosions(float dt)
{
	if (!explosionList.empty())
	{
		std::list <Explosion>::iterator it = explosionList.begin();
		while (it != explosionList.end())
		{
			//If it has an animation, keep it moving
			(*it).timer += dt;
			if ((*it).timer > (*it).animSpeed && it->toErase == false)
			{
				(*it).timer = 0.0f;
				(*it).explosionSprite.section.y += (*it).explosionSprite.section.h;
				if ((*it).explosionSprite.section.y > (*it).nFrames*(*it).explosionSprite.section.h)
				{
					it->toErase = true;
				}
			}
			App->render->AddSprite(&(*it).explosionSprite, FX);
			it++;
		}


		it = explosionList.begin();
		while (it != explosionList.end())
		{
			//Erase ended explosions
			if (it->toErase)
			{
				std::list <Explosion>::iterator it2 = it;
				it2++;
				explosionList.erase(it);
				if (it2 == explosionList.end())
				{
					break;
				}
				it = it2;
			}
			else
			{
				it++;
			}
		}
		
	}

}

void M_Missil::AssignByType(Num_Missil* output, MissileTypes typeOfMissile)
{
	output->type = typeOfMissile;
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
	case MUTALISK_MISSILE:
	{
		output->missilSprite.texture = mutaliskTexture;
		output->missilSprite.position = { 0, 0, 36, 36 };
		output->missilSprite.section = { 0, 0, 36, 36 };
		output->nFrames = 10;
		output->vel = 200.0f;
		break;
	}
	case HYDRALISK_MISSILE:
	{
		output->missilSprite.texture = hydraliskTexture;
		output->missilSprite.position = { 0, 0, 20, 20 };
		output->missilSprite.section = { 0, 0, 20, 20 };
		output->directional = true;
		output->vel = 150.0f;
	}
	}
}

void M_Missil::CreateExplosion(fPoint position, MissileTypes typeOfMissile)
{
	Explosion tmp;

	switch (typeOfMissile)
	{
	case DRAGOON_MISSILE:
	{
		tmp.explosionSprite.texture = dragoonExplosion;
		tmp.explosionSprite.position = { position.x, position.y, 44, 56 };
		tmp.explosionSprite.section = { 0, 0, 44, 56 };
		tmp.nFrames = 10;
		tmp.animSpeed = 0.04f;
		break;
	}
	case HYDRALISK_MISSILE:
	{
		tmp.explosionSprite.texture = hydraliskExplosion;
		tmp.explosionSprite.position = { position.x, position.y, 35, 35 };
		tmp.explosionSprite.section = { 0, 0, 40, 40 };
		tmp.nFrames = 8;
		tmp.animSpeed = 0.07f;
		break;
	}
	case MUTALISK_MISSILE:
	{
		tmp.explosionSprite.texture = mutaliskExplosion;
		tmp.explosionSprite.position = { position.x, position.y, 40, 40 };
		tmp.explosionSprite.section = { 0, 0, 64, 64 };
		tmp.nFrames = 11;
		tmp.animSpeed = 0.04f;
	}
	}

	tmp.explosionSprite.position.x -= tmp.explosionSprite.position.w / 2;
	tmp.explosionSprite.position.y -= tmp.explosionSprite.position.h / 2;

	explosionList.push_back(tmp);
}