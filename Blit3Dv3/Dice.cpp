#include "Dice.h"

Dice::Dice(int ID, std::vector<Sprite*> SL) 
{
	diceID = ID;
	spriteList = SL;

}

void Dice::Update(float seconds)
{
	//to write animation?
}

void Dice::Draw()
{
	spriteList[value]->Blit(posX, posY);
}
/*
private:
	int diceID;
public:
	int currentFrameNum;
	int value;
	float timeBetweenFrames;
	float frameTimer;
	std::vector<Sprite*> spriteList;
*/