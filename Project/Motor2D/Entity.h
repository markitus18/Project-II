#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "C_Point.h"
#include "SDL/include/SDL.h"

class Entity
{
	//Public methods
public:

	Entity();
	Entity(fPoint);
	Entity(float x, float y);
	~Entity();

	virtual bool Start();
	virtual bool Update(float dt);
	virtual void Destroy();

	//Set methods
	void SetPosition(float, float);
	void SetPosition(fPoint);
	void SetCollider(SDL_Rect rect);
	//Get methods
	fPoint GetPosition() const;
	SDL_Rect GetCollider() const;

	//Private methods
private:

	//Public properties
public:
	bool selected = false;
	//Private properties
protected:
	fPoint position = { 0, 0 };
	SDL_Rect collider;
	SDL_Texture* texture;
};
#endif //__ENTITY_H