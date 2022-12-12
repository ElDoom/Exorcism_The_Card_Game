#pragma once
#include "Blit3D.h"

class Card {
private:
	int cardID;
public:
	Sprite* sprite;
	std::string name;
	std::string description;
	int hpPlayer = 0;
	int energyPlayer = 0;
	int damageHP = 0;
	int damageEnergy = 0;
	int cost = 0;
	
	Card(int ID, Sprite* SL);
	void Update(float seconds);
	void Draw();
};

/*
		name="Holly Molly",
		sprite="holly_molly.png",
		description="",
		hpPlayer=2,
		energyPlayer=0,
		damageHP=0,
		damageEnergy=0,
		cost=3;
*/