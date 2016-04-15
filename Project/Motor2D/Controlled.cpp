#include <stdlib.h>

#include "Entity.h"
#include "Controlled.h"

#include "M_Render.h"
#include "j1App.h"

#include "M_EntityManager.h"

#include "j1App.h"
#include "M_GUI.h"
#include "UI_Element.h"

#include "M_PathFinding.h"
#include "C_Vec2.h"

Controlled::Controlled() :Entity()
{
}

Controlled::~Controlled()
{
}

bool Controlled::Start()
{
	
	return true;
}

bool Controlled::Update(float dt)
{
	Draw();
	return true;
}

void Controlled::SetActive(bool _active)
{
	if (!_active)
	{
		if (App->entityManager->hoveringUnit == (Unit*)this)
			App->entityManager->hoveringUnit = NULL;
		if (App->entityManager->hoveringBuilding == (Building*)this)
			App->entityManager->hoveringBuilding = NULL;
		if (App->entityManager->hoveringResource == (Resource*)this)
			App->entityManager->hoveringResource = NULL;
	}

	active = _active;
	UpdateBarState();
}

void Controlled::Draw()
{

}

void Controlled::DrawDebug()
{

}

void Controlled::SetHP(int newHP)
{
	currHP = newHP;
}

void Controlled::SetMovementType(Unit_Movement_Type _type)
{
	movementType = _type;
}

int Controlled::GetHP() const
{
	return currHP;
}

Unit_Movement_Type Controlled::GetMovementType() const
{
	return movementType;
}

void Controlled::UpdateBarState()
{
	if (GetHP() > 0)
	{
		if (active && selected)
		{
			HPBar_Empty->SetActive(true);
			HPBar_Filled->SetActive(true);
		}
		else
		{
			HPBar_Empty->SetActive(false);
			HPBar_Filled->SetActive(false);
		}
	}
}

void Controlled::UpdateBarPosition()
{

}

void Controlled::UpdateBarTexture()
{
	if (currHP >= 0 && currHP <= maxHP)
	{
		if (currHP < maxHP / 3)
		{
			HPBar_Filled->sprite.section.y = 14;
			HPBar_Filled->rect.y = 14;
		}
		else if (currHP < maxHP * 2 / 3)
		{
			HPBar_Filled->sprite.section.y = 7;
			HPBar_Filled->rect.y = 7;
		}
		else
		{
			HPBar_Filled->sprite.section.y = 0;
			HPBar_Filled->rect.y = 0;
		}
		if (App->entityManager->debug)
		{
			LOG("Sprite section y: %i", HPBar_Filled->sprite.section.y);
		}
	}
}

