#pragma once

#include "Entity.h"

enum class BrickColour {BLUE, GREEN, GREY, PURPLE, RED, YELLOW};

//externed sprites
//extern Sprite *redBrickSprite;
//extern Sprite *yellowBrickSprite;
//extern Sprite *orangeBrickSprite;
//extern Sprite *purpleBrickSprite;

extern Sprite* blueBrickSprite;
extern Sprite* greenBrickSprite;
extern Sprite* greyBrickSprite;
extern Sprite* purpleBrickSprite;
extern Sprite* redBrickSprite;
extern Sprite* yellowBrickSprite;

class BrickEntity : public Entity
{
public:
	BrickColour colour;
	BrickEntity()
	{
		typeID = ENTITYBRICK;
		colour = BrickColour::RED;
	}

	bool HandleCollision();
};

BrickEntity * MakeBrick(BrickColour type, float xpos, float ypos);

bool LoadMap(std::string fileName, std::vector<Entity*>& brickList);