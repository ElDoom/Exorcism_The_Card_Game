#pragma once
#include "Blit3D.h"

class Dice{
private:
	int diceID;
public:
	int currentFrameNum = 0;
	int value = 0;
	float timeBetweenFrames;
	float frameTimer;
	int posX;
	int posY;
	std::vector<Sprite*> spriteList;
	Dice(int ID, std::vector<Sprite*> SL);
	void Update(float seconds);
	void Draw();

};
