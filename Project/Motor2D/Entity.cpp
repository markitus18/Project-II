#include "Entity.h"

Entity::Entity()
{

}

Entity::Entity(Entity* entity)
{

}

Entity::~Entity(){}

bool Entity::Start()
{
	return true;
}

bool Entity::Update(float dt)
{
	return true;
}

bool Entity::Destroy()
{

	return true;
}

//Setters
void Entity::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
}
void Entity::SetPosition(iPoint _position)
{
	position = _position;
}
void Entity::SetHP(int _HP)
{
	HP = _HP;
}

iPoint Entity::GetPosition()
{
	return position;
}

int Entity::GetHP()
{
	return HP;
}

