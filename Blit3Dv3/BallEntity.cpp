#include "BallEntity.h"
#include "Blit3D.h"
#include "CollisionMask.h"

extern Blit3D *blit3D;
extern b2World *world;

void BallEntity::Draw() 
{
	//update the sprite locations based on the physics objects
	b2Vec2 bposition = body->GetPosition();
	bposition = Physics2Pixels(bposition);

	//get the angle of the ball
	float angle = body->GetAngle();
	angle = glm::degrees(angle);

	// Draw the objects
	sprite->angle = angle;
	sprite->Blit(bposition.x, bposition.y);
}

BallEntity * MakeBall(Sprite *sprite)
{
	//make a ball entity
	BallEntity *ballEntity = new BallEntity();

	// Define the Ball body. 
	//We set its position and call the body factory.
	b2BodyDef BallBodyDef;
	BallBodyDef.bullet = true; //balls move fast: make sure Continuos Collision Detection (CCD) is on
	BallBodyDef.type = b2_dynamicBody; //make it a dynamic body i.e. one moved by physics
	BallBodyDef.position.Set(0.0f, 0.0f); //set its position in the world
	BallBodyDef.angle = deg2rad(10.0f);

	//friction won't work on a rolling circle in Box2D, so apply angular damping to the body
	//to make it slow down as it rolls
	BallBodyDef.angularDamping = 1.8f;

	//make the userdata point back to this entity
	BallBodyDef.userData.pointer = reinterpret_cast<uintptr_t>(ballEntity);

	ballEntity->body = world->CreateBody(&BallBodyDef); //create the body and add it to the world

	// Define a ball shape for our body.
	//A circle shape for our ball
	b2CircleShape dynamicBall;
	dynamicBall.m_radius = 50.f / (2 * PTM_RATIO); //ball has diameter of 9 pixels

	//create the fixture definition - we don't need to save this
	b2FixtureDef fixtureDef;

	//collison masking
	fixtureDef.filter.categoryBits = CMASK_BALL;  //this is a ball
	fixtureDef.filter.maskBits = CMASK_BALL | CMASK_PADDLE | CMASK_EDGES | CMASK_BRICK | CMASK_GROUND;//it collides wth lotsa stuff

	// Define the dynamic body fixture.
	fixtureDef.shape = &dynamicBall;

	// Set the ball density to be non-zero, so it will be dynamic.
	//Remember, density * area determines the mass of the object
	fixtureDef.density = 1.0f;

	// Override the default friction.
	fixtureDef.friction = 0.0f;

	//restitution makes us bounce; use 0 for no bounce, 1 for perfect bounce
	fixtureDef.restitution = 1.0;

	//fixtureDef.

	// Add the shape to the body. 
	ballEntity->body->CreateFixture(&fixtureDef);

	//add a sprite to the ball entity
	ballEntity->sprite = sprite;

	return ballEntity;
}