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
#include "M_InputManager.h"
#include "M_Player.h"
#include "M_Minimap.h"
#include "M_GUI.h"

Building::Building() :Controlled()
{

}
Building::Building(int x, int y, Building_Type _type, Player_Type player) : Controlled()
{
	position.x = x;
	position.y = y;
	type = _type;
	stats.player = player;
}

Building::Building(Building& toCopy)
{
	position = toCopy.position;
	type = toCopy.type;
	stats.player = toCopy.stats.player;
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

	dead = false;
	state = BS_SPAWNING;
	LoadLibraryData();
	ChangeTileWalkability(false);
	UpdateBarPosition();

	if (type == ASSIMILATOR)
	{
		if (gasResource)
			gasResource->ocupied = true;
	}
	if (type == SUNKEN_COLONY)
		animation.currentRect = 3;

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
		ret = UpdateDeath(dt);
	}

	if (state != BS_DEAD)
	{
		RegenShield();
		CheckMouseHover();
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
				animation.currentRect = animation.firstRect = 3;
				animation.lastRect = 0;
				animation.loopEnd = false;
				attackTimer.Stop();
			}
		}
	}

	Draw();

	if (!ret)
	{
		dead = true;
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
	// HPBar_Filled->localPosition = { pos.x + collider.w / 2 - HPBar->size_x / 2, pos.y + collider.h + 10, 0, 0 };

	if (movementType == FLYING)
	{
		HPBar->localPosition.y -= 20;
	}
}

bool Building::UpdateDeath(float dt)
{
	death_animation.Update(dt);
	if (death_animation.loopEnd && logicTimer.ReadSec() > TIME_TO_ERASE_BUILDING)
	{
		return false;
	}
	return true;
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
	if (App->events->hoveringUI == false)
	{
		if (!App->entityManager->hoveringUnit)
		{
			iPoint mousePos = App->events->GetMouseOnWorld();

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
}

bool Building::HasVision(Unit* unit)
{
	iPoint buildingPos = App->pathFinding->MapToWorld(position.x + width_tiles / 2, position.y + height_tiles / 2);
	iPoint unitPos = { (int)unit->GetPosition().x, (int)unit->GetPosition().y };
	return I_Point_Cicle(unitPos, buildingPos.x, buildingPos.y, stats.attackRange);
}

void Building::SetAttack(Unit* unit)
{
	if (state != BS_DEAD)
	{
		attackingUnit = unit;
		state = BS_ATTACKING;
		if (type == PHOTON_CANNON && attackTimer.IsStopped())
		{
			animation.firstRect = animation.currentRect = 0;
		animation.lastRect = 3;
		animation.animSpeed = 15;
		animation.loopable = false;
		animation.loopEnd = false;
		animation.type = A_DOWN;
		}
		attackTimer.Start();
	}
}
/*
void Building::Attack()
{
	if (attackingUnit && attackingUnit->GetState() != STATE_DIE)
	{

	}
}
*/
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
					App->missiles->AddMissil({ (float)buildingCenter.x, (float)buildingCenter.y }, attackingUnit, stats.damage, DRAGOON_MISSILE);
					attackTimer.Start();
					break;
				}
				case (SUNKEN_COLONY) :
				{

					if (animation.currentRect == 3)
					{
						animation.firstRect = animation.currentRect = 3;
						animation.lastRect = 13;
						animation.loopable = false;
						animation.loopEnd = false;
						animation.animSpeed = 10;
						animation.type = A_RIGHT;
					}
					else if (animation.loopEnd)
					{
						animation.firstRect = animation.currentRect = 13;
						animation.lastRect = 3;
						animation.loopable = false;
						animation.loopEnd = false;
						animation.animSpeed = 10;
						animation.type = A_LEFT;
						App->missiles->AddMissil({ (float)buildingCenter.x, (float)buildingCenter.y }, attackingUnit, stats.damage, SUNKEN_MISSILE);
					}

					break;
				}
				case (SPORE_COLONY) :
				{
					App->missiles->AddMissil({ (float)buildingCenter.x, (float)buildingCenter.y }, attackingUnit, stats.damage, MUTALISK_MISSILE);
					attackTimer.Start();
					break;
				}
				}


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
	if (stats.player == PLAYER)
	{
		shieldTimer.Start();
		App->minimap->PingOnWorld(collider.x, collider.y);
	}

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
			if (stats.player == COMPUTER && !RegenHP())
			{
				shieldTimer.Start();
			}
			if (state != BS_DEAD)
			{
				//Updating basic fire
				if (currHP < maxHP / 5)
				{
					if (stats.player == PLAYER)
					{
						fire.sprite.section.y = 96;
						fire2.sprite.section.y = 96;
						fire3.sprite.section.y = 96;
					}
				}

				else if (currHP < maxHP / 2)
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
	return true;
}

void Building::RegenShield()
{
	//Protoss shield regeneration
	if (stats.player == PLAYER)
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
	//Zerg hp regeneration
	if (stats.player == COMPUTER)
	{
		if (shieldTimer.ReadSec() >= 3.7)
		{
			if (RegenHP())
			{
				shieldTimer.Stop();
			}
			else
			{
				shieldTimer.Start();
			}
		}
	}
}

bool Building::RegenHP()
{
	currHP += 1;
	if (currHP > maxHP)
	{
		currHP = maxHP;
		return true;
	}
	return false;
}

void Building::AddNewUnit(Unit_Type unitType, int creationTime, int unitPsi)
{
	if (queue.count < 5)
	{
		queue.Add(unitType, creationTime, unitPsi);
	}
	if (queue.count == 1)
	{
		if (App->player->CanBeCreated(0, 0, unitPsi))
		{
			App->player->AddPsi(unitPsi);
			queue.Start();
		}
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
	if (!queue.stopped)
	{
		if (queue.Update())
		{
			CreateUnit(queue.Pop(), stats.player);
			if (queue.count)
			{
				if (App->player->CanBeCreated(0, 0, *queue.psiList.begin()))
				{
					App->player->AddPsi(*queue.psiList.begin());
					queue.Start();
				}
				else
				{
					queue.Stop();
				}
			}
			else
			{
				queue.Stop();
			}
		}
	}
	else if (queue.count)
	{
		if (App->player->CanBeCreated(0, 0, *queue.psiList.begin(), false))
		{
			App->player->AddPsi(*queue.psiList.begin());
			queue.Start();
		}

	}
}

float Building::GetQueuePercentage()
{
	return queue.GetPercentage();
}

void Building::RemoveFromQueue(int position)
{
	if (queue.count && queue.count > position)
	{
		if (position == 0)
		{
			App->player->SubstractPsi(*queue.psiList.begin());
		}
		const UnitStatsData* unitStats = App->entityManager->GetUnitStats(*queue.units.begin());
		App->player->AddGas(unitStats->gasCost);
		App->player->AddMineral(unitStats->mineralCost);
		queue.Remove(position);

		if (position == 0 && queue.count)
		{
			if (queue.count)
			{
				if (App->player->CanBeCreated(0, 0, *queue.psiList.begin(), false))
				{
					App->player->AddPsi(*queue.psiList.begin());
					queue.Start();
				}
				else
					queue.Stop();
			}
			else
			{
				Stop();
			}
		}

	}


}

void Building::UpdateSpawn(float dt)
{
	spawn_animation.Update(dt);
	currHP = (logicTimer.ReadSec() / buildTime) * maxHP;
	stats.shield = (logicTimer.ReadSec() / buildTime) * stats.maxShield;
	if (logicTimer.ReadSec() >= buildTime)
	{
		FinishSpawn();
	}
}

void Building::FinishSpawn()
{
	App->entityManager->PlayBuildingSound(type, race, sound_ready, { (float)collider.x, (float)collider.y });
	currHP = maxHP;
	stats.shield = stats.maxShield;
	state = BS_DEFAULT;
	finished = true;
	if (type == ASSIMILATOR)
	{
		if (gasResource)
			gasResource->active = false;
	}
	else if (type == PYLON)
	{
		App->entityManager->UpdatePower(position.x, position.y, true);
	}

	App->entityManager->FinishBuildingSpawn(this);
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
	App->entityManager->PlayBuildingSound(type, race, sound_death, { (float)collider.x, (float)collider.y });
	state = BS_DEAD;
	HPBar->SetActive(false);
	if (App->entityManager->selectedBuilding == this)
	{
		App->entityManager->UnselectBuilding(this);
	}
	App->player->SubstractMaxPsi(psi);

	if (App->entityManager->hoveringBuilding == this)
	{
		App->entityManager->hoveringBuilding = NULL;
	}
	App->entityManager->RemoveBuildingCount(type);
	
	if (type != ASSIMILATOR)
		ChangeTileWalkability(true);
	else
	{
		gasResource->active = true;
		gasResource->ocupied = false;
	}

	if (type == PYLON)
		App->entityManager->ChangePowerTile(position.x, position.y, false);

	logicTimer.Start();
}

void Building::Destroy()
{

}

void Building::SpawnUnits()
{
	while (queue.count)
	{
		CreateUnit(queue.Pop(), stats.player);
	}
}

void Building::LoadLibraryData()
{
	iPoint pos = App->pathFinding->MapToWorld(position.x, position.y);

	//Loading all stats data
	const BuildingStatsData* statsData = App->entityManager->GetBuildingStats(type);

	name = statsData->name;
	race = statsData->race;
	maxHP = currHP = statsData->HP;
	stats.shield = stats.maxShield = statsData->shield;
	armor = statsData->armor;
	width_tiles = statsData->width_tiles;
	height_tiles = statsData->height_tiles;
	buildTime = statsData->buildTime;
	psi = statsData->psi;
	
	stats.damage = statsData->damage;
	stats.attackRange = statsData->attackRange;

	//Loading all sprites data
	const BuildingSpriteData* spriteData = App->entityManager->GetBuildingSprite(type);
	animation.sprite.texture = spriteData->texture;
	animation.rect_size_x = animation.sprite.section.w = spriteData->size_x;
	animation.rect_size_y = animation.sprite.section.h = spriteData->size_y;
	animation.type = A_VERTICAL;
	animation.firstRect = spriteData->anim_column_start;
	animation.lastRect = spriteData->anim_column_end;
	animation.sprite.y_ref = pos.y + (statsData->height_tiles - 1) * 16;
	animation.sprite.position.x = pos.x - spriteData->offset_x;
	animation.sprite.position.y = pos.y - spriteData->offset_y;
	animation.sprite.position.w = animation.sprite.position.h = 0;
	animation.sprite.section.x = animation.sprite.section.y = 0;

	animation.animSpeed = spriteData->animSpeed;

	//Loading shadow data
	shadow.sprite.texture = spriteData->shadow.texture;
	shadow.sprite.section = animation.sprite.section;
	shadow.rect_size_x = shadow.sprite.section.w;
	shadow.rect_size_y = shadow.sprite.section.h;
	shadow.animSpeed = spriteData->shadow.animation_speed;
	shadow.firstRect = spriteData->shadow.column_start;
	shadow.lastRect = spriteData->shadow.column_end;
	shadow.animSpeed = spriteData->shadow.animation_speed;
	shadow.type = A_DOWN;
	shadow.sprite.position = { 0, 0, 0, 0 };
	shadow.sprite.position.x = pos.x - spriteData->shadow.offset_x;
	shadow.sprite.position.y = pos.y - spriteData->shadow.offset_y;
	shadow.sprite.y_ref = position.y - 1;
	shadow.sprite.tint = { 0, 0, 0, 130 };
	shadow.sprite.position.w = shadow.sprite.position.h = 0;
	shadow.sprite.section.x = shadow.sprite.section.y = 0;

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
	const HPBarData* HPBar_data = App->entityManager->GetHPBarSprite(HPBar_type - 1);
	HPBar = App->gui->CreateUI_HPBar({ pos.x + collider.w / 2 - HPBar_data->size_x / 2, pos.y + collider.h + 10, HPBar_data->size_x, HPBar_data->size_y }, HPBar_data->fill, HPBar_data->shield, HPBar_data->empty, &maxHP, &currHP, &stats.maxShield, &stats.shield);
	HPBar->SetActive(false);
	HPBar->sprite.useCamera = true;
	//Fire texture
	if (race == PROTOSS)
	{
		fire = C_Animation(App->entityManager->fire1);
		fire2 = C_Animation(App->entityManager->fire2);
		fire3 = C_Animation(App->entityManager->fire3);		
	}
	else if (race == ZERG)
	{
		fire = C_Animation(App->entityManager->blood1);
		fire2 = C_Animation(App->entityManager->blood2);
		fire3 = C_Animation(App->entityManager->blood3);
	}

	if (type == PYLON)
	{
		pylonArea = C_Sprite(App->entityManager->pylonArea);
		pylonArea.position.x = pos.x + collider.w / 2 - pylonArea.section.w / 2;
		pylonArea.position.y = pos.y + collider.h / 2 - pylonArea.section.h / 2;
	}

	fire.sprite.position.x = pos.x + collider.w / 2 - 32;
	fire.sprite.position.y = pos.y + collider.h / 2 - 48;

	fire2.sprite.position.x = pos.x + collider.w / 3 - 32;
	fire2.sprite.position.y = pos.y + collider.h / 2 - 48;

	fire3.sprite.position.x = pos.x + collider.w / 2 - 32;
	fire3.sprite.position.y = pos.y + collider.h / 3 - 48;

	fire3.sprite.y_ref = fire2.sprite.y_ref = fire.sprite.y_ref = animation.sprite.y_ref + 1;

	//Spawn animation
	spawn_animation = C_Animation(App->entityManager->building_spawn_animation);
	spawn_animation.sprite.position.x = pos.x + collider.w / 2 - 25;
	spawn_animation.sprite.position.y = pos.y + collider.h / 2 - 25;
	spawn_animation.sprite.y_ref = animation.sprite.y_ref;

	//Death animation
	if (race == PROTOSS)
	{
		if (width_tiles / 2 + height_tiles / 2< 6)
		{
			death_animation = C_Animation(App->entityManager->protoss_rubble_s);
			death_animation.sprite.position.x = pos.x + collider.w / 2 - 48;
			death_animation.sprite.position.y = pos.y + collider.h / 2 - 48;
		}
		else
		{
			death_animation = C_Animation(App->entityManager->protoss_rubble_l);
			death_animation.sprite.position.x = pos.x + collider.w / 2 - 64;
			death_animation.sprite.position.y = pos.y + collider.h / 2 - 64;
		}
	}
	else if (race == ZERG)
	{
		if (width_tiles / 2 + height_tiles / 2 < 6)
		{
			death_animation = C_Animation(App->entityManager->zerg_rubble_s);
			death_animation.sprite.position.x = pos.x + collider.w / 2 - 48;
			death_animation.sprite.position.y = pos.y + collider.h / 2 - 48;
		}
		else
		{
			death_animation = C_Animation(App->entityManager->zerg_rubble_l);
			death_animation.sprite.position.x = pos.x + collider.w / 2 - 64;
			death_animation.sprite.position.y = pos.y + collider.h / 2 - 64;
		}
	}
}

void Building::Draw()
{
	SDL_Rect rect = { 0, 0, 64, 64 };

	if (App->entityManager->render)
	{
		if (selected)
			App->render->AddSprite(&base, SCENE);
		if (state != BS_SPAWNING && state != BS_DEAD)
			App->render->AddSprite(&animation.sprite, SCENE);
		else if (state == BS_SPAWNING)
			App->render->AddSprite(&spawn_animation.sprite, SCENE);
		else if (state == BS_DEAD)
		{
			if (death_animation.sprite.texture)
			{
				App->render->AddSprite(&death_animation.sprite, DECAL);
			}
		}
		if (type == PYLON && App->entityManager->createBuilding && state != BS_DEAD && state != BS_SPAWNING)
		{
			App->render->AddSprite(&pylonArea, SCENE);
		}
	}
	if (App->entityManager->shadows && state != BS_SPAWNING && state != BS_DEAD)
	{
		if (shadow.sprite.texture)
		{
			App->render->AddSprite(&shadow.sprite, DECAL);
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
	if (hasWaypoint && selected && stats.player == PLAYER && state != BS_DEAD)
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
		App->render->AddCircle(collider.x + collider.w / 2, collider.y + collider.h / 2, stats.attackRange, true, 255, 0, 0);
		App->render->AddCircle(collider.x + collider.w / 2, collider.y + collider.h / 2, stats.attackRange - 1, true, 255, 0, 0);
	}
}

iPoint Building::GetWorldPosition()
{
	return App->pathFinding->MapToWorld(position.x, position.y);
}