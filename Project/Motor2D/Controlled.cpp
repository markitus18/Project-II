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
	/*
	UIRect* rect1 = App->gui->CreateRect("maxHO", { 0, 0, 150, 20 }, 0, 0, 0);
	UIRect* rect2 = App->gui->CreateRect("CurrentHP", { 5, 5, 140, 10 }, 255, 0, 0);
	HPBar = App->gui->CreateBar("HP Bar", (UIElement*)rect1, (UIElement*)rect2, &maxHP, &currHP);
	HPBar->SetCamera(true);
	*/
}

