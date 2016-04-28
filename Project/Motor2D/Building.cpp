#include <stdlib.h>

#include "Building.h"
#include "Entity.h"
#include "Controlled.h"

#include "j1App.h"

#include "M_Render.h"
#include "M_EntityManager.h"
#include "UI_Element.h"
#include "M_PathFinding.h"
#include "Unit.h"
#include "Intersections.h"
#include "Resource.h"
#include "M_Input.h"

Building::Building() :Controlled()
{

}
Building::Building(int x, int y, Building_Type _type, Player_Type player) : Controlled()
{
	position.x = x;
	position.y = y;
	type = _type;
	stats.player = player;
	state = BS_SPAWNING;
	LoadLibraryData();
	ChangeTileWalkability(false);
	UpdateBarPosition();
}



Building::~Building()
{

}

bool Building::Start()
{
	in_combatTimer.Start();
	logicTimer.Start();
	shieldTimer.Start();
	attackTimer.Stop();
	return true;
}

bool Building::Update(float dt)
{
	bool ret = true;

	if (type == ASSIMILATOR)
	{
		if (gatheringUnit)
		{
			int time = gatheringTimer.ReadSec();
			if (time >= 1)
			{
				gatheringUnit->SetActive(true);
				if (gasResource->Extract(8) == 8)
					gatheringUnit->ExitAssimilator(true);
				else
				{
					gatheringUnit->ExitAssimilator(false);
					gasResource->Extract(2);
				}
				gatheringUnit->gatheredType = GAS;
				gatheringUnit = NULL;
			}
		}
	}
	
	if (state == BS_SPAWNING)
	{
		UpdateSpawn(dt);
	}

	if (state == BS_ATTACKING)
	{
		UpdateAttack();
	}
	if (state == BS_DEAD)
	{
		if (logicTimer.ReadSec() > TIME_TO_ERASE_BUILDING)
		{
			ret = false;
		}
	}

	if (state != BS_DEAD)
	{
		RegenShield();
		CheckMouseHover();
		Draw();
		fire.Update(dt);
		fire2.Update(dt);
		fire3.Update(dt);
		animation.Update(dt);
		shadow.Update(dt);
		UpdateQueue();

		if (type == PHOTON_CANNON)
		{
			if (state == BS_DEFAULT && attackTimer.ReadSec() >= 2)
			{
				animation.animSpeed = 15;
				animation.type = A_UP;
				animation.currentRect = animation.lastRect = 3;
				animation.firstRect = 0;
				animation.loopEnd = false;
				attackTimer.Stop();
			}
		}
	}

	return ret;
}

Building_Type Building::GetType() const
{
	return type;
}

void Building::UpdateBarPosition()
{
	
	iPoint pos = App->pathFinding->MapToWorld(position.x, position.y);
	//HPBar_Empty->localPosition = { pos.x + collider.w / 2 - HPBar->size_x / 2, pos.y + collider.h + 10, 0, 0 };
	//HPBar_Filled->localPosition = { pos.x + collider.w / 2 - HPBar->size_x / 2, pos.y + collider.h + 10, 0, 0 };

	if (movementType == FLYING)
	{
		//HPBar_Empty->localPosition.y -= 20;
		//HPBar_Filled->localPosition.y -= 20;
	}
	
	HPBar_Empty->UpdateSprite();
	HPBar_Filled->UpdateSprite();
	
}

void Building::ChangeTileWalkability(bool walkable)
{
	for (int h = position.y; h < position.y + height_tiles; h++)
	{
		for (int w = position.x; w < position.x + width_tiles; w++)
		{
			App->pathFinding->ChangeWalkability(w, h, walkable);
		}
	}
}

void Building::AskToEnter(Unit* unit)
{
	if (gatheringUnit == NULL)
	{
		gatheringUnit = unit;
		unit->SetActive(false);
		gatheringTimer.Start();
	}
}

void Building::CheckMouseHover()
{
	if (!App->entityManager->hoveringUnit)
	{
		int x = 0, y = 0;
		App->input->GetMousePosition(x, y);
		iPoint mousePos = App->render->ScreenToWorld(x, y);

		if (mousePos.x > collider.x && mousePos.x < collider.x + collider.w &&
			mousePos.y > collider.y && mousePos.y < collider.y + collider.h)
		{
			App->entityManager->SetBuildingHover(this);
		}
		else if (App->entityManager->hoveringBuilding == this)
		{
			App->entityManager->hoveringBuilding = NULL;
		}
	}
}

bool Building::HasVision(Unit* unit)
{
	iPoint buildingPos = App->pathFinding->MapToWorld(position.x + width_tiles / 2, position.y + height_tiles / 2);
	iPoint unitPos = { (int)unit->GetPosition().x, (int)unit->GetPosition().y };
	return I_Point_Cicle(unitPos, buildingPos.x, buildingPos.y, stats.visionRange);
}

void Building::SetAttack(Unit* unit)
{
	if (state != BS_DEAD)
	{
		attackingUnit = unit;
		state = BS_ATTACKING;
		attackTimer.Start();
		animation.firstRect = animation.currentRect = 0;
		animation.lastRect = 3;
		animation.animSpeed = 15;
		animation.loopable = false;
	}
}

void Building::UpdateAttack()
{
	if (attackingUnit)
	{
		if (HasVision(attackingUnit) && attackingUnit->GetState() != STATE_DIE)
		{
			if (attackTimer.ReadSec() >= 3)
			{
				iPoint buildingCenter = App->pathFinding->MapToWorld(position.x + width_tiles / 2, position.y + height_tiles / 2);
				switch (type)
				{
				case (PHOTON_CANNON) :
				{
					App->missiles->AddMissil({ (float)buildingCenter.x, (float)buildingCenter.y }, attackingUnit, 100, DRAGOON_MISSILE); break;
				}
				case (SUNKEN_COLONY) :
				{
					App->missiles->AddMissil({ (float)buildingCenter.x, (float)buildingCenter.y }, attackingUnit, 100, HYDRALISK_MISSILE); break;
				}
				case (SPORE_COLONY) :
				{
					App->missiles->AddMissil({ (float)buildingCenter.x, (float)buildingCenter.y }, attackingUnit, 100, MUTALISK_MISSILE); break;
				}
				}
				
				attackTimer.Start();
			}
		}
		else
		{
			attackTimer.Start();
			state = BS_DEFAULT;
			attackingUnit = NULL;
		}
	}
	else
	{
		attackTimer.Start();
		state = BS_DEFAULT;
	}
}

bool Building::Hit(int amount)
{
	in_combatTimer.Start();
	shieldTimer.Start();

	int toHit = (amount - armor);

	if (toHit > 0)
	{
		if (stats.shield >= toHit)
		{
			stats.shield -= toHit;
		}
		else
		{
			int lifeLost = toHit - stats.shield;
			stats.shield = 0;
			currHP -= lifeLost;
			if (state != BS_DEAD)
			{
				UpdateBarTexture();
				//Updating basic fire
				if (currHP < maxHP / 3)
				{
					if (stats.player == PLAYER)
					{
						fire.sprite.section.y = 96;
						fire2.sprite.section.y = 96;
						fire3.sprite.section.y = 96;
					}
				}

				else if (currHP < maxHP * 2 / 3)
				{
					if (stats.player == PLAYER)
					{
						fire.sprite.section.y = 0;
						fire2.sprite.section.y = 0;
						fire3.sprite.section.y = 0;
					}
				}

			}
			if (currHP <= 0 && state != BS_DEAD)
			{
				StartDeath();
				return false;
			}
		}
	}
	LOG("Building shield: %i", stats.shield);
	return true;
}

void Building::RegenShield()
{
	if (in_combatTimer.ReadSec() >= 10)
	{
		if (shieldTimer.ReadSec() >= 1)
		{
			stats.shield += 2;
			if (stats.shield > stats.maxShield)
				stats.shield = stats.maxShield;
			shieldTimer.Start();
		}
	}
}

void Building::AddNewUnit(Unit_Type unitType, int creationTime, int unitPsi)
{
	if (queue.units.size() < 5)
	{
		queue.Add(unitType, creationTime, unitPsi);
	}
}

Unit* Building::CreateUnit(Unit_Type type, Player_Type controller)
{
	iPoint pos = FindCloseWalkableTile();
	iPoint dst = App->pathFinding->MapToWorld(pos.x, pos.y);
	return App->entityManager->CreateUnit(dst.x, dst.y, type, controller, this);
}

void Building::UpdateQueue()
{
	if (queue.Update())
	{
		CreateUnit(queue.Pop(), PLAYER);
	}
}

void Building::UpdateSpawn(float dt)
{
	spawn_animation.Update(dt);
	currHP = (logicTimer.ReadSec() / buildTime) * maxHP;
	stats.shield = (logicTimer.ReadSec() / buildTime) * stats.maxShield;
	if (logicTimer.ReadSec() >= buildTime)
	{
		currHP = maxHP;
		stats.shield = stats.maxShield;
		state = BS_DEFAULT;
		if (type == ASSIMILATOR)
		{
			if (gasResource)
				gasResource->active = false;
		}
	}
}

iPoint Building::FindCloseWalkableTile()
{
	iPoint tile = { (int)position.x - 1, (int)position.y - 1 };
	bool maxL = false, maxD = false, maxR = false, maxU = false;
	while (!App->pathFinding->IsWalkable(tile.x, tile.y))
	{
		if (!maxL)
		{
			tile.x++;
			if (tile.x == position.x + width_tiles + 1)
				maxL = true;
		}
		else if (!maxD)
		{
			tile.y++;
			if (tile.y == position.y + height_tiles + 1)
				maxD = true;
		}
		else if (!maxR)
		{
			tile.x--;
			if (tile.x == position.x - 1)
				maxR = true;
		}
		else if (!maxU)
		{
			tile.y--;
			if (tile.y == position.y - 1)
				maxU = true;
		}
		else
			return tile;
	}
	return tile;
}

void Building::StartDeath()
{
	state = BS_DEAD;
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
	HPBar_Shield->SetActive(false);
	if (App->entityManager->selectedBuilding == this)
	{
		App->entityManager->selectedBuilding = NULL;
	}
	if (type != ASSIMILATOR)
		ChangeTileWalkability(true);
	else
		gasResource->active = true;

	logicTimer.Start();
}

void Building::Destroy()
{

}

void Building::LoadLibraryData()
{
	iPoint pos = App->pathFinding->MapToWorld(position.x, position.y);

	//Loading all stats data
	const BuildingStatsData* statsData = App->entityManager->GetBuildingStats(type);
	maxHP = currHP = statsData->HP;
	stats.shield = stats.maxShield = statsData->shield;
	armor = statsData->armor;
	width_tiles = statsData->width_tiles;
	height_tiles = statsData->height_tiles;
	buildTime = statsData->buildTime;
	psi = statsData->psi;

	//Loading all sprites data
	const BuildingSpriteData* spriteData = App->entityManager->GetBuildingSprite(type);
	animation.sprite.texture = spriteData->texture;
	animation.rect_size_x = animation.sprite.section.w = spriteData->size_x;
	animation.rect_size_y = animation.sprite.section.h = spriteData->size_y;
	animation.type = A_DOWN;
	animation.firstRect = spriteData->anim_column_start;
	animation.lastRect = spriteData->anim_column_end;
	animation.sprite.y_ref = pos.y + (statsData->height_tiles - 1) * 16;
	animation.sprite.position.x = pos.x - spriteData->offset_x;
	animation.sprite.position.y = pos.y - spriteData->offset_y;
	animation.animSpeed = spriteData->animSpeed;

	//Loading shadow data
	shadow.sprite.texture = spriteData->shadow.texture;
	shadow.sprite.section = animation.sprite.section;
	shadow.rect_size_x = shadow.sprite.section.w;
	shadow.rect_size_y = shadow.sprite.section.h;
	shadow.animSpeed = spriteData->animSpeed;
	shadow.firstRect = spriteData->anim_column_start;
	shadow.lastRect = spriteData->anim_column_end;
	shadow.animSpeed = spriteData->animSpeed;
	shadow.type = A_DOWN;
	shadow.sprite.position = { 0, 0, 0, 0 };
	shadow.sprite.position.x = pos.x - spriteData->shadow.offset_x;
	shadow.sprite.position.y = pos.y - spriteData->shadow.offset_y;
	shadow.sprite.y_ref = position.y - 1;
	shadow.sprite.tint = { 0, 0, 0, 130 };

	//Collider stats
	collider.x = pos.x;
	collider.y = pos.y;
	collider.w = statsData->width_tiles * 16;
	collider.h = statsData->height_tiles * 16;

	//Base data
	base.texture = spriteData->base.texture;
	base.section = { 0, 0, spriteData->base.size_x, spriteData->base.size_y };
	base.position = { pos.x - spriteData->base.offset_x, pos.y - spriteData->base.offset_y, 0, 0 };
	base.useCamera = true;
	base.y_ref = position.y - 2;
	base.tint = { 0, 200, 0, 255 };

	//HP Bar
	HPBar_type = spriteData->HPBar_type;
	const HPBarData* HPBar = App->entityManager->GetHPBarSprite(HPBar_type - 1);
	HPBar_Empty = App->gui->CreateUI_Image({ pos.x + collider.w / 2 - HPBar->size_x / 2, pos.y + collider.h + 10, 0, 0 }, HPBar->empty, { 0, 0, HPBar->size_x, HPBar->size_y });
	HPBar_Filled = App->gui->CreateUI_ProgressBar({pos.x + collider.w / 2 - HPBar->size_x / 2, pos.y + collider.h + 10, 0, 0 }, HPBar->fill, &maxHP, &currHP, { 0, 0, HPBar->size_x, HPBar->size_y });
	HPBar_Shield = App->gui->CreateUI_ProgressBar({ pos.x + collider.w / 2 - HPBar->size_x / 2, pos.y + collider.h + 10, 0, 0 }, HPBar->shield, &stats.maxShield, &stats.shield, { 0, 0, HPBar->size_x, HPBar->size_y });

	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
	HPBar_Shield->SetActive(false);
	HPBar_Empty->sprite.useCamera = HPBar_Filled->sprite.useCamera = HPBar_Shield->sprite.useCamera = true;

	//Fire texture
	if (stats.player == PLAYER)
	{
		fire = C_Animation(App->entityManager->fire1);
		fire2 = C_Animation(App->entityManager->fire2);
		fire3 = C_Animation(App->entityManager->fire3);		
	}
	else if (stats.player == COMPUTER)
	{
		fire = C_Animation(App->entityManager->blood1);
		fire2 = C_Animation(App->entityManager->blood2);
		fire3 = C_Animation(App->entityManager->blood3);
	}

	fire.sprite.position.x = pos.x + collider.w / 2 - 32;
	fire.sprite.position.y = pos.y + collider.h / 2 - 48;

	fire2.sprite.position.x = pos.x + collider.w / 3 - 32;
	fire2.sprite.position.y = pos.y + collider.h / 2 - 48;

	fire3.sprite.position.x = pos.x + collider.w / 2 - 32;
	fire3.sprite.position.y = pos.y + collider.h / 3 - 48;

	fire3.sprite.y_ref = fire2.sprite.y_ref = fire.sprite.y_ref = animation.sprite.y_ref + 1;


	spawn_animation = C_Animation(App->entityManager->building_spawn_animation);
	spawn_animation.sprite.position.x = pos.x + collider.w / 2 - 60;
	spawn_animation.sprite.position.y = pos.y + collider.h / 2 - 60;
	spawn_animation.sprite.y_ref = animation.sprite.y_ref;
}

void Building::Draw()
{
	SDL_Rect rect = { 0, 0, 64, 64 };

	if (App->entityManager->render)
	{
		if (selected)
			App->render->AddSprite(&base, SCENE);
		if (state != BS_SPAWNING)
			App->render->AddSprite(&animation.sprite, SCENE);
		else
			App->render->AddSprite(&spawn_animation.sprite, SCENE);
	}
	if (App->entityManager->shadows && state != BS_SPAWNING)
	{
		if (shadow.sprite.texture)
		{
			App->render->AddSprite(&shadow.sprite, SCENE);
		}
		if (currHP < maxHP / 2)
		{
			if (fire.sprite.texture)
			{
				App->render->AddSprite(&fire.sprite, SCENE);
			}
			if (fire2.sprite.texture)
			{
				App->render->AddSprite(&fire2.sprite, SCENE);
			}
			if (fire3.sprite.texture)
			{
				App->render->AddSprite(&fire3.sprite, SCENE);
			}
		}
	}
	//Should be independent from scene
	if (App->entityManager->debug)
	{
		DrawDebug();
	}
	if (hasWaypoint && selected)
	{
		iPoint wayPointWorld = App->pathFinding->MapToWorld(waypointTile.x, waypointTile.y);
		SDL_Rect rect = { wayPointWorld.x, wayPointWorld.y, 16, 16 };
		App->render->AddRect(rect, true, 0, 255, 0, 150);
	}
}

void Building::DrawDebug()
{
	App->render->AddDebugRect(collider, true, 0, 255, 0, 255, false);
	if (App->entityManager->debug)
	{
		App->render->AddCircle(collider.x + collider.w / 2, collider.y + collider.h / 2, stats.visionRange, true, 255, 0, 255);
	}
}

iPoint Building::GetWorldPosition()
{
	return App->pathFinding->MapToWorld(position.x, position.y);
}