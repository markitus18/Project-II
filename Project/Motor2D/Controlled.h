#ifndef __CONTROLLED_H__
#define __CONTROLLED_H__

#include "Entity.h"

class UI_ProgressBar;
class UI_Image;

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

	//HP controllers
	virtual void CreateBar();
	virtual void UpdateBarPosition();

	void UpdateBarState();
	void UpdateBarTexture();
private:

public:
	int psi;

protected:
	int maxHP = 100;
	int currHP = 100;

	Unit_Movement_Type movementType = GROUND;
	UI_Image* HPBar_Empty;
	UI_ProgressBar* HPBar_Filled;

};

#endif //__CONTROLLED_H__