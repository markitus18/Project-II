#include "j1Module.h"
#include "M_Player.h"

#include "j1App.h"

#include "M_GUI.h"
#include "UI_Element.h"
#include "M_InputManager.h"
#include "M_Audio.h"

M_Player::M_Player(bool _active) : j1Module(_active)
{
	name.create("player");
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

	sfx_script_adquire = App->audio->LoadFx("sounds/ui/button.ogg");
	not_enough_res_font = App->font->Load("fonts/StarCraft.ttf", 12);

	fx_not_enough_minerals = App->audio->LoadFx("sounds/protoss/units/advisor/err00.ogg");
	fx_not_enough_gas = App->audio->LoadFx("sounds/protoss/units/advisor/err01.ogg");
	fx_need_more_pylons = App->audio->LoadFx("sounds/protoss/units/advisor/err02.ogg");

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
		if (not_enough_gas->IsActive())
			not_enough_gas->SetActive(false);
		if (not_enough_minerals->IsActive())
			not_enough_minerals->SetActive(false);
		if (need_more_pylons->IsActive())
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

// Load Game State
bool M_Player::Load(pugi::xml_node& data)
{
	stats.mineral = data.attribute("mineral").as_int();
	stats.gas = data.attribute("gas").as_int();
	stats.maxPsi = data.attribute("maxPsi").as_int();
	stats.psi = data.attribute("psi").as_int();
	stats.realMaxPsi = data.attribute("realMaxPsi").as_int();

	return true;
}

// Save Game State
bool M_Player::Save(pugi::xml_node& data) const
{
	data.append_attribute("mineral") = stats.mineral;
	data.append_attribute("gas") = stats.gas;
	data.append_attribute("maxPsi") = stats.maxPsi;
	data.append_attribute("psi") = stats.psi;
	data.append_attribute("realMaxPsi") = stats.realMaxPsi;

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
	if (stats.realMaxPsi < 0)
	{
		stats.realMaxPsi = 0;
	}

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
		}
		return false;
	}

	if (stats.gas < gas)
	{
		if (warning)
		{
			DisplayGasFeedback();
		}
		return false;
	}

	if (psi)
	{
		if (stats.psi + psi > stats.maxPsi)
		{
			if (warning)
			{
			DisplayPsiFeedback();
			}
			return false;
		}
	}


	return true;
}

void M_Player::DisplayMineralFeedback()
{
	if (not_enough_gas->IsActive())
	{
		not_enough_gas->SetActive(false);
	}
	if (need_more_pylons->IsActive())
	{
		need_more_pylons->SetActive(false);
	}
	not_enough_minerals->SetActive(true);
	feedbackText_timer.Start();
	App->audio->PlayFx(fx_not_enough_minerals);
}

void M_Player::DisplayGasFeedback()
{
	if (not_enough_minerals->IsActive())
	{
		not_enough_minerals->SetActive(false);
	}
	if (need_more_pylons->IsActive())
	{
		need_more_pylons->SetActive(false);
	}
	not_enough_gas->SetActive(true);
	feedbackText_timer.Start();
	App->audio->PlayFx(fx_not_enough_gas);
}

void M_Player::DisplayPsiFeedback()
{
	if (not_enough_gas->IsActive())
	{
		not_enough_gas->SetActive(false);
	}
	if (not_enough_gas->IsActive())
	{
		not_enough_gas->SetActive(false);
	}
	need_more_pylons->SetActive(true);
	feedbackText_timer.Start();
	App->audio->PlayFx(fx_need_more_pylons);
}

