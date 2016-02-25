#include "Entity.h"

Entity::Entity()
{
	position.x = position.y = 0;
}
Entity::Entity(fPoint pos)
{
	position = pos;
}
Entity::Entity(float x, float y)
{
	position.x = x;
	position.y = y;
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
void Entity::SetPosition(fPoint _position)
{
	position = _position;
}

fPoint Entity::GetPosition()
{
	return position;
}

