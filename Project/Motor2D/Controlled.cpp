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

int Controlled::GetSizeType() const
{
	return 1; // Parche
}

Unit_Movement_Type Controlled::GetMovementType() const
{
	return movementType;
}

void Controlled::CreateBar()
{
	HPBar_Empty = App->gui->CreateUI_Image({ 0, 0, 0, 0 }, App->entityManager->hpBar_empty, { 0, 0, 31, 7 });
	HPBar_Filled = App->gui->CreateUI_ProgressBar({ 0, 0, 0, 0 }, App->entityManager->hpBar_filled, &maxHP, &currHP, { 0, 0, 31, 7});
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
	HPBar_Empty->sprite.useCamera = HPBar_Filled->sprite.useCamera = true;
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
	if (currHP < maxHP / 3)
	{
		if (HPBar_Filled->GetTexture() != App->entityManager->hpBar_low)
			HPBar_Filled->SetTexture(App->entityManager->hpBar_low);
	}
	else if (currHP < maxHP * 2 / 3)
	{
		if (HPBar_Filled->GetTexture() != App->entityManager->hpBar_mid)
			HPBar_Filled->SetTexture(App->entityManager->hpBar_mid);
	}
	else
	{
		if (HPBar_Filled->GetTexture() != App->entityManager->hpBar_filled)
			HPBar_Filled->SetTexture(App->entityManager->hpBar_filled);
	}
}

