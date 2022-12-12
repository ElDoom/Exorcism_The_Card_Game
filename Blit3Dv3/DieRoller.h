#pragma once
#include <random>

class DieRoller
{
public:

	std::mt19937 rng;

	//TODO: add any other member variables you require here

	DieRoller(); //constructor that seeds the rng with a std::random_device
	void SeedRNG(unsigned int seedVal); //seeds the rng with the seedVal
	int RollDN(int N); // returns the result of an N-sided die roll (between 1 and N);
	int Roll4D6DropLowest(); // rolls 4 six-sided dice and sums the top 3, dropping the lowest roll
};

