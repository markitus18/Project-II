#include "j1Module.h"
#include "M_Player.h"

#include "j1App.h"

#include "M_GUI.h"
#include "UI_Element.h"
#include "M_InputManager.h"
#include "M_Audio.h"

M_Player::M_Player(bool _active) : j1Module(_active)
{
	name.create("pathFinding");
}

M_Player::~M_Player()
{

}

bool M_Player::Awake(pugi::xml_node& node)
{
	return true;
}

bool M_Player::Start()
{
	int w, h, scale;
	w = App->events->GetScreenSize().x;
	h = App->events->GetScreenSize().y;
	scale = App->events->GetScale();

	sfx_script_adquire = App->audio->LoadFx("sounds/ui/button.wav");
	not_enough_res_font = App->font->Load("fonts/StarCraft.ttf", 12);

	//Loading all feedback text
	not_enough_minerals = App->gui->CreateUI_Label({ w / 2 / scale - 110, h / scale - 180, 0, 0 }, "You have not enough minerals.", not_enough_res_font);
	not_enough_minerals->SetActive(false);
	not_enough_gas = App->gui->CreateUI_Label({ w / 2 / scale - 110, h / scale - 180, 0, 0 }, "You have not enough gas.", not_enough_res_font);
	not_enough_gas->SetActive(false);
	need_more_pylons = App->gui->CreateUI_Label({ w / 2 / scale - 150, h / scale - 180, 0, 0 }, "You need aditional pylons... Build more pylons.", not_enough_res_font);
	need_more_pylons->SetActive(false);

	stats.mineral = 80;
	return true;
}

bool M_Player::Update(float dt)
{
	if (feedbackText_timer.ReadSec() >= 5)
	{
		if (not_enough_gas->GetActive())
			not_enough_gas->SetActive(false);
		if (not_enough_minerals->GetActive())
			not_enough_minerals->SetActive(false);
		if (need_more_pylons->GetActive())
			need_more_pylons->SetActive(false);
		feedbackText_timer.Start();
	}
	return true;
}

bool M_Player::CleanUp()
{
	App->gui->DeleteUIElement(not_enough_minerals);
	App->gui->DeleteUIElement(not_enough_gas);
	App->gui->DeleteUIElement(need_more_pylons);
	return true;
}

void M_Player::AddMineral(int amount)
{
	stats.mineral += amount;
}

void M_Player::AddGas(int amount)
{
	stats.gas += amount;
}

void M_Player::AddPsi(int amount)
{
	stats.psi += amount;
}

void M_Player::AddMaxPsi(int amount)
{
	stats.realMaxPsi += amount;
	stats.maxPsi += amount;

	if (stats.maxPsi > 200)
	{
		stats.maxPsi = 200;
	}
}

void M_Player::SubstractMineral(int amount)
{
	stats.mineral -= amount;
	if (stats.mineral < 0)
		stats.mineral = 0;
}

void M_Player::SubstractGas(int amount)
{
	stats.gas -= amount;
	if (stats.gas < 0)
		stats.gas = 0;
}

void M_Player::SubstractPsi(int amount)
{
	stats.psi -= amount;
	if (stats.psi < 0)
		stats.psi = 0;
}

void M_Player::SubstractMaxPsi(int amount)
{
	stats.realMaxPsi -= amount;
	stats.maxPsi = stats.realMaxPsi;

	if (stats.maxPsi > 200)
	{
		stats.maxPsi = 200;
	}
}

bool M_Player::CanBeCreated(int mineral, int gas, int psi, bool warning)
{
	if (stats.mineral < mineral)
	{
		if (warning)
		{
			DisplayMineralFeedback();
			App->audio->PlayFx(sfx_script_adquire);
		}
		return false;
	}

	if (stats.gas < gas)
	{
		if (warning)
		{
			DisplayGasFeedback();
			App->audio->PlayFx(sfx_script_adquire);
		}
		return false;
	}

	if (stats.psi + psi > stats.maxPsi)
	{
		if (warning)
		{
		DisplayPsiFeedback();
		App->audio->PlayFx(sfx_script_adquire);
		}
		return false;
	}

	return true;
}

void M_Player::DisplayMineralFeedback()
{
	if (not_enough_gas->GetActive())
	{
		not_enough_gas->SetActive(false);
	}
	if (need_more_pylons->GetActive())
	{
		need_more_pylons->SetActive(false);
	}
	not_enough_minerals->SetActive(true);
	feedbackText_timer.Start();
	App->audio->PlayFx(sfx_script_adquire);
}

void M_Player::DisplayGasFeedback()
{
	if (not_enough_minerals->GetActive())
	{
		not_enough_minerals->SetActive(false);
	}
	if (need_more_pylons->GetActive())
	{
		need_more_pylons->SetActive(false);
	}
	not_enough_gas->SetActive(true);
	feedbackText_timer.Start();
	App->audio->PlayFx(sfx_script_adquire);
}

void M_Player::DisplayPsiFeedback()
{
	if (not_enough_gas->GetActive())
	{
		not_enough_gas->SetActive(false);
	}
	if (not_enough_gas->GetActive())
	{
		not_enough_gas->SetActive(false);
	}
	need_more_pylons->SetActive(true);
	feedbackText_timer.Start();
	App->audio->PlayFx(sfx_script_adquire);
}

