#include "Ghost.h"
extern Blit3D* blit3D;

Ghost::Ghost(int ID, Sprite* SL)
{
	ghostID = ID;
	sprite = SL;

}

void Ghost::Update(float seconds)
{
	//to write animation?
}

void Ghost::Draw()
{
	sprite->Blit(blit3D->screenWidth / 2, (blit3D->screenHeight - 250));
}