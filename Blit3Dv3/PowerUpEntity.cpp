#include "PowerUpEntity.h"
#include "CollisionMask.h"

extern Sprite *multiBallSprites[];
extern Sprite* attachedBallSprites[];

extern b2World *world;

PowerUpEntity::PowerUpEntity()
{
	typeID = ENTITYPOWERUP;
	powerUpType = PowerUpType::MULTIBALL;

	for(int i = 0; i < 6; ++i)
		spriteList.push_back(multiBallSprites[i]);

	currentFrameNum = 0;
	sprite = spriteList[currentFrameNum];

	timeBetweenFrames = 1.0f / 10;
	frameTimer = 0;
}

void PowerUpEntity::Update(float seconds)
{
	frameTimer += seconds;

	//is it time to advance frames?
	while(frameTimer >= timeBetweenFrames)
	{
		frameTimer -= timeBetweenFrames;
		currentFrameNum++;
		if(currentFrameNum > 5) //loop the animation frames
			currentFrameNum = 0;

		sprite = spriteList[currentFrameNum];
	}
}

PowerUpEntity * MakePowerUp(PowerUpType type, float xpos, float ypos)
{
	PowerUpEntity *powerUpEntity = new PowerUpEntity();
	powerUpEntity->powerUpType = type;

	//set the sprites to draw with
	switch(type)
	{
	case PowerUpType::ATTACH:
		for (int i = 0; i < 6; ++i)
			powerUpEntity->spriteList[i] = attachedBallSprites[i];
		powerUpEntity->sprite = powerUpEntity->spriteList[0];
		break;
	case PowerUpType::MULTIBALL:
	default:
		for(int i = 0; i < 6; ++i)
			powerUpEntity->spriteList[i] = multiBallSprites[i];
		powerUpEntity->sprite = powerUpEntity->spriteList[0];
		break;
	}

	//make the physics body
	b2BodyDef powerUpBodyDef;

	//set the position of the center of the body, 
	//converting from pxel coords to physics measurements
	powerUpBodyDef.position.Set(xpos / PTM_RATIO, ypos / PTM_RATIO);
	powerUpBodyDef.type = b2_dynamicBody; //make it a dynamic body, so it can collide with the kinematic paddle

	//make the userdata point back to this entity
	powerUpBodyDef.userData.pointer = reinterpret_cast<uintptr_t>(powerUpEntity);

	powerUpEntity->body = world->CreateBody(&powerUpBodyDef); //create the body and add it to the world

	// Define a box shape for our dynamic body.
	b2PolygonShape boxShape;
	//SetAsBox() takes as arguments the half-width and half-height of the box
	boxShape.SetAsBox(42.0f / (2.f*PTM_RATIO), 20.0f / (2.f*PTM_RATIO));

	b2FixtureDef powerUpFixtureDef;
	powerUpFixtureDef.shape = &boxShape;
	powerUpFixtureDef.density = 1.0f; 
	powerUpFixtureDef.restitution = 0;
	powerUpFixtureDef.friction = 0.1f;

	//collison masking
	powerUpFixtureDef.filter.categoryBits = CMASK_POWERUP;  //this is a power up
	powerUpFixtureDef.filter.maskBits = CMASK_PADDLE | CMASK_GROUND;		//it collides wth paddle and ground

	powerUpEntity->body->CreateFixture(&powerUpFixtureDef);	

	//make the powerup drop
	
	powerUpEntity->body->SetLinearVelocity(b2Vec2(0.f, -30.f)); //apply velocity to kinematic object

	return powerUpEntity;
}