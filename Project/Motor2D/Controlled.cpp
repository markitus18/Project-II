#include <stdlib.h>

#include "Entity.h"
#include "Controlled.h"

#include "M_Render.h"
#include "j1App.h"
#include "M_Map.h"

#include "EntityManager.h"

#include "j1App.h"
#include "M_GUI.h"
#include "UI_Element.h"

//Scene Unit shouldnt be necessary to include after removing draw condition
#include "S_SceneUnit.h"

#include "M_PathFinding.h"
#include "C_Vec2.h"

Controlled::Controlled() :Entity()
{
	CreateBar();
}
Controlled::Controlled(float x, float y) : Entity(x, y)
{
	CreateBar();
}
Controlled::Controlled(fPoint pos) : Entity(pos)
{
	CreateBar();
}
Controlled::~Controlled()
{}

bool Controlled::Start()
{
	iPoint pos;
	pos.x = (int)position.x;
	pos.y = (int)position.y;
	/*
	HPBar->Center(pos);
	HPBar->SetLocalPosition(HPBar->GetLocalPosition().x, HPBar->GetLocalPosition().y - 60);
	*/
	HPBar_Empty->localPosition.x = position.x;
	HPBar_Empty->localPosition.y = position.y - 60;
	
	return true;
}

bool Controlled::Update(float dt)
{
	Draw();
	return true;
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

int Controlled::GetHP() const
{
	return currHP;
}

void Controlled::CreateBar()
{
	HPBar_Empty = App->gui->CreateUI_Image({ 150, 150, 0, 0 }, App->entityManager->hpBar_empty, { 0, 0, 107, 9 });
	HPBar_Filled = App->gui->CreateUI_ProgressBar({ 0, 0, 0, 0 }, App->entityManager->hpBar_filled, &maxHP, &currHP, { 2, 2, 103, 5 });
	HPBar_Empty->SetActive(false);
	HPBar_Filled->SetActive(false);
	HPBar_Empty->useCamera = HPBar_Filled->useCamera = true;
}

void Controlled::UpdateBarState()
{
	if (selected)
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

void Controlled::UpdateBarPosition()
{
	HPBar_Empty->localPosition.x = position.x - 50;
	HPBar_Empty->localPosition.y = position.y - 70;

	HPBar_Filled->localPosition.x = position.x - 48;
	HPBar_Filled->localPosition.y = position.y - 68;
}

void Controlled::UpdateBarTexture()
{
	if (currHP <= 20)
	{
		if (HPBar_Filled->GetTexture() != App->entityManager->hpBar_low)
			HPBar_Filled->SetTexture(App->entityManager->hpBar_low);
	}
	else if (currHP <= 50)
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

