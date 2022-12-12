#include "PaddleEntity.h"
#include "CollisionMask.h"

void PaddleEntity::HandleCollision(b2Body *ballBody)
{
	//fetch the current velocity vector of the ball
	b2Vec2 v = ballBody->GetLinearVelocity();
	//calculate it's length (speed)
	float speed = v.Length();
	//calculate how much bend we need based on how far from center we are
	position = body->GetPosition();
	b2Vec2 bpos = ballBody->GetPosition();
	float amount = (bpos.x - position.x) / halfWidth * bend;

	//normalize the current velocity, add the bend amount, renormalize, then rescale to speed
	v.Normalize();
	v.x += amount;
	v.Normalize();
	v *= speed;

	//set the velocity to the newly calculated velocity
	ballBody->SetLinearVelocity(v);
}

extern b2World *world;
extern Blit3D *blit3D;

PaddleEntity * MakePaddle(float xpos, float ypos, Sprite *sprite)
{
	PaddleEntity *paddleEntity = new PaddleEntity();

	
	paddleEntity->sprite = sprite;
	

	//make the physics body
	b2BodyDef paddleBodyDef;

	//set the position of the center of the body, 
	//converting from pxel coords to physics measurements
	paddleBodyDef.position.Set(xpos / PTM_RATIO, ypos / PTM_RATIO);
	paddleBodyDef.type = b2_kinematicBody; //make it a kinematic body i.e. one moved by us

	//make the userdata point back to this entity
	paddleBodyDef.userData.pointer = reinterpret_cast<uintptr_t>(paddleEntity);

	paddleEntity->body = world->CreateBody(&paddleBodyDef); //create the body and add it to the world

	// Define a box shape for our dynamic body.
	b2PolygonShape boxShape;
	//SetAsBox() takes as arguments the half-width and half-height of the box
	//boxShape.SetAsBox(42.0f / (2.f*PTM_RATIO), 11.0f / (2.f*PTM_RATIO));
	boxShape.SetAsBox(228.0f / (2.f * PTM_RATIO), 46.0f / (2.f * PTM_RATIO));

	b2FixtureDef paddleFixtureDef;
	paddleFixtureDef.shape = &boxShape;
	paddleFixtureDef.density = 1.0f; //won't matter, as we made this kinematic
	paddleFixtureDef.restitution = 0;
	paddleFixtureDef.friction = 0.1f;

	//collison masking
	paddleFixtureDef.filter.categoryBits = CMASK_PADDLE;  //this is the paddle
	paddleFixtureDef.filter.maskBits = CMASK_BALL | CMASK_POWERUP;//it collides with balls and powerups 

	paddleEntity->body->CreateFixture(&paddleFixtureDef);	

	return paddleEntity;
}