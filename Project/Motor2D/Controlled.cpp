#include <stdlib.h>

#include "Entity.h"
#include "Controlled.h"

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
	if (HPBar)
		App->gui->DeleteUIElement(HPBar);
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

void Controlled::RegenShield()
{

}

void Controlled::UpdateBarState()
{
	if (GetHP() > 0)
	{
		HPBar->SetActive((active && selected));
	}
}

void Controlled::UpdateBarPosition()
{

}

