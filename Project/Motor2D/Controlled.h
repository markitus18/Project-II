#ifndef __CONTROLLED_H__
#define __CONTROLLED_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"

#include "Entity.h"

class UIBar;

class Controlled : public Entity
{
public:
	Controlled();
	Controlled(float x, float y);
	Controlled(fPoint);
	~Controlled();

	virtual bool Start();
	virtual bool Update(float dt);

	//Drawing methods
	virtual void Draw();
	virtual void DrawDebug();

	void SetHP(int newHP);

	int GetHP() const;

	//HP controllers
	virtual void CreateBar();
	
private:

public:

protected:
	int maxHP = 100;
	int currHP = 100;
	UIBar* HPBar;

};

#endif //__CONTROLLED_H__