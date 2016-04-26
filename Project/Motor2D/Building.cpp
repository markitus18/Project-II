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
Building::Building(int x, int y, Building_Type _type) : Controlled()
{
	position.x = x;
	position.y = y;
	type = _type;
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
		animation.Update(dt);
		UpdateQueue();

		if (type == PHOTON_CANNON)
		{
			if (state == BS_DEFAULT && attackTimer.ReadSec() >= 2)
			{
				animation.animSpeed = 15;
				animation.type = A_UP;
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
		animation.type = A_DOWN;
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
				App->missiles->AddMissil({ (float)buildingCenter.x, (float)buildingCenter.y }, attackingUnit, 100, DRAGOON_MISSILE);
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
					fire.sprite.section.y = 96;
				else if (currHP < maxHP * 2 / 3)
					fire.sprite.section.y = 0;
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
	animation.firstRect = 0;
	animation.lastRect = 3;
	animation.sprite.y_ref = pos.y + (statsData->height_tiles - 1) * 8;
	animation.sprite.useCamera = true;
	animation.sprite.position.x = pos.x - spriteData->offset_x;
	animation.sprite.position.y = pos.y - spriteData->offset_y;
	animation.animSpeed = 0;

	//Loading shadow data
	shadow.texture = spriteData->shadow.texture;
	shadow.section = animation.sprite.section;
	shadow.position = { 0, 0, 0, 0 };
	shadow.position.x = pos.x - spriteData->shadow.offset_x;
	shadow.position.y = pos.y - spriteData->shadow.offset_y;
	shadow.y_ref = position.y - 1;
	shadow.tint = { 0, 0, 0, 130 };

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
	fire = C_Animation(App->entityManager->fire1);
	fire.sprite.position.x = pos.x + collider.w / 2 - 32;
	fire.sprite.position.y = pos.y + collider.h / 2 - 48;
	fire.sprite.y_ref = animation.sprite.y_ref + 1;
}

void Building::Draw()
{
	SDL_Rect rect = { 0, 0, 64, 64 };

	if (App->entityManager->render)
	{
		if (selected)
			App->render->AddSprite(&base, SCENE);
		App->render->AddSprite(&animation.sprite, SCENE);
	}
	if (App->entityManager->shadows)
	{
		if (shadow.texture)
		{
			App->render->AddSprite(&shadow, SCENE);
		}
		if (currHP < maxHP / 2)
		{
			if (fire.sprite.texture)
			{
				App->render->AddSprite(&fire.sprite, SCENE);
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
}

iPoint Building::GetWorldPosition()
{
	return App->pathFinding->MapToWorld(position.x, position.y);
}