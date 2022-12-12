#include "Entity.h"

Entity::Entity()
{
	sprite = NULL;
	body = NULL;
	typeID = ENTITYNONE; //default entity type
}

Entity::~Entity()
{
	
};

void Entity::Draw()
{
	position = body->GetPosition();
	position = Physics2Pixels(position);
	//sprite->angle = rad2deg(body->GetAngle());
	sprite->angle = rad2deg(body->GetAngle());

	// Draw the objects
	sprite->Blit(position.x, position.y);
}

void Entity::Draw(float scale)
{
	position = body->GetPosition();
	position = Physics2Pixels(position);

	sprite->angle = rad2deg(body->GetAngle());

	// Draw the objects
	sprite->Blit(position.x, position.y, scale, scale);
}

void Entity::Update(float seconds)
{
	//empty, provide one if you have something to update, like animation or game logic
}