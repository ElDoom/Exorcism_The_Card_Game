#pragma once

#include "Entity.h"

class BallEntity : public Entity
{
public:
	BallEntity() 
	{
		typeID = ENTITYBALL;
	}
	void Draw();
};

BallEntity * MakeBall(Sprite *sprite);