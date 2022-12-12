#pragma once
#include "Blit3D.h"

class Ghost {
private:
	int ghostID;
public:
	Sprite* sprite;
	int energy = 0;
	int hp = 0;
	std::string name;
	std::string description;
	Ghost(int ID, Sprite* SL);
	void Update(float seconds);
	void Draw();
};
