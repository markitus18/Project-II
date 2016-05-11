#ifndef __CONTROLLED_H__
#define __CONTROLLED_H__

#include "Entity.h"
#include "C_String.h"

class UI_HPBar;
class UI_Image;
struct HPBarData;
enum E_Race;

enum Unit_Movement_Type
{
	GROUND,
	FLYING,
};

class Controlled : public Entity
{
public:
	Controlled();
	~Controlled();

	virtual bool Start();
	virtual bool Update(float dt);

	virtual void SetActive(bool active);

	//Drawing methods
	virtual void Draw();
	virtual void DrawDebug();

	void SetHP(int newHP);
	void SetMovementType(Unit_Movement_Type type);

	int GetHP() const;
	virtual bool Hit(int amount) = 0;
	Unit_Movement_Type GetMovementType() const;

	virtual void RegenShield();
	//HP controllers
	virtual void UpdateBarPosition();

	void UpdateBarState();
private:

public:
	int psi;


	int maxHP = 100;
	int currHP = 100;

	C_String name;
	E_Race race;

protected:
	Unit_Movement_Type movementType = GROUND;

	j1Timer in_combatTimer;
	j1Timer shieldTimer;

	int HPBar_type;
	UI_HPBar* HPBar = NULL;

};

#endif //__CONTROLLED_H__