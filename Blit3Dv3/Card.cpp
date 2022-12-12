#include "Card.h"
extern Blit3D* blit3D;

Card::Card(int ID, Sprite* SL)
{
	cardID = ID;
	sprite = SL;

}

void Card::Update(float seconds)
{
	//to write animation?
}

void Card::Draw()
{
	sprite->Blit(blit3D->screenWidth - 250, 250);
}