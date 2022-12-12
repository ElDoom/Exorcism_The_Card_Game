#include "BrickEntity.h"
#include "CollisionMask.h"
//enum class BrickColour {BLUE, GREEN, GREY, PURPLE, RED, YELLOW};
//Decide whether to change colour or die off:
//return true if this pbject should be removed
bool BrickEntity::HandleCollision()
{
	bool retval = false;
	switch(colour)
	{
	case BrickColour::RED:
		colour = BrickColour::GREEN;
		sprite = greenBrickSprite;
		break;

	case BrickColour::GREEN:
		colour = BrickColour::YELLOW;
		sprite = yellowBrickSprite;
		break;

	case BrickColour::YELLOW:
	default: //because PURPLE wasn't handled yet, it will be handled by the default case
		retval = true;
		break;
	}

	return retval;
}

extern b2World *world;

BrickEntity * MakeBrick(BrickColour type, float xpos, float ypos)
{
	BrickEntity *brickEntity = new BrickEntity();
	brickEntity->colour = type;

	//set the sprite to draw with
	//enum class BrickColour {BLUE, GREEN, GREY, PURPLE, RED, YELLOW};
	switch(type)
	{
	case BrickColour::BLUE:
		brickEntity->sprite = blueBrickSprite;
		break;

	case BrickColour::GREEN:
		brickEntity->sprite = greenBrickSprite;
		break;	
	
	case BrickColour::GREY:
		brickEntity->sprite = greyBrickSprite;
		break;

	case BrickColour::PURPLE:
		brickEntity->sprite = purpleBrickSprite;
		break;

	case BrickColour::RED:
		brickEntity->sprite = redBrickSprite;
		break;

	case BrickColour::YELLOW:
		brickEntity->sprite = yellowBrickSprite;
		break;

	default:
		brickEntity->sprite = greyBrickSprite;
		break;

	}

	//make the physics body
	b2BodyDef brickBodyDef;

	//set the position of the center of the body, 
	//converting from pxel coords to physics measurements
	brickBodyDef.position.Set(xpos / PTM_RATIO, ypos / PTM_RATIO);
	brickBodyDef.type = b2_kinematicBody; //make it a kinematic body i.e. one moved by us

	//make the userdata point back to this entity
	brickBodyDef.userData.pointer = reinterpret_cast<uintptr_t>(brickEntity);

	brickEntity->body = world->CreateBody(&brickBodyDef); //create the body and add it to the world

	// Define a box shape for our dynamic body.
	b2PolygonShape boxShape;
	//SetAsBox() takes as arguments the half-width and half-height of the box
	boxShape.SetAsBox(64.0f / (2.f*PTM_RATIO), 32.0f / (2.f*PTM_RATIO));

	b2FixtureDef brickFixtureDef;
	brickFixtureDef.shape = &boxShape;
	brickFixtureDef.density = 1.0f; //won't matter, as we made this kinematic
	brickFixtureDef.restitution = 0;
	brickFixtureDef.friction = 0.1f;

	//collison masking
	brickFixtureDef.filter.categoryBits = CMASK_BRICK;  //this is a brick
	brickFixtureDef.filter.maskBits = CMASK_BALL;		//it collides wth balls

	brickEntity->body->CreateFixture(&brickFixtureDef);
	
	return brickEntity;
}

bool LoadMap(std::string fileName, std::vector<Entity*>& brickList)
{
	//clear the current brickList
	for (auto B : brickList) delete B;
	brickList.clear();

	//open file
	std::ifstream myfile;
	myfile.open(fileName);

	if (myfile.is_open())
	{
		//read in # of bricks
		int brickNum = 0;
		myfile >> brickNum;
		BrickColour colour;
		//read in each brick
		////enum class BrickColour {BLUE, GREEN, GREY, PURPLE, RED, YELLOW};
		for (; brickNum > 0; --brickNum)
		{
			//make a brick
			BrickEntity* B;
			int t = 0;
			int x = 0, y = 0;
			myfile >> t;
			switch (t)
			{
			case 0:
				colour = BrickColour::BLUE;
				break;
			case 1:
				colour = BrickColour::GREEN;
				break;
			case 2:
				colour = BrickColour::GREY;
				break;
			case 3:
				colour = BrickColour::PURPLE;
				break;
			case 4:
				colour = BrickColour::RED;
				break;
			case 5:
				colour = BrickColour::YELLOW;
				break;
			default:
				colour = BrickColour::GREY;
				break;
			}
			myfile >> x;
			myfile >> y;
			B = MakeBrick(colour,x,y);
			brickList.push_back(B);
		}

		myfile.close();
		return true;
		
	}
	return false;
	
}